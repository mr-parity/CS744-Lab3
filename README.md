# xv6 RISC-V — CS744 Lab 3

This repository contains a modified version of **xv6-riscv**, the teaching
operating system developed by MIT. The modifications were implemented as part
of **CS744: Design and Engineering of Computing Systems, Lab 3**.

The lab focuses on understanding xv6 system calls, process management,
file-descriptor behavior, and virtual-memory/page-table mechanisms.

---

## About xv6

xv6 is a small Unix-like teaching operating system inspired by Unix Version 6
(V6). It is designed to expose important operating-system concepts through a
small and readable codebase.

The source tree is organized into several major components:

- `kernel/` — kernel code including process management, virtual memory,
  system calls, traps, and file systems
- `user/` — user-space programs and system-call wrappers
- `mkfs/` — utility used to create the xv6 file-system image
- `Makefile` — build configuration for the kernel and user programs

The operating system runs on a RISC-V architecture and is executed using QEMU.

---

# CS744 Lab 3 — Implemented Features

This project implements the three major tasks specified in CS744 Lab 3.

---

# Task 1 — User-Space Programs

Task 1 focuses on understanding xv6 user-space programs, file-system
interfaces, process creation, and execution.

## A. Hello World

Added a basic `hello` user-space program that prints:

```text
Hello World!
```

The program demonstrates the standard structure of an xv6 user program and
uses `printf()` and `exit()`.

## B. Linux-style Commands

Implemented the following user-space commands:

- `clear`
- `head`
- `tail`

The `clear` command clears the terminal screen.

The `head` command prints the first N lines of a specified file.

The `tail` command prints the last N lines of a specified file.

These programs use xv6 file-system interfaces including:

- `open()`
- `read()`
- `close()`

Basic argument and file-opening errors are also handled.

## C. Command Executor — cmd

Added the `cmd` user program.

`cmd` creates a child process using `fork()` and uses `exec()` in the child
to execute another xv6 user program.

Example:

```text
$ cmd echo hello world
hello world
```

Arguments can be passed to the program being executed.

The implementation handles missing arguments as well as `fork()` and `exec()`
failures.

## D. File Copy — cp

Implemented the `cp` user-space command.

Usage:

```text
$ cp source destination
```

The program copies the contents of the source file into the destination file.

The implementation uses xv6 file-system operations such as:

- `open()`
- `read()`
- `write()`
- `close()`

## E. Multi-Process grep — mgrep

Implemented `mgrep`, a multi-file version of grep.

Usage:

```text
$ mgrep <pattern> <file1> <file2> ... <fileN>
```

For every input file, the parent creates exactly one child process.

Each child independently searches its assigned file using the existing
pattern-matching logic from xv6 grep.

Matching lines are printed together with the worker process ID.

The implementation also handles the issue of interleaved output from multiple
worker processes.

---

# Task 2 — System Calls, Processes, and Files

Task 2 extends xv6 with new system calls and modifications to process and
file-management functionality.

## A. getppid

Implemented:

```c
int getppid(void);
```

Returns the process ID of the parent of the calling process.

Example:

```text
[parent] my pid is 3
[parent] forking a child process
[child] my pid is 4
[child] my parent is 3
```

## B. square

Implemented:

```c
int square(int num);
```

The system call accepts an integer argument and returns its square.

Example:

```text
square of 2 is 4
square of 5 is 25
```

## C. Process Child Counting

Added a `child_count` field to `struct proc` to keep track of the number of
immediate children currently belonging to a process.

Implemented the following system calls:

```c
int get_child_count(void);
int get_process_child_count(int pid);
```

The child counter is:

- initialized to zero when a process is allocated
- incremented when a child is created
- decremented when a child is reaped

Appropriate locking is used when updating the counter to prevent race
conditions when multiple processes or CPUs access the parent's child count.

`get_process_child_count()` returns `-1` if the specified process does not
exist.

