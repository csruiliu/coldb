#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "utils_func.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG 1
#define LOG_ERR 1
#define LOG_INFO 1

/**
 * a hash function for kv store
 **/
int hash_func(const char* s, size_t a, size_t m) {
    long hash = 0;
    size_t len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

/**
 * Removes newline characters from the input string.
 * Shifts characters over and shortens the length of
 * the string by the number of newline characters.
 **/
char* trim_newline(char* str) {
    int length = strlen(str);
    int current = 0;
    for (int i = 0; i < length; ++i) {
        if (!(str[i] == '\r' || str[i] == '\n')) {
            str[current++] = str[i];
        }
    }
    // Write new null terminator
    str[current] = '\0';
    return str;
}

/**
 * Removes space characters from the input string.
 * Shifts characters over and shortens the length of
 * the string by the number of space characters.
 **/
char* trim_whitespace(char* str) {
    int length = strlen(str);
    int current = 0;
    for (int i = 0; i < length; ++i) {
        if (!isspace(str[i])) {
            str[current++] = str[i];
        }
    }
    // Write new null terminator
    str[current] = '\0';
    return str;
}

/**
 * Removes parenthesis characters from the input string.
 * Shifts characters over and shortens the length of
 * the string by the number of parenthesis characters.
 **/
char* trim_parenthesis(char* str) {
    int length = strlen(str);
    int current = 0;
    for (int i = 0; i < length; ++i) {
        if (!(str[i] == '(' || str[i] == ')')) {
            str[current++] = str[i];
        }
    }
    // Write new null terminator
    str[current] = '\0';
    return str;
}

/**
 * Removes quote characters from the input string.
 * Shifts characters over and shortens the length of
 * the string by the number of space characters.
 **/
char* trim_quote(char* str) {
    int length = strlen(str);
    int current = 0;
    for (int i = 0; i < length; ++i) {
        if (str[i] != '\"') {
            str[current++] = str[i];
        }
    }
    // Write new null terminator
    str[current] = '\0';
    return str;
}

/**
 * The following three functions will show output on the terminal
 * based off whether the corresponding level is defined.
 * To see log output, define LOG.
 * To see error output, define LOG_ERR.
 * To see info output, define LOG_INFO
 **/
void coldb_log(FILE* out, const char *format, ...) {
#ifdef LOG
    va_list v;
    va_start(v, format);
    vfprintf(out, format, v);
    va_end(v);
#else
    (void) out;
    (void) format;
#endif
}

void log_err(const char *format, ...) {
#ifdef LOG_ERR
    va_list v;
    va_start(v, format);
    fprintf(stderr, ANSI_COLOR_RED);
    vfprintf(stderr, format, v);
    fprintf(stderr, ANSI_COLOR_RESET);
    va_end(v);
#else
    (void) format;
#endif
}

void log_info(const char *format, ...) {
#ifdef LOG_INFO
    va_list v;
    va_start(v, format);
    fprintf(stdout, ANSI_COLOR_GREEN);
    vfprintf(stdout, format, v);
    fprintf(stdout, ANSI_COLOR_RESET);
    fflush(stdout);
    va_end(v);
#else
    (void) format;
#endif
}

