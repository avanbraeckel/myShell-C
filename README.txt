Austin Van Braeckel - 1085829 - avanbrae@uoguelph.ca
2021-02-05
CIS*3310 Assignment 1
Submitted to: Professor Deborah Stacey

IMPLEMENTATIONS:
All 3 sets' functionality from the CIS*3110 Assignment 1 description has been implemented, other than the exception that is outlined in the
LIMITATIONS section below.
- "exit" command
- A command with no arguments (eg. "ls")
- A command with arguments (eg. "ls -l")
- A command, with or without arguments, executed in the background using &
- A command, with or without arguments, whose output is pipied to the input of another command
- "echo $PATH" shows the PATH
- "echo $HISTFILE" shows the path to the HISTFILE (".CIS3110_history")
- "echo $HOME" shows the path to the HOME directory
- Reads the profile file (".CIS3110_profile") and executes any commands inside before startup
- Can use the "export" command to change the value for $PATH, $HOME, and $HISFTILE (eg. "export PATH=/bin")
- Can use the "history" command to see previous commands used in the shell
- Can use the "history -c" command to clear the HISTFILE (".CIS3110_history")
- Can use the "history n" where n is an integer greater than 0, to see the n last lines of the HISTFILE (".CIS3110_history)
- The "cd" command can be used to change directory
- "cd ~" can be used to change to the HOME directory

ASSUMPTIONS:
Assuming that the "home directory" (command: "cd ~") is the directory from which the shell is run
Assuming that all given directories and filenames have no spaces within the names
Assuming that there is a maximum of one '>' or '<' in a command
Assuming there is only one level of piping ('|')
Assuming that for a background process, the '&' appears as the last argument, with a space before it, separating from any previous arguments
Assuming that for "echo $HOME", "echo $PATH", and "echo $HISTFILE", that the capitals are necessary ("$home" won't give the same output)
Assuming export is to be restricted to only PATH, HOME, and HISTFILE
Assuming that when using the export command to change $HISTFILE, it continues in the original HISTFILE until "history -c" is used
Assuming that the .CIS3110_history resets (is cleared) when the shell is exited, and does not save between different instances of the execution of the shell
Assuming that the commands in the .CIS3110_profile file are executed at the beginning when running the shell, and the directory/prompt is not shown until they complete, but lines will be skipped
Assuming that a background process must notify the user that it is done immediately, at the time of completion
Assuming that commands from the .CIS3110_profile file are not stored in the .CIS3110_history file for visibility with the "history" command

LIMITATIONS:
The $PATH that is chosen is not searched for the commands, and only the default PATH is searched (for all normal bash commands)
The command "cd ~" must be used, or "cd <directory>", as "cd ~/<directory>" is not supported
Quotation escaping is not implemented in the shell, files/directories with spaces are not supported (eg. 'My Folder')

**This program code was written by myself, and myself only - with many sources used for research, as shown below, and within the myShell.c header documentation

SOURCES USED:
- class (CIS*3110) CourseLink examples
- man pages
- http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
- https://stackoverflow.com/questions/28502305/writing-a-simple-shell-in-c-using-fork-execvp
- https://stackoverflow.com/questions/50280498/how-to-only-kill-the-child-process-in-the-foreground?fbclid=IwAR3pmr6csA0gT0yvCAnDYu0Q8XEoaVJwyaIOVegXKnE_zVu62X1aKlUYkfk
- https://stackoverflow.com/questions/12663270/freopen-and-execvp-in-c
- https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection
- https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
- https://www.geeksforgeeks.org/making-linux-shell-c/
- https://www.cs.cornell.edu/courses/cs414/2004su/homework/shell/shell.html