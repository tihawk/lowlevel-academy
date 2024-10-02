.global _start       // Declare the _start label as global for the linker
.section .text       // Define the section of the code as text (code section)

_start:              // Define the start of the program
    mov r0, pc       // Move the value of the Program Counter (PC) into R0
    sub r0, r0, #8   // Subtract 8 from the value in R0
    bx r0            // Branch to the address in R0
