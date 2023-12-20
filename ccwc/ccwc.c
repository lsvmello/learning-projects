#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>

void print_usage(void) {
  printf("\
Usage: ccwc [OPTION]... [FILE]\n\
Print newline, word and byte counts of the specified FILE.\n\n\
If no FILE is specified then read stardart input.\n\n\
The options below may be used to select which counts are printed.\n\
  -c     print the byte count\n\
  -h     display this help and exit\n\
  -l     print the newline count\n\
  -m     print the character count\n\
  -w     print the word count\n\
");
}

int main(int argc, char** argv) {
  setlocale(LC_ALL, "");

  bool print_bytes = false;
  bool print_chars = false;
  bool print_lines = false;
  bool print_words = false;

  char *file_name = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "-chlmw")) != -1) {
    switch(opt)
    {
      case 'c':
        print_bytes = true;
        break;
      case 'l':
        print_lines = true;
        break;
      case 'm':
        print_chars = true;
        break;
      case 'w':
        print_words = true;
        break;
      case 'h':
        print_usage();
        exit(EXIT_SUCCESS);
      case '?':
        printf("Try '--help' option for more information.\n");
        exit(EXIT_FAILURE);
      default:
        file_name = optarg;
        break;
    }

    if (optind < argc) {
      file_name = argv[optind];
    }
    FILE *file = NULL;
    if (file_name) {
      file = fopen(file_name, "r");
      if (file == NULL) {
        printf("An error has ocurred: %i", errno);
        exit(errno);
      }
    } else {
      file = stdin;
    }

    unsigned long long byte_count = 0;
    unsigned long long char_count = 0;
    unsigned long long line_count = 0;
    unsigned long long word_count = 0;

    wint_t c;
    char buff[8];
    bool in_word = false;
    while (WEOF != (c = fgetwc(file))) {

      char_count++;
      byte_count += wctomb(buff, c);

      if (c == L'\n') line_count++;
      if (iswspace(c)) {
        if (in_word) {
          word_count++;
          in_word = false;
        }
      } else {
        in_word = true;
      }
    }
    fclose(file);

    if (print_bytes) {
      printf("%llu", byte_count);
    }
    if (print_lines) {
      printf("%llu", line_count);
    }
    if (print_words) {
      printf("%llu", word_count);
    }
    if (print_chars) {
      printf("%llu", char_count);
    }
    if (!print_bytes && !print_chars && !print_lines && !print_words) {
      printf("  %llu  %llu %llu", line_count, word_count, byte_count);
    }
    printf(" %s\n", file_name != NULL ? file_name : "");
  }
  exit(EXIT_SUCCESS);
}
