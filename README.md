# Basic unix shell

Program accepts any command line unix shell command from stdin and produces the corresponding output to stdout. It supports to run shell
commands in the foreground as well as in the background (e.g.,“ls &”). In addition, it also supports “pipe” command (e.g., “ls | wc”) and
multiple commands in one line separated by “;” (e.g., “ls; ps;”). Shell program will exit when user types “exit”.

## Required Package: 
libreadline-dev
### To install the readline library:
sudo apt-get install libreadline-dev

### Command to create the binary:
gcc shell_swapnil.c -o shell_swapnil -lreadline