## D. nfork

Implemented:

```c
int nfork(int n, int *child_pids);
```

The system call creates `n` children for the calling process.

The parent receives `n` as the return value, while every newly created child
receives `0`.

The PIDs of the created children are copied into the user-space array
provided by the parent.

The implementation uses the existing `kfork()` mechanism without modifying
`kfork()`.

## E. System Call Counters

Added per-process counters for tracking the number of times each system call
is invoked.

Implemented:

```c
int print_syscalls(void);
int print_process_syscalls(int pid);
```

`print_syscalls()` prints the system-call invocation counts for the current
process.

`print_process_syscalls()` prints the invocation counts for the specified
process.

The counters are stored in the process structure and updated during system
call dispatch.

## F. File Descriptor Information

Implemented the following system calls:

```c
uint64 get_inode_num(int fd);
uint64 get_read_offset(int fd);
```

`get_inode_num()` returns the inode number associated with a file descriptor.

`get_read_offset()` returns the current read offset associated with a file
descriptor.

Invalid file descriptors, unsupported file types, and unreadable files are
handled by returning `-1`.

The implementation demonstrates that parent and child processes created using
`fork()` can share the same underlying open file and therefore observe the
same file offset.

## G. peek2

Implemented:

```c
int peek2(int fd, char *user_addr, int num_bytes);
```

`peek2()` reads data from the current file offset without advancing the
file's read offset.

For example:

```text
peeking 5 bytes: hello
peeking 5 bytes again: hello
peeking 2 bytes: he
reading 2 bytes: he
peeking 3 bytes after reading: llo
```

Return values:

- `-1` for an invalid file descriptor, unsupported file type, or unreadable
  file
- `-2` if the end of the file has been reached
- otherwise, the operation succeeds

The data is safely copied from kernel memory into the user-space buffer.

---

# Task 3 — Virtual Memory and Page Tables

Task 3 focuses on xv6's RISC-V Sv39 virtual-memory system.

The implementation works with:

- virtual addresses
- physical addresses
- page tables
- page-table entries (PTEs)
- PTE validity
- PTE permission flags
- process virtual address-space size

The implementation makes use of xv6's existing page-table traversal and
address-translation mechanisms.

## A. PTE Validity — pteValid

Implemented:

```c
int pteValid(uint64 va);
```

The system call determines whether a given virtual address is mapped to a
valid physical page.

It returns:

- `1` — valid mapping
- `0` — invalid or unmapped address

The corresponding PTE is examined and its `PTE_V` flag is checked.

Example:

```text
$ pte
VA: 0x0000000000003FBC valid: 1
VA: 0x0000000100000000 valid: 0
```

## B. PTE Flags — getPTEFlags

Implemented:

```c
int getPTEFlags(uint64 va);
```

This system call locates the PTE corresponding to a virtual address and
reports its access permissions.

The following PTE flags are inspected:

- `PTE_R` — readable
- `PTE_W` — writable
- `PTE_X` — executable
- `PTE_U` — accessible from user mode

Example:

```text
$ pflag
VA: 0x0000000000000000 -> R:1 W:0 X:1 U:1
VA: 0x0000000000003fac -> R:1 W:1 X:0 U:1
VA: 0x0000000000004000 -> R:1 W:1 X:0 U:1
VA: 0x0000000000001000 -> R:1 W:1 X:0 U:1
VA: 0x0000000000000900 -> R:1 W:0 X:1 U:1
VA: 0x0000003ffffff000 -> R:1 W:0 X:1 U:0
```

A helper function was added to locate the PTE corresponding to a virtual
address using the current process's page table.

## C. Virtual-to-Physical Address Translation — va2pa

Implemented:

```c
uint64 va2pa(uint64 virtual_addr);
```

The system call translates a virtual address into its corresponding physical
address using the current process's page table.

The implementation uses xv6's existing page-table traversal and address
translation mechanisms.

