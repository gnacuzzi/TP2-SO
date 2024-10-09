global setupStackFrame

section .text

entryWrapper:
    call rdi
    

setupStackFrame:
    ; RDI: Dirección de la función (code)
    ; RSI: Base de la pila (stackBase)
    ; RDX: Argumentos (args)
    ; RCX: Número de argumentos (argc)

    mov r8, rsp     ; Preservar rsp actual
    mov r9, rbp     ; Preservar rbp actual
    mov rsp, rdx    ; Cargar sp del nuevo proceso
    mov rbp, rdx    ; Establecer rbp del nuevo proceso
    
    and rsp, -16    ; Alinear la pila a 16 bytes
    
    push 0x0        ; SS
    push rdx        ; RSP
    push 0x202      ; RFLAGS (IF bit set)
    push 0x8        ; CS
    push entryWrapper ; RIP (dirección de entryWrapper)
    
    ; Guardar el estado de los registros
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    
    push rcx        ; argv (tercer argumento original)
    push rsi        ; argc (segundo argumento original)
    
    push rbp
    push rdi        ; RIP (primer argumento original - dirección de la función)
    push rdx        ; Stack base (no necesario, pero mantenido por consistencia)
    push rbx
    push rax
    
    mov rax, rsp    ; Guardar el nuevo RSP en RAX para devolverlo
    mov rsp, r8     ; Restaurar RSP original
    mov rbp, r9     ; Restaurar RBP original
    ret