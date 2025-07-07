#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_FILE "output.txt"

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <timeout> <command> [arguments...]\n", argv[0]);
    exit(1);
  }

  int timeout = atoi(argv[2]);
  if (timeout <= 0) {
    fprintf(stderr, "Timeout must be a positive number.\n");
    exit(1);
  }

  printf("Starting timeout: %d seconds\n", timeout);

  char **command_args = &argv[3];

  int output_fd = open(OUTPUT_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (output_fd == -1) {
    perror("Failed to open output file");
    exit(1);
  }

  pid_t command_pid = fork();
  if (command_pid == 0) {
    // Redirect stdout and stderr to the output file
    dup2(output_fd, STDOUT_FILENO);
    dup2(output_fd, STDERR_FILENO);
    close(output_fd);

    printf("Executing command: %s\n", command_args[0]);
    execvp(command_args[0], command_args);
    perror("Failed to execute command");
    exit(1);
  }
  close(output_fd);

  pid_t sleep_pid = fork();
  if (sleep_pid == 0) {
    sleep(timeout);
    exit(0);
  }

  int status;
  pid_t finished_pid = waitpid(-1, &status, 0);

  if (finished_pid == command_pid) {
    printf("Command finished first.\n");
    kill(sleep_pid, SIGKILL);
    waitpid(sleep_pid, NULL, 0);

    if (WIFEXITED(status)) {
      printf("Command exited with status: %d\n", WEXITSTATUS(status));
      return WEXITSTATUS(status);
    } else {
      printf("Command did not exit normally.\n");
      return 1;
    }
  } else if (finished_pid == sleep_pid) {
    printf("Timeout reached! Killing command. Status: 124\n");
    kill(command_pid, SIGKILL);
    waitpid(command_pid, NULL, 0);

    // Print the output from the file
    FILE *output_file = fopen(OUTPUT_FILE, "r");
    if (output_file) {
      char ch;
      while ((ch = fgetc(output_file)) != EOF) {
        putchar(ch);
      }
      fclose(output_file);
    } else {
      perror("Failed to read output file");
    }

    return 124;
  }

  return 0;
}