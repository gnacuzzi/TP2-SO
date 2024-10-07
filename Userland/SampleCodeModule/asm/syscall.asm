GLOBAL write
GLOBAL read_char
GLOBAL clear_screen
GLOBAL get_seconds
GLOBAL get_minutes
GLOBAL get_hours
GLOBAL setlettersize
GLOBAL getRegs
GLOBAL make_sound
GLOBAL draw_rectangle
GLOBAL get_ticks
GLOBAL wait_delta
GLOBAL syscreateProcess
GLOBAL sysmalloc
GLOBAL sysfree
GLOBAL sysgetMemInfo
GLOBAL sysgetpid
GLOBAL syskillProcess
GLOBAL syschangePriority
GLOBAL sysblockProcess
GLOBAL sysunblockProcess
GLOBAL sysyield
GLOBAL syswaitProcess

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
    mov rax, 8
    int 80h
    ret

draw_rectangle:
    mov rax, 9
    mov r10, rcx
    int 80h
    ret

get_ticks:
    mov rax, 10
    int 80h
    ret

wait_delta:
    mov rax, 11
    int 80h
    ret

sysmalloc:
    mov rax, 12
    int 80h
    ret

sysfree:
    mov rax, 13
    int 80h
    ret

sysgetMemInfo:
    mov rax, 14
    int 80h
    ret

syscreateProcess:
    mov rax, 15
    int 80h
    ret

sysgetpid:
    mov rax, 16
    int 80h
    ret


syskillProcess:
    mov rax, 18
    int 80h
    ret

syschangePriority:
    mov rax, 19
    int 80h
    ret

sysblockProcess:
    mov rax, 20
    int 80h
    ret

sysunblockProcess:
    mov rax, 21
    int 80h
    ret

sysyield:
    mov rax, 22
    int 80h
    ret

syswaitProcess:
    mov rax, 23
    int 80h
    ret