#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

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

uint64 sys_printSysCalls(void)
{
    uint64* counterArray = myproc()->sysCounter;

    printk("System Call counts for current process:\n");
    for(int i=0;i<32;i++)
    {
        if(counterArray[i]!=0)
        {
             printk("%d \t %ld\n",i,counterArray[i]);
        }
    }

    return 0;
}

uint64 sys_printProcessSysCalls(void)
{
    int pid;
    argint(0,&pid);
    struct proc *p;

    // find the pid in process table
    acquire(&wait_lock);

    for(p=proc; p<&proc[NPROC]; p++)
    {
        if(p->state!=UNUSED && p->pid==pid)
        {
            uint64* counterArray = p->sysCounter;
            printk("System Call counts for process: %d\n",pid);

            for(int i=0;i<32;i++)
            {
                    if(counterArray[i]!=0)
                    {  
                         printk("%d \t %ld\n",i,counterArray[i]);
                    }
            }
            release(&wait_lock);
            break;
        }
    }

    return 0;
}

uint64 sys_getInodeNum(void)
{
  int fd;
  argint(0,&fd);

  struct proc* p = myproc();

  // is fd in valid bounds
  if(fd<0||fd>=NOFILE)
  {
    return -1;
  }

  // check if ofile[fd] points to valid struct file
  struct file* f = p->ofile[fd];

  if(f==0)
  {
    return -1; // invalid fd
  }

  // is valid type of INODE
  if(f->type!= FD_INODE)
  {
    return -1;
  }

  int inodeNum = f->ip->inum;

  return inodeNum;
}

uint64 sys_getReadOffset(void)
{
  int fd;
  argint(0,&fd);

  struct proc* p =myproc();

  // is fd in valid bounds
  if(fd<0||fd>=NOFILE)
  {
    return -1;
  }

  // check if ofile[fd] points to valid struct file
  struct file* f = p->ofile[fd];

  if(f==0)
  {
    return -1; // invalid fd
  }

  // is valid type of INODE
  if(f->type!= FD_INODE)
  {
    return -1;
  }

  int offset = f->off;

  return offset;
}

