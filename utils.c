/** @author Nicolas Boeckh & Sofia Delijaj */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>

#include "utils.h"

/** @brief Replaces "/home/<username>" with "~" (nice emoji action)
 * @param env_cwd The current process working directory.
 */
void homeSubterfuge(char *env_cwd) {
    char *env_home = getenv("HOME");
    char *cwd_buffer = malloc(STR_BUFFER_SIZE);

    if (strncmp(env_home, env_cwd, strlen(env_home)) == 0) {    // Check if similar or dissimilar
        int i = 0;
        while (env_home[i] == env_cwd[i])   // Get the index of the last "/". 
            i++;
        snprintf(cwd_buffer, 2, "%s", "~"); // Fool everyone, including myself, multiple times.
        for (int j = i; j < strlen(env_cwd); j++) { // Cleanup
            char *charBuffer = malloc(2);
            snprintf(charBuffer, 2, "%c", env_cwd[j]);
            strncat(cwd_buffer, charBuffer, STR_BUFFER_SIZE);
            free(charBuffer);
        }

        int size = strlen(env_cwd);
        memset(env_cwd, 0, size);
        strncat(env_cwd, cwd_buffer, size);
    }
    
    free(cwd_buffer);
}

/** Allows fancy color printouts to the console.
 * @param string    The string to funky print
 * @param ANSI_FG_COLOR A foreground ANSI setting
 * @param ANSI_BG_COLOR A background ANSI setting
 * @param ANSI_DECO A decoration (underline, bold, etc.) ANSI setting
 * @param newline Whether a newline should be printed at the end.
 */ 
void printColorized(char *string, int ANSI_FGCOLOR, int ANSI_BGCOLOR, int ANSI_DECO, int newLine) {
    printf("\033[%i;%i;%im%s\033[0m%s", ANSI_DECO, ANSI_BGCOLOR, ANSI_FGCOLOR, string, (newLine == 1) ? "\n\0" : "\0");
}

/** @brief Prints a nice prompt. You can't fool me twice... Actually I have fooled myself already 7 times today.
 * @param env_user The user name
 * @param env_name The PC name
 * @param env_cwd The current working directory
 */
void print_prompt(char *env_user, char *env_name, char *env_cwd) {
    char *curr = malloc(STR_BUFFER_SIZE);
    snprintf(curr, STR_BUFFER_SIZE, "\33[2K\r%s@%s", env_user, env_name);

    printColorized(curr, ANSI_FGCOLOR_GREEN, ANSI_BGCOLOR_BLACK, ANSI_DECO_BOLD, 0);
    printf(":");
    printColorized(getenv("CWD"), ANSI_FGCOLOR_CYAN, ANSI_BGCOLOR_BLACK, ANSI_DECO_BOLD, 0);
    printf("$ ");
    
    free(curr);
}

/** @brief Imagine strncat and snprintf had a baby. Now imagine this being an original idea (is it working ?).
 * @param src The source buffer
 * @param max_size The maximum size of the buffer
 * @param fmt The format string
 * @param ... Optargs
 */
void strfncat(char *src, int max_size, char *fmt, ...) {
    char buffer[max_size];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    strncat(src, buffer, max_size);
}
