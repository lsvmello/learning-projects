#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// MinUnit testing framework. http://www.jera.com/techinfo/jtns/jtn002.html
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { printf("Running: "#test"\n"); char *message = test(); \
                              tests_run++; if (message) return message; } while (0)
#define strings_equal(a, b) strcmp(a, b) == 0

int tests_run = 0;

#define STACK_MAX 256
#define INITIAL_GC_THRESHOLD 8

typedef enum {
  OBJ_INT,
  OBJ_PAIR
} ObjectType;

typedef struct sObject {
  ObjectType type;
  bool marked;

  /* The next object in the linked list of heap allocated objects. */
  struct sObject *next;

  union {
    /* OBJ_INT */
    int value;

    /* OBJ_PAIR */
    struct {
      struct sObject *head;
      struct sObject *tail;
    };
  };
} Object;

typedef struct {
  Object *stack[STACK_MAX];
  int stackSize;

  /* The first object in the linked list of all objects on the heap. */
  Object *firstObject;

  /* The total number of currently allocated objects. */
  int numObjects;

  /* The number of objects required to trigger a GC. */
  int maxObjects;
} VM;

void assert(bool condition, const char *message) {
  if (!condition) {
    printf("%s\n", message);
    exit(1);
  }
}

VM *newVM() {
  VM *vm = malloc(sizeof(VM));
  vm->stackSize = 0;
  vm->firstObject = NULL;
  vm->numObjects = 0;
  vm->maxObjects = INITIAL_GC_THRESHOLD;
  return vm;
}

void push(VM *vm, Object *value) {
  assert(vm->stackSize < STACK_MAX, "Stack overflow!");
  vm->stack[vm->stackSize++] = value;
}

Object *pop(VM *vm) {
  assert(vm->stackSize > 0, "Stack underflow!");
  return vm->stack[--vm->stackSize];
}

void mark(Object *object) {
   /* If already marked, we're done. Check this first to avoid recursing
      on cycles in the object graph. */
  if (object->marked) return;

  object->marked = true;

  if (object->type == OBJ_PAIR) {
    mark(object->head);
    mark(object->tail);
  }
}

void markAll(VM *vm) {
  for (int i = 0; i < vm->stackSize; i++) {
    mark(vm->stack[i]);
  }
}

void sweep(VM *vm) {
  Object **object = &vm->firstObject;
  while(*object) {
    if (!(*object)->marked) {
      /* This object wasn't reached, so remove it from the list and free it. */
      Object *unreached = *object;

      *object = unreached->next;
      free(unreached);
      vm->numObjects--;
    } else {
      /* This object was reached, so unmark it (for the next GC) and move on to
         the next. */
      (*object)->marked = false;
      object = &(*object)->next;
    }
  }
}

void gc(VM *vm, bool debug) {
  int numObjects = vm->numObjects;

  markAll(vm);
  sweep(vm);

  vm->maxObjects = vm->numObjects == 0 ? INITIAL_GC_THRESHOLD : vm->numObjects * 2;

  if (debug) printf("Collected %d objects, %d remaining.\n", numObjects - vm->numObjects, vm->numObjects);
}

Object *newObject(VM *vm, ObjectType type) {
  if (vm->numObjects == vm->maxObjects) gc(vm, true);

  Object *object = malloc(sizeof(Object));
  object->type = type;
  object->marked = false;

  object->next = vm->firstObject;
  vm->firstObject = object;
  vm->numObjects++;

  return object;
}

void pushInt(VM *vm, int intValue) {
  Object *object = newObject(vm, OBJ_INT);
  object->value = intValue;

  push(vm, object);
}

Object *pushPair(VM *vm) {
  Object *object = newObject(vm, OBJ_PAIR);
  object->tail = pop(vm);
  object->head = pop(vm);

  push(vm, object);
  return object;
}

void objectPrint(const Object *object) {
  switch (object->type) {
    case OBJ_INT:
      printf("%d", object->value);
      break;
    case OBJ_PAIR:
      printf("(");
      objectPrint(object->head);
      printf(", ");
      objectPrint(object->tail);
      printf(")");
      break;
  }
}

void freeVM(VM *vm) {
  vm->stackSize = 0;
  gc(vm, false);
  free(vm);
}

char *test_objects_on_stack_are_preserved() {
  VM *vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);

  gc(vm, true);
  mu_assert("Should have preserved objects.", vm->numObjects == 2);
  freeVM(vm);

  return 0;
}

char *test_unreached_objects_are_collected() {
  VM* vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);
  pop(vm);
  pop(vm);

  gc(vm, true);
  mu_assert("Should have collected objects.", vm->numObjects == 0);
  freeVM(vm);

  return 0;
}

char *test_reach_nested_objects() {
  VM* vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);
  pushPair(vm);
  pushInt(vm, 3);
  pushInt(vm, 4);
  pushPair(vm);
  pushPair(vm);

  gc(vm, true);
  mu_assert("Should have reached objects.", vm->numObjects == 7);
  freeVM(vm);

  return 0;
}

char *test_handle_cycles() {
  VM* vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);
  Object* a = pushPair(vm);
  pushInt(vm, 3);
  pushInt(vm, 4);
  Object* b = pushPair(vm);

  /* Set up a cycle, and also make 2 and 4 unreachable and collectible. */
  a->tail = b;
  b->tail = a;

  gc(vm, true);
  mu_assert("Should have collected objects.", vm->numObjects == 4);
  freeVM(vm);

  return 0;
}

char *test_performance() {
  VM* vm = newVM();

  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 20; j++) {
      pushInt(vm, i);
    }

    for (int k = 0; k < 20; k++) {
      pop(vm);
    }
  }
  freeVM(vm);

  return 0;
}

static char *all_tests() {
  mu_run_test(test_objects_on_stack_are_preserved);
  mu_run_test(test_unreached_objects_are_collected);
  mu_run_test(test_reach_nested_objects);
  mu_run_test(test_handle_cycles);

  /* mu_run_test(test_performance); */

  return 0;
}

int main() {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("all tests passed\n");
  }
  printf("%d tests run\n", tests_run);
  return result != 0;
}
