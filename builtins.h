/** @author Nicolas Boeckh & Sofia Delijaj */

#ifndef HEADER_BUILTINS
#define HEADER_BUILTINS

int changeDirectory(int *_argc, char **_argv);

int getHelp(int *_argc, char **_argv);

#define BUILTINS_COUNT 2

int (*builtin[BUILTINS_COUNT]) (int *, char **);

char *builtins[BUILTINS_COUNT];

int executeBuiltin(int *_argc, char **_argv);

#endif