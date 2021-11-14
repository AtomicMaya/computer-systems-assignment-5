/** @author Nicolas Boeckh & Sofia Delijaj */

#define _GNU_SOURCE

#include <sys/stat.h>   /* lstat */
#include <stdlib.h>     /* malloc, FILENAME_MAX */
#include <string.h>
#include <unistd.h>     /* chdir, getcwd */
#include <errno.h>
#include <stdlib.h>     /* setenv */

#include "builtins.h"
#include "utils.h"

/** @brief Write the content of tempPath to path, set length and index accordingly.
 * @param path The variable that stores the original path
 * @param tempPath The variable that stores the modified path
 * @param len The length of the new path
 * @param i The index of the iteration
 */
#define REWRITE(path, tempPath, len, i) {\
memset(path, 0, FILENAME_MAX);\
snprintf(path, FILENAME_MAX, "%s", tempPath);\
len = strlen(path);\
i = -1; /* Because i++ happens on next loop for it to reset to 0. Gave me a little headache... */\
free(tempPath);\
setenv("CWD", path, 1);\
}

/** @brief Builtin cd, handles soft links rather than screwing up using realpath (Not pwetty)
 * @param _argc Argument count
 * @param _argv The argument vector
 * @return The status of the command (0 if cool).
 */
int changeDirectory(int *_argc, char **_argv) {
    if (*_argc > 2) {   // Skip if too many arguments
        fprintf(stderr, "-bash: cd: too many arguments\n");
        return -1;
    }
    
    char *path = malloc(FILENAME_MAX);
    memset(path, 0, FILENAME_MAX);
    snprintf(path, FILENAME_MAX, "%s", ((_argv[1]) ? _argv[1] : getenv("HOME"))); // if not given, assume user home
    
    char *pathDup = malloc(FILENAME_MAX); // Error fallback
    memset(pathDup, 0, FILENAME_MAX);
    snprintf(pathDup, FILENAME_MAX, "%s", getenv("CWD")); // if not given, assume user home

    int len = strlen(path);
    for (int i = 0; i < len; i++) {
        //printf("%s %d %c\n", path, i, path[i]); // Debug statement...

        char *tempPath = malloc(FILENAME_MAX);
        memset(tempPath, 0, FILENAME_MAX);
        if (path[len - 1] == '/' && len > 2)
            path[len - 1] = '\0';

        if(strlen(path) == 4 && strncmp(path, "//..", 4) == 0) { // Edge case. Nothing to see here.
            strfncat(tempPath, FILENAME_MAX, "%s", "//");
            REWRITE(path, tempPath, len, i);
            break;
        } 
        
        if (i == 0 && path[0] == '~') { // ~ not recognized as home, needs to be set /home/<username> manually.
            strfncat(tempPath, FILENAME_MAX, "%s%s", getenv("HOME"), path + 1);
            REWRITE(path, tempPath, len, i);

        } else if (i == 0 && path[i] == '/' && path[i + 1] && path[i + 1] == '/') {
            // Bash default behavior, /x -> /x, //x -> //x, ///x -> /x, //.../ -> /x
            int count = 1;
            for (int j = 0; j < len; j++) { // Count the slashes
                if (path[j + 1] && path[j + 1] == '/') 
                    count++;
                else
                    break;
            }
            
            strfncat(tempPath, FILENAME_MAX, "%s%s\0", (count != 2) ? "/" : "//", path + count); // If 2 slashes, set 2 slashes, else set 1.

            REWRITE(path, tempPath, len, i);

        } else if (path[i] == '.' && (path[i + 1] == 0 || path[i + 1] == '/' || path[i + 1] == '\0') && path[i - 1] != '.')  { // Path has . or ./ in it.
            if (i == 0) {   // Path starts with no relative values.
                strfncat(tempPath, FILENAME_MAX, "%s%s%s", getenv("CWD"), (getenv("CWD")[0] == '/' && strlen(getenv("CWD")) == 1) ? "" : "/", path);
                REWRITE(path, tempPath, len, i);
                continue;
            }
            strfncat(tempPath, i, "%s", path);
            strfncat(tempPath, FILENAME_MAX, "%s", path + i + 2);   // Skip stuff and stuff.
            
            REWRITE(path, tempPath, len, i);
        } else if (path[i] == '.' && path[i + 1] && path[i + 1] == '.') { // If points to parent
            if (i == 0) {   // Path starts with no relative values.
                strfncat(tempPath, FILENAME_MAX, "%s%s%s", getenv("CWD"), (getenv("CWD")[0] == '/' && strlen(getenv("CWD")) == 1) ? "" : "/", path);
                REWRITE(path, tempPath, len, i);
                continue;
            }

            int count = 0;
            int j = i;
            for ( ; count != 2 && j >= 0; j--)
                if (path[j] == '/') // Count the index back 2 slashes.
                    count++;

            j += 1;
            if (count >= 2) {
                strfncat(tempPath, j + 1, "%s", path);
                strfncat(tempPath, FILENAME_MAX - (j + 1), "%s", (path + i + 2) ? path + i + 3 : "");
                REWRITE(path, tempPath, len, i);
            } else {
                strfncat(tempPath, FILENAME_MAX, "/", path);
                REWRITE(path, tempPath, len, i);
            }
        } else if (strchr(path, '/') == NULL) {
            strfncat(tempPath, FILENAME_MAX, "%s%s%s", getenv("CWD"), (getenv("CWD")[0] == '/' && strlen(getenv("CWD")) == 1) ? "" : "/", path);
            REWRITE(path, tempPath, len, i);
            continue;
        } else {
            free(tempPath);
        }
    }

    strncat(path, "\0", FILENAME_MAX);  // Terminate the string

    struct stat* statBuffer = malloc(sizeof(struct stat));  // Redundancy checks
    int status = -1;

    if (lstat(path, statBuffer) == 0)   // Check whether it's a DIR or a symbolic link.
        if (S_ISDIR(statBuffer->st_mode) || S_ISLNK(statBuffer->st_mode)) {
            status = chdir(path);  
            if (status == 0) {                
                homeSubterfuge(path);
                setenv("CWD", path, strlen("naan zero"));
            } else {
                fprintf(stderr, "%s\n", strerror(errno));
                setenv("CWD", pathDup, strlen("error case"));
            }
        } else {
            fprintf(stderr, "-bash: cd: %s: Not a directory\n", path);
                setenv("CWD", pathDup, strlen("error case"));
        }
    else {
        fprintf(stderr, "-bash: cd: %s: No such file or directory\n", path);
        setenv("CWD", pathDup, strlen("error case"));
    }

    free(statBuffer);   // Clean stuff.
    free(path);
    free(pathDup);
    return status;
}

/** @brief Prints out the builtins
 * @param _argc Useless
 * @param _argv Useless
 * @return This can't fail
 */
int getHelp(int *_argc, char **_argv) {
    printf("Built-ins :\n- help\n- cd <dir>\n");
    return 0;
}

/** @brief How intuitively well-named can a function be ?
 * @param _argv The arguments vector.
 */
int executeBuiltin(int *_argc, char **_argv) {
    for (int i = 0; i < BUILTINS_COUNT; i++)
        if (strcmp(_argv[0], builtins[i]) == 0) 
            return (*builtin[i])(_argc, _argv);
    return -42;
}

/** Define the functions */
int (*builtin[]) (int *, char **) = {
    &changeDirectory,
    &getHelp
};

/** Define the builtins */
char *builtins[] = {
    "cd",
    "help"
};