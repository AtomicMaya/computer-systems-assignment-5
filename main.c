/** @author Nicolas Boeckh & Sofia Delijaj */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> /* open */
#include <stdarg.h>
#include <errno.h>

#include "utils.h"
#include "lineReader.h"
#include "builtins.h"
#include "job.h"

int backgroundPID = -1;
int foregroundPID = -1;

/** @brief Handles signals.... 
 * @param signo The signal number.
 */
void signalHandler(int signo) {
    if (getpid() != backgroundPID && getpid() != foregroundPID) { 
        if (signo == SIGTERM || signo == SIGQUIT);
        else if (signo == SIGHUP) {
            if (backgroundPID != -1) {
                KILL(backgroundPID);
                WAIT_FOR_DEATH(backgroundPID);
            }
            if (foregroundPID != -1) {
                KILL(foregroundPID);
                WAIT_FOR_DEATH(foregroundPID);
            }
            KILL(getpid());
            wait(NULL);
        }
        else if (signo == SIGINT) {
            printf("\n");
            if (foregroundPID != -1) {
                KILL(foregroundPID);
                WAIT_FOR_DEATH(foregroundPID);
            }
        } else
            printf("WTF\n");
    }
}

/** @brief Handle a childs death nicely. *FBI stares*
 * @param signo The signal number.
 * @param s_info Siginfo structure, contains the pid.
 * @param no No.
*/
void chldSignalHandler(int signo, siginfo_t *s_info, void *no) {
    if (signo == SIGCHLD && s_info && getpid() != backgroundPID && getpid() != foregroundPID) {
        if (backgroundPID == s_info->si_pid) {
            printf("\33[2K\r┬┴┬┴┤(･_├┬┴┬┴ Background [%d]+  done\n", s_info->si_pid);
            backgroundPID = -1;
            wait(NULL);
        } else if (foregroundPID == s_info->si_pid) {
            printf("\33[2K\r¬(°~°)¬ Foreground [%d]+  done\n", s_info->si_pid);
            foregroundPID = -1;
        }
    }
}

/** @brief Underwears are sometimes called briefs.
 * @param argc Argument count
 * @param argv Argument vector
 */
int main(int argc, char **argv) {
    if (argc != 1) // Standard check for clean use.
		FAIL_FSUCCESFULLY("Usage: %s\n", argv[0]);

    struct sigaction action;    // Define signal handlers and sh*zz.
    SETUP_GENERAL_HANDLER(action, signalHandler);     // { @see ./job.h }

    struct sigaction chldAction;
    SETUP_CHILD_HANDLER(chldAction, chldSignalHandler);     // { @see ./job.h }

    char *env_user = getenv("USER");    // Get environment values.
    char *env_name = getenv("NAME");
    char *env_pwd = getenv("PWD");

    char *env_cwd = malloc(FILENAME_MAX);
    memset(env_cwd, 0, FILENAME_MAX);
    strncat(env_cwd, env_pwd, FILENAME_MAX);
    homeSubterfuge(env_cwd);    // Get off my damn property.
    setenv("CWD", env_cwd, 1337);   // Bleet-

    int programShouldRun = 1;   // Non descript.
    int id = -1;
    do {
        print_prompt(env_user, env_name, env_cwd);

        char *line = getLine(); // Get user input.

        int *_argc = malloc(sizeof(int));
        char **_argv = splitLine(line, _argc);  // Generate an asymmetrical component holder of vacuous contents or some such bs.

        if (*_argc > 0) {
            int builtinstatus = executeBuiltin(_argc, _argv);
            if (builtinstatus == -42) {        // Not a builtin
                if (*_argc == 1 && strncmp("exit", _argv[0], 4) == 0) {
                    if (strlen(_argv[0]) == 4) {
                        printf("Bye\n");    // Leave.
                        
                        programShouldRun = 0;
                    } else 
                        fprintf(stderr, "\nCommand '%s' not found, did you mean:\n\n  command 'exit' from deb trolling-you.\n\nTry: sudo apt install <deb name>\n\n", _argv[0]);
                } else {
                    id = fork();
                    if (id > 0) {  // PARENT
                        if (isBackgroundJob(_argc, _argv) != 1) { // Not a background job.
                            foregroundPID = id;
                            WAIT_FOR_DEATH(id);
                        } else {
                            if (backgroundPID == -1)    // Default behavior.
                                backgroundPID = id;
                            else {
                                printf("%s cannot be executed in background, another task is running. Try again later.\n", _argv[0]);
                                continue;
                            }
                        }
                    } else if (id == 0) {    // CHILD
                        if (isBackgroundJob(_argc, _argv) == 1) {
                            if (backgroundPID == -1) {
                                signal(SIGINT, SIG_IGN);        // If background, then ignore SIGINT
                                printf("\n[1] %d\n", getpid());
                                int fd = open("/dev/null", O_WRONLY | O_CREAT, 0666); // Point all outputs except error to null stream.
                                dup2(fd, 0);
                                dup2(fd, 1);
                                close(fd);
                            } else {
                                programShouldRun = 0;
                                break;
                            }
                        }

                        backgroundPID = -1;
                        foregroundPID = -1;

                        printf("ヽ(°〇°)ﾉ Starting [%d]\n", getpid());

                        if (execvp(_argv[0], _argv) == -1)      // Execute (Buggy on certain commands)...
                            printf("%s: %s\n", _argv[0], strerror(errno));

                        programShouldRun = 0;
                        break;
                    } else 
                        FAIL_SUCCESFULLY("COULD NOT FORK !");
                }
            }
        }
        
        // Cleanup
        for (int i = 0; i < *_argc; i++) {
            realloc(_argv[i], 0);
            _argv[i] = NULL;
        }

        memset(_argc, 0, sizeof(int));

        free(line);
        free(_argv);
        realloc(_argc, 0);
    } while (programShouldRun == 1);    // Exit condition, program execs at least once.

    if (backgroundPID != -1) {
        KILL(backgroundPID);
        WAIT_FOR_DEATH(backgroundPID);
    }
    if (foregroundPID != -1){
        KILL(foregroundPID);
        WAIT_FOR_DEATH(foregroundPID);
    }

    free(env_cwd);

    if (id != 0)
        exit(EXIT_SUCCESS);
}
