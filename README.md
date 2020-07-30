# x86-stub - Executing requests on the x86 side of an AMD CPU forwarded from the PSP stub

## Description

This little x86 stub replaces any UEFI firmware loaded by the PSP and is used to execute
requests like I/O port reads and writes or memory accesses from the x86 BSP of an AMD CPU.
The requests are forwarded by the PSP stub from PSPEmu using a fixed mailbox area in DRAM.

This stub is tailored towards AMD CPUs and expects DRAM to be fully initialized when it
starts executing so don't expect it to work somewhere else.
