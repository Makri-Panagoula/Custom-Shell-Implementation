Use make run for execution.

4th Question :
    For the background processes in the same command (p.ex. with pipes) we put them in the same process group, we don't do anything for foreground commands. There isn't the ability to bring a background process to the foreground.

5th Question:
    We maintain an array with alias , where we insert at the end and upon deletion we set pointer to NULL, we reallocate only if memory isn't enough (we don't care much if we are wasting memory (or a little more time on search) upon many deletions).If you insert multiple aliases on the same nickname both are saved but only the first one in time order will be considered.
    Please insert alias in quotation marks WITHOUT white spaces after the opening or before the closing one (strictly in the exact form that was asked in assignments instructions,check given example).

6th Question:
    Our motive is for the parent process to ignore the signals and "forward" them to running process (current command).Let me clarify at this point that next commands in the same line will be executed normally (that's what i understood from assignment's instructions).So we have two cases, one that we receive signal in parent process and then we ignore it and the first process to be executed receives it or we receive signal in child process where we have default behaviour for handling it. Keep in mind that in our shell once you suspend a process you can't resume it execution, it doesn't support bg or fg execution , also when we are in createalias,destroyalias,cd,exit we ignore signals.In case of parallel execution of commands the signal stops the most recently forked process.Signals Ctrl - C and Ctrl - Z don't work for background processes as it happens in all shells.
    Disclaimer : I have personally decided that for aliases commands signals won't be forwarded because : a) signal calling won't make any visible difference for the user since they don't produce any output and b) user mustn't use them in real life conditions because it will cause undefined behaviour(p.ex. for destroy some pointers might be freed and aliases[pos] not set to NULL so we still consider the alias existent and upon accessing we might end up with segfault and for create if p.ex. reallocation is needed but not performed due to signal then when inserting next element we will get segfault).Same goes for exit because it's our "special" command for escaping the shell.(Run through executable not Makefile).


7th Question:
    We consider myHistory as a simulation of upper arrow button , therefore it can't be combined with any other command in the same line even using ';' as a
    seperator.myHistory prints the last 20 (or the available ones in case they are less) lines (exactly as it happens when we use the button in linux , not each command that may be in the line seperately).Number of wanted line should be in the range [1,20] , with 0 being first(oldest) line the user gave.Keep in mind that for every call of myHistory num ONLY the referenced (num) line is stored in the archive (as it makes sense and happens in normal shells).

In the rest of the questions I haven't made any particular admissions , so just check the comments (they are pretty detailed) .
