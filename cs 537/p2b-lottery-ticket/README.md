Proportional-share Scheduling:
In sysproc.c, I add two functions: settickets() just set tickets number user inputs for processes, getpinfo() requires extra help in proc.c to access ptable.
Thus, in proc.c, I add a helper function info_generator(), which loop over all processes in ptable and add their info into given pstat struct.
In proc.c, I set every process's initial ticket to one in allproc(), and let children process inherits its parent's number of tickets in fork().
I changed the shceduler()'s inner for-loop, so when current process is runnable, update its inuse status and ticks number.
I also add code to check if current process's ticks number is a multiple of its tickets number, 
if its ticks number is, then this process had run enough proportion of time and switch to next process automatically with the loop,
if not, then do not switch to next process with "p--".
Null-pointer Dereference:
I changed sz = 0 to sz = 4096 in exec.c and changed the for-loop in copyuvm() to start from 4096 instead of 0 in vm.c to skip the zero page.