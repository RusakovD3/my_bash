#include "m_bash_source.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

int execute_command(char *args[]) {
    int status;
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
    return status;
}

int my_bash() {
    char input[MAX_INPUT_SIZE];
    char *left_args[MAX_ARGS];
    char *right_args[MAX_ARGS];
    char *token;
    int has_pipe = 0, pipefd[2];
    pid_t pid;

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

        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Проверка наличия канала
        char *pipe_pos = strchr(input, '|');
        if (pipe_pos != NULL) {
            *pipe_pos = '\0'; // Разделяем на две строки
            has_pipe = 1;
        }

        // Разбор первой части команды
        int arg_count = 0;
        token = strtok(input, " ");
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            left_args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        left_args[arg_count] = NULL;

        if (has_pipe) {
            // Разбор второй части команды, если есть канал
            arg_count = 0;
            token = strtok(pipe_pos + 1, " ");
            while (token != NULL && arg_count < MAX_ARGS - 1) {
                right_args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            right_args[arg_count] = NULL;

            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid = fork();
        if (pid == 0) {
            if (has_pipe) {
                close(pipefd[0]); // Закрываем для чтения в дочернем процессе
                dup2(pipefd[1], STDOUT_FILENO); // Перенаправляем stdout в канал
                close(pipefd[1]); // Закрытие дескриптора после дупликации
                execute_command(left_args);
                exit(EXIT_SUCCESS);
            } else {
                execute_command(left_args);
                exit(EXIT_SUCCESS);
            }
        } else if (pid > 0) {
            if (has_pipe) {
                close(pipefd[1]); // Закрываем для записи в родительском процессе
                wait(NULL); // Ждем завершения первой команды

                pid = fork(); // Создаем новый дочерний процесс для второй команды
                if (pid == 0) {
                    dup2(pipefd[0], STDIN_FILENO); // Перенаправляем stdin из канала
                    close(pipefd[0]);
                    execute_command(right_args);
                    exit(EXIT_SUCCESS);
                } else if (pid > 0) {
                    close(pipefd[0]);
                    wait(NULL); // Ждем завершения второй команды
                }
            } else {
                wait(NULL); // Ждем завершения команды, если нет канала
            }
        } else {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        has_pipe = 0; // Сброс флага для следующей итерации
    }
    exit(EXIT_SUCCESS);
}
