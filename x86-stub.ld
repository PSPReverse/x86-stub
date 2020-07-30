/** @file
 * AMD Zen x86 stub - Linker file.
 */

/*
 * Copyright (C) 2020 Alexander Eichner <alexander.eichner@campus.tu-berlin.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
MEMORY
{
    RAM : ORIGIN = 0xffff0000, LENGTH = 64K
}


SECTIONS
{
    . = 0xffff0000;

    .rodata ALIGN(0x10):
    {
        *(.rodata)
        *(.rodata*)
    } > RAM

    .data ALIGN(0x10):
    {
        *(.data)
        *(.data*)
    } > RAM

    .bss ALIGN(0x10):
    {
        BSS_START = .;
        *(.bss)
        *(.bss*)
        BSS_END = .;
    } > RAM

    .text ALIGN(0x10):
    {
        *(.text);
        *(.text*);
    } > RAM

    /* Reset vector goes here. */
    . = 0xfffffff0;

    .reset_vector ALIGN(0x10):
    {
        *(.reset_vector);
    } > RAM

    _ram_end = ORIGIN(RAM) + LENGTH(RAM);
}

