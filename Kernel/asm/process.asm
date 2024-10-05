section .text
global setupStackFrame

setupStackFrame:
global setupStackFrame

setupStackFrame:
    mov r8, rsp     ; Preservar rsp actual
    mov r9, rbp     ; Preservar rbp actual
    mov rsp, rdx    ; Cargar sp del nuevo proceso
    mov rbp, rdx    ; Establecer rbp del nuevo proceso
    
    push 0x0        ; SS
    push rdx        ; RSP
    push 0x202      ; RFLAGS (IF bit set)
    push 0x8        ; CS
    push rdi        ; RIP (dirección de executeProcess)
    
    mov rdi, rsi    ; Primer argumento de executeProcess (código de la función principal)
    mov rsi, rcx    ; Segundo argumento (args)
    
    ; Guardar el estado de los registros
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push rdx
    push rcx
    push rbx
    push rax
    
    mov rax, rsp    ; Guardar el nuevo RSP en RAX para devolverlo
    mov rsp, r8     ; Restaurar RSP original
    mov rbp, r9     ; Restaurar RBP original
    ret