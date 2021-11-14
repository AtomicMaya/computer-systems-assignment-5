/** @author Nicolas Boeckh & Sofia Delijaj */

#ifndef HEADER_STEVE        /* I don't have a clue either why I name it STEVE */
#define HEADER_STEVE


int isBackgroundJob(int *_argc, char **_argv);

/** @brief Tries to kill a process at a certain PID, first gently, then less gently, then not.
 * @param pid The pid of the process to be killed.
 */
#define KILL(pid) {\
if (pid != -1)\
    if (kill(pid, SIGKILL) == -1)\
        FAIL_SUCCESFULLY("Immortal process, what's the thing about that ?\n");\
}

/** @brief Sets up a handler for the parent process.
 * @param action The concerned sigaction
 * @param func The callback function.
 */
#define SETUP_GENERAL_HANDLER(action, func) {\
memset(&action, 0, sizeof(action));\
action.sa_handler = func;\
sigaction(SIGINT, &action, NULL);\
}

/** @brief Sets up a handler for the child processes.
 * @param action The concerned sigaction
 * @param func The callback function.
 */
#define SETUP_CHILD_HANDLER(action, func) {\
memset(&action, 0, sizeof(action));\
action.sa_flags = SA_SIGINFO | SA_RESTART; /* Child will alert. */\
action.sa_sigaction = func;\
sigaction(SIGCHLD, &action, NULL);\
sigaction(SIGINT, &action, NULL);\
}

/** @brief Forces the parent to wait for the child's death.
 * @param pid The concerned pid.
 */
#define WAIT_FOR_DEATH(pid) {\
int waitStatus;\
do {    /* Wait for job to finish (I think). */\
    waitpid(pid, &waitStatus, WUNTRACED);\
} while (!WIFEXITED(waitStatus) && !WIFSIGNALED(waitStatus));\
}

#endif