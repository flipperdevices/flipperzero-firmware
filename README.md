# EXTRA
A pack of extra commands for the Flipper Zero (a C library). Check out the demo [here](https://github.com/Milk-Cool/EXTRA-demo)!

## Downloading and importing
In your app directory run:
```bash
git clone https://github.com/Milk-Cool/EXTRA.git
```

## Usage
To include it in a plugin:
```c
#include "EXTRA/extra.h"
```
To register extra commands:
```c
extra_init_register();
```
To unregister and free the used memory:
```c
extra_deinit_unregister();
```
You can also use `extra_registered()` to check if extra commands are currently registered.

## Commands (`extra_help`)
```
extra_help [command]
Lists all commands and usages. Specify a command to only output its usage.
cd [path]
Changes the current working directory to the specified direcotry. If no directory is specified, it changes it to /ext.
cp <path1> <path2>
Copies the specified file (<path1>) to the specified location (<path2>).
echo <message>
Prints the specified message.
false
Always outputs nothing.
get <name>
Prints the value of variable <name>
if <command1>;<command2>
If <command1> output is non-empty, executes command 2. Note that commands are separated by a semicolon, not a space.
input <name>
Gets an input from the user and saves it to the variable <name>
ls [path]
Lists all files and directories in the specified direcotry. If no directory is specified, it takes the current working directory.
mkdir <path>
Creates a directory at the specified path.
mv <path1> <path2>
Moves the specified file (<path1>) to a new location (<path2>).
not <command>
If <command> output is non-empty, then output nothing. Otherwise, output \"true\".
pwd
Prints the current working directory.
read <path>
Read the contents of the specified file.
rm <path>
Deletes the file/directory (recursively) at the specified path.
sequence <sequence>
Plays a notification sequence.
Takes sequence name (sequence defined as sequence_<sequence name> in notification/notification_messages.h) as the only argument.
set <name> <value>
Sets the variable <name> to <value>
sh
Starts the EXTRA shell.
sleep <time>
Freezes for <time> milliseconds.
start <path>
Execute the script at the specified path.
Executes commands from a file line by line, igores lines starting with '#' as comments.
For more info refer to README.md.
touch <path>
Creates a file at the specified path.
true
Always outputs "true".
while <command1>;<command2>
While <command1> output is non-empty, repeatedly execute <command2>.
write <path>
Writes input text to the specified file.
```

## Scripts
Use `start <path_to_script>` to run a script.
A script is just CLI commands in a file, each one is on a new line.
If `#` character is present in a line everything after it is ignored as a comment.

This example outputs "Hello World", waits a second and plays the success sequence (the one that plays, for example, when the Flipper has successfully written data to a tag).

```bash
# Example script

echo Hello World
sleep 1000 # 1000ms = 1s
sequence success
```

## Variables
Use `set <varname> <value>` to create a variable.
There can be no more than 64 variables at the same time.
You can also use `input <varname>` to set a variable value to a string input by user.

> TODO: command and function to clear variables

Also in scripts and EXTRA shell you can use `%<varname>%` to insert a variable value.

Example:
```bash
input in
echo %in% # This outputs the same thing as the user typed in
```
