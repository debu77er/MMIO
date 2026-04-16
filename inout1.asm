; its not mmio
; nasm -f elf32 program.asm -o program.o
;    Link (if needed) or run in an environment that allows direct port I/O.
;    Note: Direct port I/O typically requires kernel privileges. Running this code as a regular user may result in a permission error. To test, you might need to run it as root or within a kernel module.
;
;    Caution: Be careful with port I/O operations on real hardware. Incorrect use can cause system instability.
;

section .text
    global _start

_start:
    ; Read keyboard status from port 0x64
    mov dx, 0x64
    in al, dx           ; Read status byte into al

    ; Check if output buffer is full (bit 0)
    test al, 1
    jz no_data           ; If zero, no data available

    ; Read data from port 0x60
    mov dx, 0x60
    in al, dx           ; Read keystroke data

    ; (Optional) Do something with the data
    ; For example, store it somewhere or process it

no_data:
    ; Exit the program
    mov eax, 1          ; sys_exit system call
    xor ebx, ebx        ; Exit code 0
    int 0x80