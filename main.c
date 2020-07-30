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
 * Local memcpy variant.
 *
 * @returns nothing.
 * @param   pvDst                   Where to copy to.
 * @param   pvSrc                   What to copy.
 * @param   cbCopy                  How many bytes to copy.
 */
static void x86StubMemcpy(void *pvDst, const void *pvSrc, size_t cbCopy)
{
    uint8_t *pbDst = (uint8_t *)pbDst;
    const uint8_t *pbSrc = (uint8_t *)pvSrc;

    while (cbCopy--)
        *pbDst++ = *pbSrc++;
}


/**
 * The main C entry point.
 *
 * @returns Nothing (never returns).
 */
void _c_start(void)
{
    PX86STUBMBX pMbx = (PX86STUBMBX)(uintptr_t)X86_STUB_MBX_START;

    pMbx->u32MagicReqResp = X86STUB_MBX_MAGIC_READY;

    for (;;)
    {
        /* Wait for a new request to arrive. */
        while (pMbx->u32MagicReqResp != X86STUB_MBX_MAGIC_REQ);

        /* Copy the request into a local stack buffer. */
        X86STUBMBX Req = { 0 };
        x86StubMemcpy(&Req, pMbx, sizeof(Req));

        switch (Req.enmReq)
        {
            case X86STUBMBXREQ_IOPORT_READ:
            {
                uint32_t u32Data = 0;
                uint16_t IoPort = (uint16_t)Req.u.IoPort.u32IoPort;

                switch (Req.u.IoPort.cbAccess)
                {
                    case 1:
                    {
                        uint8_t bVal = 0;
                        asm volatile ("inb %0, %1": : "a"(bVal), "Nd"(IoPort));
                        u32Data = bVal;
                        break;
                    }
                    case 2:
                    {
                        uint16_t u16Val = 0;
                        asm volatile ("inw %0, %1": : "a"(u16Val), "Nd"(IoPort));
                        u32Data = u16Val;
                        break;
                    }
                    case 4:
                    {
                        uint32_t u32Val = 0;
                        asm volatile ("inl %0, %1": : "a"(u32Val), "Nd"(IoPort));
                        u32Data = u32Val;
                        break;
                    }
                    default:
                        break;

                    *(volatile uint32_t *)&pMbx->u.IoPort.u32Val = u32Data;
                }
                break;
            }
            case X86STUBMBXREQ_IOPORT_WRITE:
            {
                uint16_t IoPort = (uint16_t)Req.u.IoPort.u32IoPort;

                switch (Req.u.IoPort.cbAccess)
                {
                    case 1:
                    {
                        uint8_t bVal = (uint8_t)Req.u.IoPort.u32Val;
                        asm volatile ("outb %0, %1": : "a"(bVal), "Nd"(IoPort));
                        break;
                    }
                    case 2:
                    {
                        uint16_t u16Val = (uint16_t)Req.u.IoPort.u32Val;
                        asm volatile ("outw %0, %1": : "a"(u16Val), "Nd"(IoPort));
                        break;
                    }
                    case 4:
                    {
                        uint32_t u32Val = (uint32_t)Req.u.IoPort.u32Val;
                        asm volatile ("outl %0, %1": : "a"(u32Val), "Nd"(IoPort));
                        break;
                    }
                    default:
                        break;
                }
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

