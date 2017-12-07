CC=i686-elf-gcc
CFLAGS=-ffreestanding -O0 -nostdlib -fno-builtin -fno-stack-protector -fno-exceptions -m32 -g -ftrapv -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Waggregate-return -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Winit-self -Wformat-nonliteral -Wno-unused-parameter -Wno-unused-but-set-parameter -std=c11 -fms-extensions -ggdb3

#CFLAGS=-ffreestanding -O0 -nostdlib -fno-builtin -fno-stack-protector -fno-exceptions -m32 -g -Wall -Wextra -Wpedantic -fms-extensions -std=c11

#CFLAGS = -O0 -g -ftrapv -Werror -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Waggregate-return -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Winit-self -Wformat-nonliteral -Wno-unused-parameter -Wno-unused-but-set-parameter -std=c11 -fms-extensions -ggdb3 -D_POSIX_C_SOURCE -m32
#CFLAGS = -O0 -g -ftrapv -Werror -Wextra -Wall -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Waggregate-return -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Winit-self -Wformat-nonliteral -Wno-unused-parameter -Wno-unused-but-set-parameter -std=c11 -fms-extensions -ggdb3 -D_POSIX_C_SOURCE -m32

LD_FLAGS=-ffreestanding -O0 -nostdlib -fbuiltin -Wall -Wextra -Werror -Wno-format -fstack-protector-all -g

# select all the o files (both compiled asm and c files)

C_SRCS = $(wildcard *.c)
HEADERS = $(wildcard *.h)
C_OBJS = $(C_SRCS:.c=.o)

ASM_SRCS = $(wildcard *.asm)
ASM_OBJS = $(ASM_SRCS:.asm=.o)

BIN=myos.bin
ISO=myos.iso

.PHONY: setup verify create-iso qemu iso-qemu dqemu ddqemu

all: setup $(C_OBJS) $(ASM_OBJS) $(BIN) create-iso

setup :
	cd barebones-toolchain-master; . ./setenv.sh

verify :
	if grub-file --is-x86-multiboot myos.bin; then
		echo multiboot confirmed
	else
		echo the file is not multiboot
	fi

create-iso : 
	mkdir -p isodir/boot/grub
	cp $(BIN) isodir/boot/$(BIN)
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir

qemu : $(C_OBJS) $(ASM_OBJS) $(BIN) create-iso
	#qemu-system-i386 -kernel $(BIN)
	qemu-system-i386 -machine q35 -cdrom $(ISO)

ddqemu : $(C_OBJS) $(ASM_OBJS) $(BIN) create-iso
	#qemu-system-i386 -kernel $(BIN)
	qemu-system-i386 -machine q35 -cdrom $(ISO) -d guest_errors,int,exec,cpu_reset,in_asm -D qemu_log_file.txt
	
dqemu : $(C_OBJS) $(ASM_OBJS) $(BIN) create-iso
	#qemu-system-i386 -machine q35 -cdrom $(ISO) -gdb tcp::26000 -S -d guest_errors,int,exec,cpu,cpu_reset,out_asm,in_asm &
	qemu-system-i386 -machine q35 -cdrom $(ISO) -gdb tcp::26000 -S -d guest_errors,int -D qemu_log_file.txt &
	(printf "set disassembly-flavor-intel\ntarget remote localhost:26000\nsymbol-file $(BIN)\nset disassembly-flavor intel\nset architecture i386\nset print pretty on\nset print array on\nb *kernel_main\nset disassemble-next-line on\nc\n"; cat) | gdb


iso-qemu : $(C_OBJS) $(ASM_OBJS) $(BIN) create-iso
	qemu-system-i386 -cdrom $(ISO)

$(BIN) : $(ASM_OBJS) $(C_OBJS)
	$(CC) -T linker.ld -o $@ $(LD_FLAGS) $^ -lgcc 

%.o : %.asm
	nasm -f elf32 $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -vf *.o
	rm -vf $(BIN)
	rm -vf $(ISO)
	rm -rvf isodir

