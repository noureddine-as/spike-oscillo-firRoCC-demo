#=======================================================================
# UCB VLSI FLOW: Makefile for riscv-bmarks
#-----------------------------------------------------------------------
# Yunsup Lee (yunsup@cs.berkeley.edu)
#-----------------------------------------------------------------------
# Modify by Gabriel Villanova

XLEN ?= 64
ISA  ?= rv64imafd	
ABI  ?= lp64

APP   = main

#--------------------------------------------------------------------
# Sources and Includes
#--------------------------------------------------------------------
SOURCES = ./src
INCS   += -I./include 
LINKER  = link.ld
#--------------------------------------------------------------------
# Build rules
#--------------------------------------------------------------------
RISCV_PREFIX ?= riscv$(XLEN)-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)gcc
#RISCV_GCC_OPTS ?= -DPREALLOCATE=1 -mcmodel=medany -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf
RISCV_GCC_OPTS ?= -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -ffreestanding -static -nostdlib -nostartfiles -std=gnu99 -O0 -ffast-math -fno-common $(INCS)
RISCV_LINK ?= $(RISCV_GCC) -T$(LINKER) $(INCS)
RISCV_LINK_OPTS ?= -static -nostdlib -nostartfiles -lm -lgcc -T$(LINKER)
RISCV_OBJDUMP ?= $(RISCV_PREFIX)objdump --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.data
RISCV_SIM ?= spike --isa=rv$(XLEN)gc

#------------------------------------------------------------
# make all
all: obj link size

#------------------------------------------------------------
# size
size:
	$(RISCV_PREFIX)size $(APP)

#------------------------------------------------------------
# make link
link:
	$(RISCV_LINK) $(SOURCES)/syscalls.o $(SOURCES)/crt.o $(SOURCES)/$(APP).o $(RISCV_LINK_OPTS) -o $(APP)

#------------------------------------------------------------
# make objects .o
obj:
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(INCS) -c $(SOURCES)/syscalls.c -o $(SOURCES)/syscalls.o 
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(INCS) -c $(SOURCES)/crt.S -o $(SOURCES)/crt.o 
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(INCS) -o $(SOURCES)/$(APP).o -c $(SOURCES)/$(APP).c

sim:
	@spike $(APP)

mem_dump:
	$(RISCV_PREFIX)objdump -d $(APP)

hex:
	riscv64-unknown-elf-objcopy -I binary -O ihex $(APP) $(APP).hex
	@#riscv64-unknown-elf-objcopy -O ihex $(APP) $(APP).hex


clean:
	rm -rf src/*.o
	rm -rf a.out
	rm -rf $(APP)
