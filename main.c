/** @file
 * AMD Zen x86 stub - Main C entry point.
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


#include <common/cdefs.h>
#include <common/types.h>

#include <x86/x86-stub.h>


/**
 * The main C entry point.
 *
 * @returns Nothing (never returns).
 */
void _c_start(void)
{
    volatile PX86STUBMBX pMbx = (PX86STUBMBX)(uintptr_t)X86_STUB_MBX_START;

    pMbx->u32MagicReqResp = X86STUB_MBX_MAGIC_READY;
    asm volatile ("wbinvd");

    for (;;)
    {
        /* Wait for a new request to arrive. */
        while (pMbx->u32MagicReqResp != X86STUB_MBX_MAGIC_REQ);
        asm volatile ("wbinvd");

        switch (pMbx->enmReq)
        {
            case X86STUBMBXREQ_IOPORT_READ:
            {
                uint16_t IoPort = (uint16_t)pMbx->u.IoPort.u32IoPort;

                switch (pMbx->u.IoPort.cbAccess)
                {
                    case 1:
                    {
                        volatile uint8_t bVal = 0;
                        asm volatile ("inb %w1, %b0": "=a"(bVal) : "Nd"(IoPort));
                        pMbx->u.IoPort.u32Val = bVal;
                        break;
                    }
                    case 2:
                    {
                        volatile uint16_t u16Val = 0;
                        asm volatile ("inw %w1, %w0": "=a"(u16Val) : "Nd"(IoPort));
                        pMbx->u.IoPort.u32Val = u16Val;
                        break;
                    }
                    case 4:
                    {
                        volatile uint32_t u32Val = 0;
                        asm volatile ("inl %w1, %0": "=a"(u32Val) : "Nd"(IoPort));
                        pMbx->u.IoPort.u32Val = u32Val;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case X86STUBMBXREQ_IOPORT_WRITE:
            {
                uint16_t IoPort = (uint16_t)pMbx->u.IoPort.u32IoPort;

                switch (pMbx->u.IoPort.cbAccess)
                {
                    case 1:
                    {
                        uint8_t bVal = (uint8_t)pMbx->u.IoPort.u32Val;
                        asm volatile ("outb %b1, %w0": : "Nd"(IoPort), "a"(bVal));
                        break;
                    }
                    case 2:
                    {
                        uint16_t u16Val = (uint16_t)pMbx->u.IoPort.u32Val;
                        asm volatile ("outw %w1, %w0": : "Nd"(IoPort), "a"(u16Val));
                        break;
                    }
                    case 4:
                    {
                        uint32_t u32Val = (uint32_t)pMbx->u.IoPort.u32Val;
                        asm volatile ("outl %1, %w0": : "Nd"(IoPort), "a"(u32Val));
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case X86STUBMBXREQ_MEM32_READ:
            {
                switch (pMbx->u.Mem32.cbAccess)
                {
                    case 1:
                    {
                        pMbx->u.Mem32.u32Val = *(volatile uint8_t *)(uintptr_t)pMbx->u.Mem32.u32MemAddr;
                        break;
                    }
                    case 2:
                    {
                        pMbx->u.Mem32.u32Val = *(volatile uint16_t *)(uintptr_t)pMbx->u.Mem32.u32MemAddr;
                        break;
                    }
                    case 4:
                    {
                        pMbx->u.Mem32.u32Val = *(volatile uint32_t *)(uintptr_t)pMbx->u.Mem32.u32MemAddr;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case X86STUBMBXREQ_MEM32_WRITE:
            {
                switch (pMbx->u.Mem32.cbAccess)
                {
                    case 1:
                    {
                        *(volatile uint8_t *)(uintptr_t)pMbx->u.Mem32.u32MemAddr = (uint8_t)pMbx->u.Mem32.u32Val;
                        break;
                    }
                    case 2:
                    {
                        *(volatile uint8_t *)(uintptr_t)pMbx->u.Mem32.u32MemAddr = (uint16_t)pMbx->u.Mem32.u32Val;
                        break;
                    }
                    case 4:
                    {
                        *(volatile uint8_t *)(uintptr_t)pMbx->u.Mem32.u32MemAddr = pMbx->u.Mem32.u32Val;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case X86STUBMBXREQ_MSR_READ:
            {
                uint32_t idMsr = pMbx->u.Msr.idMsr;
                uint32_t idKey = pMbx->u.Msr.idKey;
                uint32_t u32ValLow = 0;
                uint32_t u32ValHigh = 0;

                asm volatile ("rdmsr": "=a"(u32ValLow), "=d"(u32ValHigh) : "c"(idMsr), "D"(idKey));
                pMbx->u.Msr.u64Val = ((uint64_t)u32ValHigh << 32) | u32ValLow;
                break;
            }
            case X86STUBMBXREQ_MSR_WRITE:
            {
                uint32_t idMsr = pMbx->u.Msr.idMsr;
                uint32_t idKey = pMbx->u.Msr.idKey;
                uint32_t u32ValLow = (uint32_t)pMbx->u.Msr.u64Val;
                uint32_t u32ValHigh = (uint32_t)(pMbx->u.Msr.u64Val >> 32);

                asm volatile ("wrmsr": : "a"(u32ValLow), "d"(u32ValHigh), "c"(idMsr), "D"(idKey));
                break;
            }
            default:
                /* Do nothing */
                break;
        }

        /* Mark that processing is done, response data is in the mailbox and we are ready to accept new requests. */
        asm volatile ("wbinvd");
        pMbx->u32MagicReqResp = X86STUB_MBX_MAGIC_READY;
        asm volatile ("wbinvd");
    }
}

