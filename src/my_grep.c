#include "my_grep.h"

int main(int argc, char* argv[]) {
  flag_t flags = {0};
  int* arg_list = calloc(argc, sizeof(int));
  if (arg_list == NULL) {
    fprintf(stderr, "my_grep: %s\n", strerror(errno));
    exit(errno);
  }

  char** patterns = NULL;
  int counter_of_patterns = 0;
  read_arguments(argc, argv, arg_list, &flags);
  patterns = get_patterns(&counter_of_patterns, argv, argc, arg_list);

  if (check_flags(&flags, arg_list, argc)) {
    output(argc, argv, arg_list, &flags, patterns, counter_of_patterns);
  } else {
    fprintf(stderr, "Usage: my_grep [OPTIONS]... PATTERNS [FILES]...\n");
  }

  free(arg_list);
  for (int i = 0; i < counter_of_patterns; i++) {
    free(patterns[i]);
  }
  free(patterns);
  return 0;
}

void read_arguments(int argc, char* argv[], int* arg_list, flag_t* flags) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      arg_list[i] = FLAG;
      int search_pattern = read_flags(argv[i], flags);
      if (search_pattern == 1) {
        if (i != argc - 1) {
          i++;
          arg_list[i] = PATTERN;
        } else {
          fprintf(stderr, "my_grep: option requires an argument -- e\n");
          exit(1);
        }
      }
      if (search_pattern == 2) {
        if (i != argc - 1) {
          i++;
          arg_list[i] = PATTERN_FILE;
        } else {
          fprintf(stderr, "my_grep: option requires an argument -- f\n");
          exit(1);
        }
      }
    } else {
      arg_list[i] = PATH_TO_FILE;
    }
  }
}

int read_flags(char* str, flag_t* flags) {
  str++;
  int result = 0;
  int read_end = 0;
  while (read_end == 0) {
    if (*str == 'e') {
      flags->e = 1;
      result = 1;
    } else if (*str == 'i') {
      flags->i = 1;
    } else if (*str == 'v') {
      flags->v = 1;
    } else if (*str == 'c') {
      flags->c = 1;
    } else if (*str == 'l') {
      flags->l = 1;
    } else if (*str == 'n') {
      flags->n = 1;
    } else if (*str == 'h') {
      flags->h = 1;
    } else if (*str == 's') {
      flags->s = 1;
    } else if (*str == 'f') {
      flags->f = 1;
      result = 2;
    } else if (*str == 'o') {
      flags->o = 1;
    } else if (*str == '\0') {
      read_end = 1;
    } else {
      fprintf(stderr, "my_grep: invalid option -- '%c'\n", *str);
      exit(1);
    }
    str++;
  }
  return result;
}

char** get_patterns(int* counter_of_patterns, char* argv[], int argc,
                    int* const arg_list) {
  char** patterns = NULL;

  *counter_of_patterns = get_count_of_patterns(argc, arg_list);
  if (counter_of_patterns != 0) {
    patterns = (char**)malloc(*counter_of_patterns * sizeof(char*));
    if (patterns == NULL) {
      fprintf(stderr, "my_grep: %s\n", strerror(errno));
      exit(errno);
    }
  }

  for (int i = 0; i < *counter_of_patterns;) {
    for (int j = 0; j < argc; j++) {
      if (arg_list[j] == PATTERN) {
        patterns[i] = set_pattern(argv[j], 'e');
        i++;
      }
      if (arg_list[j] == PATTERN_FILE) {
        patterns[i] = set_pattern(argv[j], 'f');
        i++;
      }
    }
  }
  return patterns;
}

char* set_pattern(char* source, char flag) {
  char* pattern = NULL;

  if (flag == 'f') {
    FILE* file;
    file = fopen(source, "r");
    if (file != NULL) {
      int count_of_lines = counter_of_lines(source);
      pattern = (char*)malloc(sizeof(char) * count_of_lines * 512);
      char* line = NULL;
      size_t line_size = 0;
      getline(&line, &line_size, file);
      strcpy(pattern, line);

      while (getline(&line, &line_size, file) != EOF) {
        strcat(pattern, line);
      }

      for (int i = 0; pattern[i] != '\0'; i++) {
        if (pattern[i] == '\n') {
          pattern[i] = '|';
        }
      }
      free(line);
    } else {
      fprintf(stderr, "my_grep: %s: %s\n", source, strerror(errno));
      exit(1);
    }
    fclose(file);
  }
  if (flag == 'e') {
    int size_of_pattern = strlen(source) + 1;
    pattern = (char*)malloc(sizeof(char) * size_of_pattern);
    if (pattern == NULL) {
      fprintf(stderr, "my_grep: %s\n", strerror(errno));
      exit(errno);
    }
    strcpy(pattern, source);
  }
  return pattern;
}

