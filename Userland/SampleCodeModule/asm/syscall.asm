GLOBAL write
GLOBAL read_char
GLOBAL clear_screen
GLOBAL get_seconds
GLOBAL get_minutes
GLOBAL get_hours
GLOBAL setlettersize
GLOBAL getRegs
GLOBAL get_control

GLOBAL make_sound

GLOBAL draw_rectangle

GLOBAL get_ticks

GLOBAL wait_delta

GLOBAL testmm

read_char:
    mov rax, 0
    int 80h
    ret

write:
    mov rax, 1
    int 80h
    ret

clear_screen:
    mov rax, 2
    int 80h
    ret

get_seconds:
    mov rax, 3
    int 80h
    ret

get_minutes:
    mov rax, 4
    int 80h
    ret

get_hours:
    mov rax, 5
    int 80h
    ret

setlettersize:
    mov rax, 6
    int 80h
    ret

getRegs:
    mov rax, 7
    int 80h
    ret

make_sound:
    mov rax, 9
    int 80h
    ret

draw_rectangle:
    mov rax, 10
    mov r10, rcx
    int 80h
    ret

get_ticks:
    mov rax, 11
    int 80h
    ret

wait_delta:
    mov rax, 12
    int 80h
    ret

testmm:
    mov rax, 13
    int 80h
    ret