#include "m_bash_source.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

int my_bash() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    char *token;
    int status;

    while (1) {
        printf("mybash$ ");
        fflush(stdout);

        if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
            perror("Input error!\n");
            break;
        }
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0'; // Удаление новой строки в конце ввода
        }

        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            // Выход из интерпретатора
            break;
        }

        // Разбор ввода на программу и аргументы
        int arg_count = 0;
        token = strtok(input, " ");
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL; // Последний аргумент должен быть NULL для execvp

        pid_t pid = fork();
        if (pid == 0) {
            // Дочерний процесс
            if (execvp(args[0], args) == -1) {
                perror("execvp error");
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) {
            // Родительский процесс
            waitpid(pid, &status, 0);
        } else {
            // Ошибка fork
            perror("fork error");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
