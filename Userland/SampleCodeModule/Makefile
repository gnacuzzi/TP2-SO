include ../Makefile.inc

MODULE_BIN=0000-sampleCodeModule.bin
MODULE_ELF=0000-sampleCodeModule.elf
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

all: $(MODULE_BIN) $(MODULE_ELF)

# Binario para QEMU
$(MODULE_BIN): $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../$(MODULE_BIN)

# ELF para GDB
$(MODULE_ELF): $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../$(MODULE_ELF)

obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf obj
	rm -rf ../$(MODULE_BIN) ../$(MODULE_ELF)
