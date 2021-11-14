/** @author Nicolas Boeckh & Sofia Delijaj */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/** @brief Checks whether the process is supposed to be run in the background and removes the token.
 * @param _argc See the main function.
 * @param _argv See the main function.
 */
int isBackgroundJob(int *_argc, char **_argv) {
    char *index = strrchr(_argv[*_argc - 1], '&');  // Checks if the last element contains an ampersand symbol.
    if (index) {    // If an ampersand exists.
        if (strlen(_argv[*_argc - 1]) == 1 && strncmp(_argv[*_argc - 1], "&", 1) == 0) {    // If it's the last, shrink the array.
            _argv[*_argc - 1][0] = '\0';
            memset(_argv[*_argc - 1], 0, FILENAME_MAX);
            _argv[*_argc - 1] = NULL;   // Somehow this removed most of the leaky memory... I don't get it either.
            *_argc -= 1;
        } else // replace the character
            _argv[*_argc - 1][index - _argv[*_argc - 1]] = '\0';
        return 1;   // Confirm
    }

    return 0;   // Deny
}
