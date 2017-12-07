Proof: https://www.youtube.com/watch?v=QK-k52oNjvU

This project was completed in 4-5 days starting on Thursday Nov 30 - Tuesday 5 (when CIS380 was due, haha), so there are a lot of bugs/errors/edge cases and this is on baremetal, so ... yeah...

Please add to issues if you find any errors. Thanks!

And big shoutout to the osdev community. The stuff they put online provided me with the tools/information necessary to complete this project! :)

### Dependencies

qemu (emulator)

### What does this run on?

This runs on the x86 family of processors and should work on x64 as well (?) since Intel processors are very backwards compatiable

### What is in is this setup?

Runs on physical memory (no virtual memory since virtual memory is hard and I didn't have the time to write the code and is it even necessary??

Setup basic hardware components (Global Descriptor Table, Interrupt Descriptor Table, Programmable Microcontroller, Programmable Interval Controller, Keyboard)

Has some standard library code (Yes, there is no "standard" library when one boots on hardware, you gotta write it yourself or find something online)
[ex, string.h, stdio.h, etc...]

Has very funky unistd.h wrapper (some code is modified for penn-os to run properly)

ucontext library (Oh dear... it was all basically written in assembly, even the source was in assembly =/)

signal library (SIGALRM, SIGINT, SIGTSTP) - VERY UNSTABLE. Runs in an interrupt handler, so pretty unsafe... in addition, setcontext/swapcontext is used in the signal handlers, so pretty damn funky way to handle this case

### How to link your penn-os with hardware setup

Place all your penn-os files into the directory.

There are some things you have to modify before moving on~

In kernel_main.c, you will need to uncomment main.c and include your own make flat fs code before main.c (since we aren't running unix/linux, we can't run ./mkFlatFAT and ./penn-os flatfs)

Move your code for setting up SIGALRM to the start of the shell function/context (so, not in main)

Change all your #include <...> to #include "...".
Some includes might be changed, or may not exist in this library

Crosscompiler: https://github.com/rm-hull/barebones-toolchain

I have placed the cross compiler in here already, so that may be why the download process takes so long

cd barebones-toolchain-master/
. ./setenv.sh
cd ..

make

Emulator
```
make qemu
```

Hardware (usb, MAKE SURE YOU DON"T KEEP SENSITIVE DATA ON USB AND PC)
```
I have windows, so download https://rufus.akeo.ie/
install program
run it
There is a disk-image thing in the bottom right, so click it and locate myos.iso (you might have to transfer it out of the unix/linux vm to your host os
Click start
Wait for the program to finish formatting your usb
Eject usb
plug usb into a computer with x86 processor
click f12 (or some way to get to the boot menu) and select boot from usb
It should boot up and run!
```

What if the usb didn't work?
You might have to zero out your usb
Search online for guides

Not using windows?
Search for guides online. OS DEV community probably has a tutorial

### Oh no, it doesn't work!

Yeah... printf could maybe help? Mostly try to move your signal initialize code somewhere else

make dqemu (will boot up qemu with gdb)

Feel free to change any of the files to make your penn-os compile :). I have not written everything perfectly or implemented the entire standard library, so yeah, go for it

# File Setup

```
assert.h - assert() macro
bootloader.asm - grub setups everything and starts here. This calls kernel_main
cpu.h/.c - not used
ctype.h - standard library for finding type of character
debug.h/.c - abort function, halts processor
elf.h - x86 elf structure - used to parse kernel size
fcntl.h - includes unistd.h
gdt.h/.c - global descriptor table initialization
gdt_selectors.asm - asm stub for gdt.h/.c
grub.cfg - grub configuration
hal.h/.c - used to disable/enable interrupts
idt.h/.c - interrupt descriptor table initialization
includes.h - standard integer value header (uint32_t, etc...)
isr.h/.c - interrupt service routines for interrupts after idt is iniitailized
isr_handler.asm - stub for isrs
kernel_main.c - main file that initializes everything and calls main
keyboard.h/.c - keyboard hardware functions
linker.ld - links all the files into an elf x86 executable and specifies how the file is laid out
malloc.h/.c - standard libarary functions such as malloc, free, realloc, calloc
math.h/.c - standard math functions
mman.h/.c - mmap
multiboot.h - standard for parsing multiboot structure created by GRUB when booing the kernel
phys_mm.h/.c - parses multiboot and uses a bitmap to represent how memory is laid out
pic.h/.c - programmable interrupt controller initialization
pit.h/.c - programmable interval timer intialization
ports.h/.c - wrapper around assembly to writing to ports
sigaction_stub.asm - not used (used before for experimenting when signal handlers)
signal.h/.c - signal functions (sigaction, etc...)
stdio,h/.c - standard input/output functions
stdlib.h/.c standard library functions
string.h/.c - standard string library functions
task.h/.c - not used
time.h/.c - linux itimer to specify the delay between timer interrupts
ucontext.h/.c - wrapper around assembly for ucontext_stub.asm
ucontext_stub.asm - ucontext assembly library
unistd.h/.c - linux functions
us_keyboard.h - specifies structures for a US keyboard
valgrind.h - empty macros for vaglrind (DOES NOT WORK, just to make it less work for people to integerate)
vga.h/.c - montior color/text functions
virtual_mm.h/.c - not used (not using virtual memory)
```

# How everything is ran in order

### bootloader

Use GNU GRUB, so we don't have to handle 16 bit tranferring to protected mode 32 bits

### after bootloader

We get into kernel_main, and we have to initialze some hardware/processor data structures

GDT - Global Descriptor Table
IDT - Interrupt Descriptor Table
PIC - Programmable Interrupt Controller
PIT - Programmable Interval Timer
Keyboard - Well... keyboard

GDT/IDT
http://download.intel.com/support/processors/pentium4/sb/25366821.pdf
http://wiki.osdev.org/Global_Descriptor_Table
http://wiki.osdev.org/Interrupt_Descriptor_Table

PIC/PIT/Keyboard
http://wiki.osdev.org/8259_PIC
http://wiki.osdev.org/Programmable_Interval_Timer
http://wiki.osdev.org/%228042%22_PS/2_Controller

### after hardware

Physical memory manager - read what physical memory is reserved/avaliable + location of kernel
Memory manager - malloc/free on top of physical memory manager
unistd libarary - penn-os wrapper stuff

Memory management
http://wiki.osdev.org/Memory_management

### what after that?
signal handlers - I had to come up with a way to do this. I don't really know why it works, but it does ;) (searched up sigaction, didn't help. Here is link: https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/sigaction.c.html)

