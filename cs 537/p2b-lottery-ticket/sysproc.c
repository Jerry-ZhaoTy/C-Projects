#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Set up ticket number for processes
int
sys_settickets(void)
{
    // Get ticket number from user
    int ticket_num = 0;
    argint(0, &ticket_num);

    // Check if given number of tickets is valid
    if (ticket_num <= 0) return -1;

    // Set up valid ticket number for current process
    acquire(&tickslock);
    myproc()->tickets = ticket_num;
    release(&tickslock);

    return 0;
}

// Update info of all processes into pstat
int
sys_getpinfo(void)
{
    // Get pstat user inputs
    struct pstat* proc_stat;
    if (argptr(0, (void*)&proc_stat, sizeof(*proc_stat)) < 0)
      return -1;

    // Check if given pstat is valid
    if (proc_stat == 0) return -1;

    // Updating the given pstat
    acquire(&tickslock);
    info_generator(proc_stat);
    release(&tickslock);

    return 0;
}
