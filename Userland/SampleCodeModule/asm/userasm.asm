GLOBAL dividebyzeroexception
GLOBAL invalidoperationexception

dividebyzeroexception:
    mov rax, 0
    div rax
    ret

invalidoperationexception:
	db	0xFF, 0xFF    ; invalid opcode
    ret