int get_count_of_patterns(int argc, int* const arg_list) {
  int counter_of_patterns = 0;
  for (int i = 0; i < argc; i++) {
    if ((arg_list[i] == PATTERN) || (arg_list[i] == PATTERN_FILE)) {
      counter_of_patterns += 1;
    }
  }
  if (counter_of_patterns == 0) {
    for (int i = 0; i < argc; i++) {
      if (arg_list[i] == PATH_TO_FILE) {
        arg_list[i] = PATTERN;
        counter_of_patterns = 1;
        break;
      }
    }
  }
  return counter_of_patterns;
}

int check_flags(flag_t* flags, int* const arg_list, int argc) {
  int counter_of_patterns = get_count_of_patterns(argc, arg_list);
  int counter_of_files = 0;
  for (int i = 0; i < argc; i++) {
    if (arg_list[i] == PATH_TO_FILE) {
      counter_of_files++;
    }
  }
  if (counter_of_files == 1) {
    flags->h = 1;
  }
  return (counter_of_files * counter_of_patterns);
}

int counter_of_lines(char* path_to_file) {
  int result = 0;
  FILE* file;
  file = fopen(path_to_file, "r");
  char* line = NULL;
  size_t size_of_line = 0;
  while (getline(&line, &size_of_line, file) != EOF) {
    result++;
  }
  fclose(file);
  free(line);
  return result;
}

void output(int argc, char* argv[], int* arg_list, flag_t* flags,
            char* patterns[], int counter_of_patterns) {
  for (int i = 1; i < argc; i++) {
    if (arg_list[i] == PATH_TO_FILE) {
      print_file(argv[i], patterns, counter_of_patterns, flags);
    }
  }
}

void print_file(char* path_to_file, char** patterns, int counter_of_patterns,
                flag_t* flags) {
  FILE* file;
  file = fopen(path_to_file, "r");
  if (file != NULL) {
    regex_t reg;
    compile_regular_expression(&reg, flags->i, patterns, counter_of_patterns);
    char* line = NULL;
    size_t line_size = 0;

    int counter_of_matches = 0;
    int counter_of_lines = 0;

    while (getline(&line, &line_size, file) != EOF) {
      counter_of_lines++;
      regmatch_t str_match[1];
      int result = regexec(&reg, line, 1, str_match, REG_NOTEOL);
      if (flags->v) {
        if (result == 0) {
          result = REG_NOMATCH;
        } else {
          result = 0;
        }
      }
      if (result == 0) {
        counter_of_matches++;
        if (!flags->l && !flags->c) {
          print_line(line, &counter_of_lines, path_to_file, flags, &reg,
                     str_match);
        }
      }
    }
    if (flags->l) {
      if (counter_of_matches > 0) {
        printf("%s\n", path_to_file);
      }
    } else if (flags->c) {
      if (!flags->h) {
        printf("%s:", path_to_file);
      }
      printf("%d\n", counter_of_matches);
    }
    regfree(&reg);
    free(line);
    fclose(file);
  } else {
    if (!flags->s) {
      fprintf(stderr, "my_grep: %s: %s\n", path_to_file, strerror(errno));
    }
  }
}

void compile_regular_expression(regex_t* reg, int ignore_case, char* patterns[],
                                int counter_of_patterns) {
  int regcomp_flag = REG_EXTENDED;
  if (ignore_case) {
    regcomp_flag = REG_ICASE | REG_EXTENDED;
  }

  int size_of_expression = counter_of_patterns;
  for (int i = 0; i < counter_of_patterns; i++) {
    size_of_expression += strlen(patterns[i]);
  }

  char* reg_expression = (char*)(malloc(size_of_expression * sizeof(char)));
  strcpy(reg_expression, patterns[0]);
  for (int i = 1; i < counter_of_patterns; i++) {
    strcat(reg_expression, "|");
    strcat(reg_expression, patterns[i]);
  }

  regcomp(reg, reg_expression, regcomp_flag);
  free(reg_expression);
}

void print_line(char* line, int* counter_of_lines, char* path_to_file,
                flag_t* flags, regex_t* reg, regmatch_t* str_match) {
  if (!(flags->o && flags->v)) {
    if (!flags->h) {
      printf("%s:", path_to_file);
    }
    if (flags->n) {
      printf("%d:", *counter_of_lines);
    }
    if (flags->o) {
      for (int i = str_match->rm_so; i < str_match->rm_eo; i++) {
        printf("%c", line[i]);
      }
      printf("\n");
      line += str_match->rm_eo;
      if (regexec(reg, line, 1, str_match, 1) == 0) {
        print_line(line, counter_of_lines, path_to_file, flags, reg, str_match);
      }
    } else {
      printf("%s", line);
    }
    if (line[strlen(line) - 1] != '\n') {
      printf("\n");
    }
  }
}
