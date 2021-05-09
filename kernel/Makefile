# Make rosco_m68k ROM images
# 
# Copyright (c)2020 Ross Bamford
# See LICENSE

# Removed cflag -pedantic for non-standard use of labels
# Removed cflag -Wall because this is getting messy

EXTRA_CFLAGS?=
SYSINCDIR?=../libs/build/include
SYSLIBDIR?=../libs/build/lib
LIBS=-lmachine -leasy68k -lcstdlib -lstart_serial -ldebug_stub -lprintf -lgpio -lsdfat
DEFINES=-DROSCO_M68K
CFLAGS=-std=c11 -ffreestanding -nostartfiles -Werror	\
				-I$(SYSINCDIR) -mcpu=68010 -march=68010 -mtune=68010	\
				-mno-align-int -mno-strict-align $(DEFINES)
LDFLAGS=-T $(SYSLIBDIR)/ld/serial/rosco_m68k_kernel.ld -L $(SYSLIBDIR)	\
				-Map=$(MAP)
ASFLAGS=-Felf -m68010 -spaces -quiet $(DEFINES)
CC=m68k-elf-gcc
LD=m68k-elf-ld
AS=vasmm68k_mot
RM=rm -f
KERMIT=kermit

SERIAL?=/dev/modem
BAUD?=9600

# Output config
BINARY_BASENAME=kernel
BINARY_EXT=bin
MAP=$(BINARY_BASENAME).map
BINARY=$(BINARY_BASENAME).$(BINARY_EXT)

OBJECTS=musl/stdlib/atoi.o spinlock.o \
		process.o common.o trap14.o context.o fork.o syscall.o \
		pages.o malloc.o fs.o users.o \
		bin/kill.o bin/memplay.o bin/syscaller.o bin/dir.o \
		bin/ps.o bin/reboot.o bin/uptime.o bin/who.o bin/blinkd.o \
		system.o spawn.o shell.o kmain.o

%.o : %.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

%.o : %.S
	$(AS) $(ASFLAGS) -o $@ $<

$(BINARY) : $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)
	chmod a-x $@

.PHONY: all clean dump

all: $(BINARY)

clean: 
	$(RM) $(OBJECTS) $(BINARY) $(BINARY_ODD) $(BINARY_EVEN) $(MAP)

dump: $(BINARY)
	od --endian=big -tx1 $(BINARY)

load: $(BINARY)
	kermit -i -l $(SERIAL) -b $(BAUD) -s $(BINARY)

terminal:
	minicom -D /dev/cu.usbserial-1 -c on -R utf-8
