; CPU PIPELINE SIMULATOR - SIMPLIFIED WORKING VERSION
; Save as: pipeline.asm

ORG 100h

jmp start

; =========== DATA SECTION ===========
title_msg DB 'CPU Pipeline Simulator', 13, 10
          DB '========================', 13, 10, '$'

prompt_msg DB 13, 10, 'Enter instruction (ADD/SUB/LOAD/STORE/MUL/JMP)', 13, 10
           DB 'Type END to finish', 13, 10
           DB '-> $'

result_msg DB 13, 10, '=== RESULTS ===', 13, 10, '$'
np_msg DB 'Non-Pipelined Cycles: $'
p_msg DB 'Pipelined Cycles: $'
speed_msg DB 'Speedup: $'
x_msg DB 'x faster$'
saved_msg DB 'Cycles Saved: $'
inst_msg DB 'Total Instructions: $'

newline DB 13, 10, '$'

; Variables
inst_count DB 0      ; Instruction counter
np_cycles DW 0       ; Non-pipelined cycles
p_cycles DW 0        ; Pipelined cycles

; Buffer for input
input_buffer DB 10 DUP(0)

; =========== CODE SECTION ===========
start:
    ; Display title
    mov ah, 09h
    mov dx, offset title_msg
    int 21h
    
    ; Initialize instruction counter
    mov inst_count, 0
    
input_loop:
    ; Display prompt
    mov ah, 09h
    mov dx, offset prompt_msg
    int 21h
    
    ; Get input from user
    mov di, offset input_buffer
    
read_char:
    mov ah, 01h      ; Read character with echo
    int 21h
    
    cmp al, 13       ; Check for Enter key
    je process_input
    
    ; Convert lowercase to uppercase
    cmp al, 'a'
    jb store_char
    cmp al, 'z'
    ja store_char
    sub al, 32       ; Convert to uppercase
    
store_char:
    mov [di], al
    inc di
    jmp read_char
    
process_input:
    mov byte ptr [di], '$'  ; Null terminate string
    
    ; Check if buffer is empty
    mov al, [input_buffer]
    cmp al, '$'
    je input_loop
    
    ; Check for END command
    mov si, offset input_buffer
    cmp byte ptr [si], 'E'
    jne not_end
    cmp byte ptr [si+1], 'N'
    jne not_end
    cmp byte ptr [si+2], 'D'
    jne not_end
    jmp calculate
    
not_end:
    ; Validate instruction (simple check for first letter)
    mov al, [si]
    
    ; Check valid instructions
    cmp al, 'A'
    je valid_inst
    cmp al, 'S'
    je valid_inst
    cmp al, 'L'
    je valid_inst
    cmp al, 'M'
    je valid_inst
    cmp al, 'J'
    je valid_inst
    
    ; Invalid instruction
    mov ah, 09h
    mov dx, offset newline
    int 21h
    jmp input_loop
    
valid_inst:
    ; Increment instruction count
    inc inst_count
    
    ; Clear buffer for next input
    mov di, offset input_buffer
    mov cx, 10
clear_buf:
    mov byte ptr [di], 0
    inc di
    loop clear_buf
    
    jmp input_loop

calculate:
    ; Display separator
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset result_msg
    int 21h
    
    ; Display instruction count
    mov dx, offset inst_msg
    int 21h
    
    mov al, inst_count
    mov ah, 0
    call print_number
    
    ; Calculate non-pipelined cycles (inst_count * 5)
    mov al, inst_count
    mov ah, 0
    mov bl, 5
    mul bl
    mov np_cycles, ax
    
    ; Display non-pipelined cycles
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset np_msg
    int 21h
    
    mov ax, np_cycles
    call print_number
    
    ; Calculate pipelined cycles (5 + (inst_count - 1))
    mov al, inst_count
    mov ah, 0
    add ax, 4        ; 5-1 = 4
    mov p_cycles, ax
    
    ; Display pipelined cycles
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset p_msg
    int 21h
    
    mov ax, p_cycles
    call print_number
    
    ; Calculate and display speedup
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset speed_msg
    int 21h
    
    ; Calculate speedup (integer division)
    mov ax, np_cycles
    mov bx, p_cycles
    cmp bx, 0
    je skip_speedup
    
    mov dx, 0
    div bx
    call print_number
    
    mov ah, 09h
    mov dx, offset x_msg
    int 21h
    
skip_speedup:
    ; Display cycles saved
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset saved_msg
    int 21h
    
    mov ax, np_cycles
    sub ax, p_cycles
    call print_number
    
    ; Display analysis
    call show_analysis
    
    ; Wait for key press
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset newline
    int 21h
    
    mov dx, offset exit_msg
    int 21h
    
    mov ah, 07h
    int 21h
    
    ; Exit to DOS
    mov ah, 4Ch
    int 21h

; =========== SUBROUTINES ===========

; Print number (0-255)
print_number:
    push ax
    push bx
    push cx
    push dx
    
    ; Handle zero case
    cmp ax, 0
    jne convert_num
    
    mov ah, 02h
    mov dl, '0'
    int 21h
    jmp print_done
    
convert_num:
    mov cx, 0
    mov bx, 10
    
div_loop:
    mov dx, 0
    div bx
    push dx
    inc cx
    cmp ax, 0
    jne div_loop
    
print_loop:
    pop dx
    add dl, '0'
    mov ah, 02h
    int 21h
    loop print_loop
    
print_done:
    pop dx
    pop cx
    pop bx
    pop ax
    ret

; Show analysis information
show_analysis:
    push ax
    push dx
    
    mov ah, 09h
    mov dx, offset newline
    int 21h
    mov dx, offset newline
    int 21h
    
    mov dx, offset analysis1
    int 21h
    
    mov dx, offset analysis2
    int 21h
    
    mov dx, offset analysis3
    int 21h
    
    mov dx, offset stages_msg
    int 21h
    
    pop dx
    pop ax
    ret

; =========== MESSAGES ===========
analysis1 DB 'ANALYSIS:', 13, 10, '$'
analysis2 DB 'Non-Pipeline: Each instruction runs sequentially', 13, 10, '$'
analysis3 DB 'Pipeline: Instructions overlap in 5 stages', 13, 10, '$'

stages_msg DB 13, 10, '5-STAGE PIPELINE:', 13, 10
           DB '1. IF - Instruction Fetch', 13, 10
           DB '2. ID - Instruction Decode', 13, 10
           DB '3. EX - Execute', 13, 10
           DB '4. MEM - Memory Access', 13, 10
           DB '5. WB - Write Back', 13, 10, '$'

exit_msg DB 'Press any key to exit...$'

END