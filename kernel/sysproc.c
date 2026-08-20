#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

// get the process table and waitlock
extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64 sys_getppid(void)
{
    return myproc()->parent->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64 sys_nfork(void)
{
    int num_child;
    uint64 childPidAddress;

    // take args from user programs n process count and ptr to array int*
    argint(0,&num_child);
    argaddr(1,&childPidAddress);

    // if num_child 0 or -ve early return
    if(num_child<=0)
    {
        return 0;
    }


    // create n childs
    for(int i=0; i<num_child; i++)
    {
       int childPid =  kfork();

       if(childPid<0)
       {
           return -1;
       }
       else if (childPid==0)
       {
           // child code
           return 0;
       }
       else
       {
           //parent code
           uint64 dst = childPidAddress + (i * sizeof(int));

           if (copyout(myproc()->pagetable,myproc()->sz, dst, (char*)&childPid, sizeof(int)) < 0) {
                return -1;
           }
           //localCopyOfPid[i]=childPid;
           
       }
    }

   


    return num_child;
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_square(void)
{
    int num;
    argint(0,&num);

    uint64 numCast = (uint64)num;

    return numCast * numCast;
}

uint64 sys_getChildCount(void)
{
    return myproc()->child_count;
}

uint64 sys_getProcessChildCount(void)
{
    int pid;
    struct proc* p;

    argint(0,&pid);

    acquire(&wait_lock);

    for(p=proc; p<&proc[NPROC]; p++)
    {
        if(p->state!=UNUSED && p->pid==pid)
        {
            int count= p->child_count;
            release(&wait_lock);
            return count;
        }
    }

    return -1;
}
