#ifndef SRC_GREP_MY_GREP_H_
#define SRC_GREP_MY_GREP_H_

#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME 0
#define FLAG 1
#define PATTERN 2
#define PATH_TO_FILE 3
#define PATTERN_FILE 4

struct grep_flag_type {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
};

typedef struct grep_flag_type flag_t;

void read_arguments(int argc, char* argv[], int* arg_list, flag_t* flags);
int read_flags(char* str, flag_t* flags);
char** get_patterns(int* counter_of_patterns, char* argv[], int argc,
                    int* const arg_list);
char* set_pattern(char* source, char flag);
int get_count_of_patterns(int argc, int* const arg_list);
int check_flags(flag_t* flags, int* const arg_list, int argc);
int counter_of_lines(char* path_to_file);
void output(int argc, char* argv[], int* arg_list, flag_t* flags,
            char** patterns, int counter_of_patterns);
void print_file(char* path_to_file, char** patterns, int counter_of_patterns,
                flag_t* flags);
void compile_regular_expression(regex_t* reg, int ignore_case, char* patterns[],
                                int counter_of_patterns);
void print_line(char* line, int* counter_of_lines, char* path_to_file,
                flag_t* flags, regex_t* reg, regmatch_t* str_match);

#endif  // SRC_GREP_MY_GREP_H_