ucontext - ah... ucontext. Okay here is the code for x86 assembly. It's actually pretty cool how it was implemented. Props to the guy(s) who made this, but it wasn't portable enough

https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/getcontext.S.html
https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/setcontext.S.html
https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/makecontext.S.html
https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/swapcontext.S.html

# References:
https://github.com/cfenollosa/os-tutorial - Straightfoward, good code, but does not explain much

http://wiki.osdev.org/Tutorials - Very good resource!!!

https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf - Awesome tutorial that explains the beginning parts such as bootloader/gdt well! Very detailed asm starter guide :)

https://tuhdo.github.io/os01/ - Another great tutorial on the beginning parts of the operating system! Unfortunately, as of right now, it does not cover much besides the basics.

http://www.brokenthorn.com/Resources/OSDevIndex.html - Absolutely the best resource on OS development on the internet. Although a bit outdated for some tutorials, there are other os tutorials (ex. mentioned above) to supplement the code. Explains everything! When I mean everything, I mean everything!!! PIC? A whole tutorial dedicated to it, Great code and explanation.

INTEL MANUAL BOIS :)
http://download.intel.com/support/processors/pentium4/sb/25366821.pdf

### ISSUES

If you press CTRL C/ CTRL Z while typing fg/bg, the program will be in an undefined state (deadlock, return from main, triple fault, etc...), so try to type a bit slower

