# ⏱️ timeout — A Simple Timeout Utility in C

This is a lightweight timeout utility written in **C**, inspired by the behavior of the Linux timeout command. It allows you to run any command for a fixed number of seconds — and if the command doesn't finish in time, it gets terminated automatically.

## 🧩 How It Works

- Starts two child processes:
  - One that runs the actual command with its arguments.
  - One that sleeps for the specified timeout.
- Whichever finishes first wins:
  - If the **command** finishes first: it exits normally, and its exit code is returned.
  - If the **timer** finishes first: the command is killed, and exit code `124` is returned.
- If the command had any output, it's saved to `output.txt` and printed after termination.

## 🛠️ Building

```bash
gcc timeout.c -o timeout
```

## 🚀 Usage

```bash
./timeout timeout <seconds> <command> [args...]
```

## Examples

```bash
./timeout timeout 5 sleep 3         # exits normally (code 0)
./timeout timeout 2 sleep 5         # killed after timeout (code 124)
./timeout timeout 1 factor 1234567  # compute before timeout
```

## 📁 Files

```text
File            Description
timeout.c       Main source file with the implementation
output.txt      Captures command output (if killed)
.gitignore      Ignores compiled binaries and temp files
```

## 🧪 Sample Output

```bash
$ ./timeout timeout 5 sleep 3
Starting timeout: 5 seconds
Command finished first.
Command exited with status: 0

$ ./timeout timeout 2 sleep 6
Starting timeout: 2 seconds
Timeout reached! Killing command. Status: 124
Executing command: sleep
```

## 📎 Notes

Uses fork(), execvp(), kill(), waitpid() and file redirection.

Output is only printed from output.txt if the command was killed.

Keeps everything simple and POSIX-compatible.

## ⚙️ Author

Irtaza Shahzad
