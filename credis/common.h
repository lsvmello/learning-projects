#pragma once

#include <stdint.h>
#include <stddef.h>

#define container_of(ptr, type, member) ({                 \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);   \
    (type *)( (char *)__mptr - offsetof(type, member) );})

inline uint64_t str_hash(const uint8_t *data, size_t len) {
  uint32_t h = 0x811C9DC5;
  for (size_t i = 0; i < len; ++i) {
    h = (h + data[i]) * 0x01000193;
  }
  return h;
}

enum {
  SER_NIL = 0, // Like `NULL`
  SER_ERR = 1, // An error code and message
  SER_STR = 2, // A string
  SER_INT = 3, // An int64
  SER_DBL = 4, // A double
  SER_ARR = 5, // An array
};

