# @file
# AMD Zen x86 stub - Makefile.
#

#
# Copyright (C) 2020 Alexander Eichner <alexander.eichner@campus.tu-berlin.de>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

CFLAGS=-m32 -O2 -DIN_X86_STUB -g -std=gnu99 -fomit-frame-pointer -nostartfiles -nostdlib -ffreestanding -Wextra -Werror
LIBGCC=$(shell gcc -print-libgcc-file-name)
LDFLAGS=$(LIBGCC)


OBJS = main.o

all : x86-stub.elf x86-stub.raw

clean:
	rm -f x86-reset.o $(OBJS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

%.o: %.asm
	$(CFLAGS) -c -o $@ $^

x86-reset.o: x86-reset.asm
	yasm -f elf -m x86 -g dwarf2 -o $@ $^

x86-stub.elf : x86-stub.ld x86-reset.o $(OBJS)
	ld -melf_i386 -Map=x86-stub.map -T $^ -o $@ $(LDFLAGS)

x86-stub.raw: x86-stub.elf
	objcopy -O binary $^ $@

