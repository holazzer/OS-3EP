# measurement: syscall and context switch

Based on Operating System: Three Easy Pieces.

### time

use `gettimeofday` from `<sys/time.h>` to get a timestamp

### Syscall

Call empty read call and read zero byte.
See code in [bench.c](/bench.c)

```sh
$ gcc bench.c -o bench
$ ./bench 100000
Get fd:3
Syscall 100000 times.
tv0 :1595929770 647941
tv1 :1595929770 703798
diff:55857 ms
```

### Context Switch

The author recommend we use two processes to communicate with one another, like using a pipe, so it can only continue when switched to the other.

I use fork() to create a child process(marked as B,parent marked as A) and two pipes. B echos back what it receives and exit when its read pipe is closed. A would send and receive one character for n times.

However, only when your OS runs on one cpu does this absolutely reflect context switch cost, since my virtual machine always use one core, I guess it's fine for me. Use `sched_setaffinity()` to designate a processor to run on.

Use `lscpu` to see information about your CPU.Here's mine:

```sh
$ lscpu | grep 'CPU(s)'
CPU(s):                          1
On-line CPU(s) list:             0
NUMA node0 CPU(s):               0
$ gcc two-pipe.c -o 2p
$ ./2p 100000
pipes created: [4]A->B[3] [6]A<-B[5]
child:2655
pipe-transfer 100000 times.
tv0 :1595929556 212372
child process exit.
tv1 :1595929556 947527
diff:735155 ms
wait gets: 0 1
```

BTW, WSL is so much slower than a virtual machine.

see `lscpu | grep Virtualization`

Have fun!
