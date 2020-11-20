# Copyright (c)2020 Ross Bamford
# See LICENSE at https://github.com/rosco-m68k/rosco_m68k/blob/develop/LICENSE

EXTRA_CFLAGS?=
SYSINCDIR?=./libs/build/include
SYSLIBDIR?=./libs/build/lib
LIBS=-lprintf-softfloat -lcstdlib -lmachine -lstart_serial -lgcc
GCC_LIBS=$(shell $(CC) --print-search-dirs 															\
		| grep libraries:\ =																								\
    | sed 's/libraries: =/-L/g' 																				\
    | sed 's/:/m68000\/ -L/g')m68000/
DEFINES=-DROSCO_M68K
CFLAGS=-std=c11 -ffreestanding -Wall -pedantic -Werror -I$(SYSINCDIR) 	\
			 -mcpu=68010 -march=68010 -mtune=68010														\
			 -mno-align-int -mno-strict-align $(DEFINES)
LDFLAGS=-T $(SYSLIBDIR)/ld/serial/rosco_m68k_program.ld -L $(SYSLIBDIR) \
				-Map=$(MAP)
ASFLAGS=-Felf -m68010 -quiet $(DEFINES)
CC=m68k-elf-gcc
LD=m68k-elf-ld
AS=vasmm68k_mot
RM=rm -f

KERMIT=kermit
SERIAL?=/dev/cu.usbserial-DN05JN76
BAUD?=9600

# Output config
BINARY_BASENAME=kernel
BINARY_EXT=bin
MAP=$(BINARY_BASENAME).map
BINARY=$(BINARY_BASENAME).$(BINARY_EXT)

OBJECTS=kmain.o

%.o : %.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

%.o : %.S
	$(AS) $(ASFLAGS) -o $@ $<

$(BINARY) : $(OBJECTS)
	$(LD) $(LDFLAGS) $(GCC_LIBS) $^ -o $@ $(LIBS)
	chmod a-x $@

.PHONY: all clean load

all: $(BINARY)

clean: 
	$(RM) $(OBJECTS) $(BINARY) $(BINARY_ODD) $(BINARY_EVEN) $(MAP)

load: $(BINARY)
	kermit -i -l $(SERIAL) -b $(BAUD) -s $(BINARY)

term:
	minicom -D /dev/cu.usbserial-DN05JN76 -c on -R utf-8