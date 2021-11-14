/** @author Nicolas Boeckh & Sofia Delijaj */
#ifndef UTILS
#define UTILS

#include <stdio.h> /*   fprintf     */

#define	EXIT_FAILURE	1   // Instead of including the entirety of stdlib...
#define	EXIT_SUCCESS	0   // ...
#define BUFFER_SIZE     1024
#define STR_BUFFER_SIZE 256

#define ANSI_FGCOLOR_BLACK      30
#define ANSI_FGCOLOR_RED        31
#define ANSI_FGCOLOR_GREEN      32
#define ANSI_FGCOLOR_YELLOW     33
#define ANSI_FGCOLOR_BLUE       34
#define ANSI_FGCOLOR_MAGENTA    35
#define ANSI_FGCOLOR_CYAN       36
#define ANSI_FGCOLOR_WHITE      37
#define ANSI_FGCOLOR_BLACK_B    90
#define ANSI_FGCOLOR_RED_B      91
#define ANSI_FGCOLOR_GREEN_B    92
#define ANSI_FGCOLOR_YELLOW_B   93
#define ANSI_FGCOLOR_BLUE_B     94
#define ANSI_FGCOLOR_MAGENTA_B  95
#define ANSI_FGCOLOR_CYAN_B     96
#define ANSI_FGCOLOR_WHITE_B    97

#define ANSI_BGCOLOR_BLACK      40
#define ANSI_BGCOLOR_RED        41
#define ANSI_BGCOLOR_GREEN      42
#define ANSI_BGCOLOR_YELLOW     43
#define ANSI_BGCOLOR_BLUE       44
#define ANSI_BGCOLOR_MAGENTA    45
#define ANSI_BGCOLOR_CYAN       46
#define ANSI_BGCOLOR_WHITE      47
#define ANSI_BGCOLOR_BLACK_B    40
#define ANSI_BGCOLOR_RED_B     101
#define ANSI_BGCOLOR_GREEN_B   102
#define ANSI_BGCOLOR_YELLOW_B  103
#define ANSI_BGCOLOR_BLUE_B    104
#define ANSI_BGCOLOR_MAGENTA_B 105
#define ANSI_BGCOLOR_CYAN_B    106
#define ANSI_BGCOLOR_WHITE_B   107

#define ANSI_DECO_NONE         0
#define ANSI_DECO_BOLD         1
#define ANSI_DECO_FAINT        2
#define ANSI_DECO_ITALIC       3
#define ANSI_DECO_UNDERLINE    4

#define FAIL_SUCCESFULLY(msg)       { fprintf(stderr, msg); exit(EXIT_FAILURE); }
#define FAIL_FSUCCESFULLY(msg, ...) { fprintf(stderr, msg, __VA_ARGS__); exit(EXIT_FAILURE); }

void print_prompt(char *env_user, char *env_name, char *env_pwd);

void printColorized(char *string, int ANSI_FGCOLOR, int ANSI_BGCOLOR, int ANSI_DECO, int newLine);

void homeSubterfuge(char *env_pwd);

void strfncat(char *src, int max_size, char *fmt, ...);

#endif