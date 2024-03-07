# Custom-Shell-Implementation
Our goal in this assignment is to recreate the basic functionalities of a shell in a Linux environment using a small parser.
## Functionalities
- Supports every sort of input/output redirection.
- Supports pipes.
- Supports background execution with the '&' at the end of command.
- Supports multiple command execution using ';' as a seperator between them.
- Supports wild characters (for current directory).
- Supports combination of the above.
- Supports aliases. 
- Supports Signals. 
- Stores archive up to 20 commands which user views with myHistory command. myHistory following a number in range [1,20] reexecutes the corresponding command.

## General Admissions
- Aliases are created in the following format; createalias alias "regular command name".Alias should be in quotation marks without white spaces after the opening or before the closing one just like given example.Aliases can be destroyed with the destroyalias alias command.If you insert multiple aliases on the same nickname both are saved but only the first one in time order will be considered.For aliases commands signals won't be forwarded.
- The myHistory command can't be used with another one in the same line.
- The Ctrl-C signals stops the running process but doesn't terminate the shell.Shell also ignores Ctrl-Z and forwards it in the running processes.
- In case of parallel execution of commands the signal stops the most recently forked process.Signals Ctrl - C and Ctrl - Z don't work for background processes as it happens in all shells.

## Usage
Use make run to start the shell.