The implementation was tested using multiple programs covering:

- different virtual pages
- parent and child processes
- different offsets within the same page

These tests demonstrate that:

- different virtual pages can map to different physical pages
- the same virtual address can map to different physical pages in different
  processes
- virtual addresses within the same page preserve their page offset during
  translation

## D. Virtual Address Space Size — getvasize

Implemented:

```c
int getvasize(int pid);
```

The system call takes a process ID and returns the amount of virtual address
space currently used by that process.

The implementation obtains the process's address-space size from the `sz`
field of `struct proc`.

Example:

```text
$ v_getvasize
Pid of the process is 3
Size of process:
16384 Bytes
Address returned by sbrk: 0x0000000000004000
Size of process:
17408 Bytes
Address returned by sbrk: 0x0000000000004000
```

The change in address-space size can be observed after:

```c
sbrk(1024);
```

because the process's size increases by 1024 bytes.

---

## System Call Integration

The new system calls were integrated into xv6's existing system-call
mechanism.

The following files were modified where required:

- `kernel/syscall.h`
  - assigns unique system-call numbers
- `kernel/syscall.c`
  - registers system-call handlers
  - dispatches system calls
  - maintains system-call counters
- `kernel/sysproc.c`
  - implements process-related system calls
- `kernel/proc.c`
  - implements process-management and page-table-related functionality
- `kernel/proc.h`
  - extends `struct proc`
- `kernel/vm.c`
  - supports virtual-memory and page-table operations
- `kernel/defs.h`
  - contains declarations for kernel functions shared across source files
- `user/user.h`
  - exposes system-call declarations to user programs
- `user/usys.pl`
  - generates user-space system-call wrappers
- `Makefile`
  - includes newly added user programs in the xv6 file-system image

---

## Testing

The implementation was tested by building and running xv6 using QEMU.

Build:

```text
make clean
make
```

Run xv6:

```text
make qemu
```

The implemented functionality can be tested using the following programs and
commands from the xv6 shell:

```text
hello
clear
head
tail
cmd
cp
mgrep

ppid
sq
test_children
test_nfork
test_syscalls
test_fd
test_peek

pte
pflag
t1_va2pa
t2_va2pa
t3_va2pa
v_getvasize
```

Because xv6 user programs are statically included in the file-system image,
changes to user programs or the Makefile should be followed by a clean
build before running QEMU.

---

## Repository Structure

```text
.
├── kernel/
│   ├── proc.c
│   ├── proc.h
│   ├── syscall.c
│   ├── syscall.h
│   ├── sysproc.c
│   ├── vm.c
│   ├── riscv.h
│   └── defs.h
│
├── user/
│   ├── hello.c
│   ├── clear.c
│   ├── head.c
│   ├── tail.c
│   ├── cmd.c
│   ├── cp.c
│   ├── mgrep.c
│   └── ...
│
├── mkfs/
├── Makefile
└── README.md
```

---

## Summary

CS744 Lab 3 extends xv6 with functionality across three major areas.

**Task 1 — User-Space Programs**
- Added `hello`
- Added `clear`
- Added `head`
- Added `tail`
- Added `cmd`
- Added `cp`
- Added multi-process `mgrep`

**Task 2 — System Calls, Processes, and Files**
- Added `getppid`
- Added `square`
- Added process child counting
- Added `nfork`
- Added per-process system-call counters
- Added inode-number inspection
- Added file read-offset inspection
- Added `peek2`

**Task 3 — Virtual Memory**
- Added PTE validity checking with `pteValid`
- Added PTE permission inspection with `getPTEFlags`
- Added virtual-to-physical address translation with `va2pa`
- Added virtual address-space size inspection with `getvasize`

Overall, the project provides hands-on experience with xv6's user/kernel
boundary, system-call mechanism, process lifecycle, synchronization,
file-descriptor behavior, virtual memory, page tables, and RISC-V PTE
permissions.
