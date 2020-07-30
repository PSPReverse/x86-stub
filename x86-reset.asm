;; @file
; AMD Zen x86 stub - Reset code.
;

;
; Copyright (C) 2020 Alexander Eichner <alexander.eichner@campus.tu-berlin.de>
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.
;

EXTERN _c_start
SECTION .text

; We start in real mode.
BITS 16
CPU 80386

; Our GDT
_gdt:
    dq 0                        ; Null segment
    ; Data segment
_gdt_data:
    dw 0xffff                   ; Limit
    dw 0                        ; Base address
    db 0
    db 10010010b
    db 11001111b
    db 0x0
    ; Code segment
_gdt_code:
    dw 0xffff                   ; Limit
    dw 0                        ; Base address
    db 0
    db 10011010b
    db 11001111b
    db 0x0
_gdt_end:

_gdt_desc:
    dw _gdt_end - _gdt
    dd _gdt

CODESEG equ _gdt_code - _gdt
DATASEG equ _gdt_data - _gdt

_start:
    cli                         ; Disable interrupts
    lgdt [_gdt_desc]            ; Load GDT
    xor ax, ax                  ; Clear AX
    mov ss, ax                  ; Load stack segment selector
    mov ds, ax                  ; Load data segment selector
    mov es, ax                  ; Load extra segment selector
    mov fs, ax                  ; Load segment selector
    mov gs, ax                  ; Load segment selector

    mov eax, cr0                ; Load CR0 into EAX
    or eax, 0x1                 ; Enable protected mode
    mov cr0, eax                ; Write back CR0
    jmp CODESEG:_protected_mode ; Do a far jump to protected mode, this will load CS

; Entering protected mode
BITS 32

_protected_mode:
    mov ax, DATASEG             ; Initialize the segment selectors
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, _reset             ; Set stack pointer
    sub esp, 48                 ; Align pointer to a 64byte cache line (reset vector occupies top 16bits)

    call _c_start               ; Call into the C startup routine

; The reset vector section so the linker can place it at the right location
SECTION .reset_vector

_reset:
        jmp _start

