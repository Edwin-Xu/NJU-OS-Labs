SECTION .data
msg1        db      'Please input a integer: ', 0h ; 
newLine	db	0xa

SECTION .bss
x_i:     resb    25  ;输入。最后一位是0，用于求长度时检测
x:    	 resb    25  ;填补后
y_i:     resb    25  ;
y:     	 resb    25 
xCache:	 resd	 3;
yCache:	 resd	 3; 
addation:	 resd	 3
multi:	 resd	 5

 
SECTION .text
global  _start
_start:

;#############################First Integer###########################
;-------input X:
        mov     eax, msg1
        call    sprint
 
        mov     edx, 21        ; number of bytes to read
        mov     ecx, x_i         ; reserved space to store our input (known as a buffer)
        mov     ebx, 0          ; write to the STDIN file
        mov     eax, 3          ; invoke SYS_READ (kernel opcode 3)
        int     80h

        mov     eax, x_i     ; move our buffer into eax (Note: input contains a linefeed-newline)


;-------get length----------eax has len
	call	slen
	sub	eax,1;
	;eax是纯的长度，不包含换行


;-------填补：左边+‘0’，直到24位
	;push	eax
	mov	ecx,24
	sub	ecx,eax 	;补的长度
	mov	ebx,x		;地址
L_0:
	mov	byte [ebx],'0'
	inc	ebx
	loop	L_0
	
	
	;ebx:第二步起始地址：
	mov	ecx,eax	;计数
	mov	edx,x_i ;x_i起始地址
L_0_r:
	push	ecx
	mov	cl, [edx]
	mov	[ebx],cl
	inc	edx
	inc	ebx
	pop	ecx
	loop	L_0_r



;-------get integer------------
	mov	edx,x	
	call	getInt 		;返回在ebx
	mov	dword[xCache],ebx
	

	mov	edx,x+8
	call	getInt 		;返回在ebx
	mov	dword[xCache+4],ebx

	mov	edx,x+16
	call	getInt 		;返回在ebx
	mov	dword[xCache+8],ebx



;#############################Second Integer###########################
;-------input Y:
        mov     eax, msg1
        call    sprint
 
        mov     edx, 21        
        mov     ecx, y_i       
        mov     ebx, 0         
        mov     eax, 3       
        int     80h

        mov     eax,y_i     

;-------get length----------eax has len
	call	slen
	sub	eax,1;
	;eax是纯的长度，不包含换行


;-------填补：左边+‘0’，直到20位
	;push	eax	;长度  之前忘了pop了，debug好久
	mov	ecx,24
	sub	ecx,eax 	;补的长度
	mov	ebx,y		;地址
L_0_y:
	mov	byte [ebx],'0'
	inc	ebx
	loop	L_0_y
	
	
	;ebx:第二步起始地址：
	mov	ecx,eax	;计数
	mov	edx,y_i ;y_i起始地址
L_0_r_y:
	push	ecx
	mov	cl, [edx]
	mov	[ebx],cl
	inc	edx
	inc	ebx
	pop	ecx
	loop	L_0_r_y


;-------get integer------------
	mov	edx,y	
	call	getInt 		;返回在ebx
	mov	dword [yCache],ebx
	

	mov	edx,y+8
	call	getInt 		;返回在ebx
	mov	dword [yCache+4],ebx

	mov	edx,y+16
	call	getInt 		;返回在ebx
	mov	dword [yCache+8],ebx





;add--------------------------------加法：
	;--->最低寄存器相加
	mov	eax,[xCache+8]

	mov	ebx,[yCache+8]


	add	eax,ebx		;第三个对位寄存器相加，不会溢出，但是需要把超出8位的部分加到第二个寄存器
	cmp	eax,100000000
	jnb	overflow1 
	;eax<100000000	
	mov	edx,0;-----------edx:carry
	jmp	continue1
overflow1:
	;eax>=100000000
	sub	eax,100000000	
	mov	edx,1

continue1:
	mov	[addation+8],eax	;加法最低寄存器

	;--->中间寄存器相加
	mov	eax,[xCache+4]


	mov	ebx,[yCache+4]

	add	eax,edx	;进位
	add	eax,ebx

	cmp	eax,100000000
	jnb	overflow2
	;eax<100000000	
	mov	edx,0
	jmp	continue2
overflow2:
	;eax>=100000000
	sub	eax,100000000	
	mov	edx,1

continue2:
	mov	[addation+4],eax	;加法中间寄存器

	;--->最高寄存器相加
	mov	eax,[xCache]
	mov	ebx,[yCache]
	add	eax,edx	;进位
	add	eax,ebx
	mov	[addation],eax	;加法最高寄存器	

	mov	eax,[addation]
	cmp	eax,0
	jz	nextOutput1
	call	iprint
nextOutput1:
	mov	eax,[addation+4]
	cmp	eax,0
	jz	nextOutput2
	call	iprint	
nextOutput2:
	mov	eax,[addation+8]
	call	iprint


	call	nextLine




;---------------------------乘法
;--------------->>>>>>方法1：循环向加法：
	;比较大小，确定循环数：
	mov	edx,0
	;如果数X大edx = 1, 否则edx=0
	;比较高位：
	mov	eax,[xCache]
	mov	ebx,[yCache]
	cmp	eax,ebx
	jz	nextCompare1
	jnb	xBigger
	jb	yBigger
	
nextCompare1:
	;比较中间位：
	mov	eax,[xCache+4]
	mov	ebx,[yCache+4]
	cmp	eax,ebx
	jz	comFinished 
	jnb	xBigger
	jb	yBigger

xBigger:
	mov	edx,1
	jmp	comFinished
yBigger:
	mov	edx,0

comFinished:

	cmp	edx,1
	jz	main
	;Y大，切换：
	mov	eax,[xCache]
	mov	ebx,[yCache]
	mov	[xCache],ebx
	mov	[yCache],eax
	mov	eax,[xCache+4]
	mov	ebx,[yCache+4]
	mov	[xCache+4],ebx
	mov	[yCache+4],eax
	mov	eax,[xCache+8]
	mov	ebx,[yCache+8]
	mov	[xCache+8],ebx
	mov	[yCache+8],eax


main:
	;下面以数Y作为循环数，上面的比较后交换，x和y
	;可以直接使用X对位加multi，然后写一个判断进位的函数，每次加都判断并处理
	
	;Y: a2,b2,c2
	;----->以C2为循环：
	mov	ecx,[yCache+8]
Loop_c:
	call	addXCache
	call	handleCarry
	loop	Loop_c

	;----->以b2为循环：
	mov	ecx,[yCache+4]
	cmp	ecx,0
	jz	nextLoop1
Loop_b:
	push	ecx

	mov	ecx,100000000
inner_loop:
	call	addXCache
	call	handleCarry
	loop	inner_loop

	pop	ecx
	loop	Loop_b

nextLoop1:

	;----->以a2为循环：
	mov	ecx,[yCache]
	cmp	ecx,0
	jz	nextLoop2
Loop_a:
	push	ecx

	mov	ecx,100000000
inner_loop_a:
	push	ecx
	mov	ecx,100000000

inner_loop_a_2:
	call	addXCache
	call	handleCarry
	loop	inner_loop_a_2

	pop	ecx
	loop	inner_loop_a

	pop	ecx
	loop	Loop_a

nextLoop2:
	call	resPrint
	call	nextLine

        call    quit






;------------------------>>>>>>>>>functions:

;处理进位
handleCarry:
	push	eax
	push	ebx

	mov	eax,[multi+16]
	cmp	eax,100000000
	jb	notCarry1
	;has carry:
	mov	ebx,[multi+12]
	inc	ebx
	mov	[multi+12],ebx
	sub	eax,100000000
	mov	[multi+16],eax
notCarry1:
	
	mov	eax,[multi+12]
	cmp	eax,100000000
	jb	notCarry2
	;has carry:
	mov	ebx,[multi+8]
	inc	ebx
	mov	[multi+8],ebx
	sub	eax,100000000
	mov	[multi+12],eax
notCarry2:
	
	mov	eax,[multi+8]
	cmp	eax,100000000
	jb	notCarry3
	;has carry:
	mov	ebx,[multi+4]
	inc	ebx
	mov	[multi+4],ebx
	sub	eax,100000000
	mov	[multi+8],eax
notCarry3:

	mov	eax,[multi+4]
	cmp	eax,100000000
	jb	notCarry4
	;has carry:
	mov	ebx,[multi]
	inc	ebx
	mov	[multi],ebx
	sub	eax,100000000
	mov	[multi+4],eax
notCarry4:
	pop	ebx
	pop	eax
	ret

;multi每次都加XCache
addXCache:
	push	eax
	push	ebx

	mov	ebx,[xCache+8]
	mov	eax,[multi+16]
	add	eax,ebx
	mov	[multi+16],eax

	mov	ebx,[xCache+4]
	mov	eax,[multi+12]
	add	eax,ebx
	mov	[multi+12],eax

	mov	ebx,[xCache]
	mov	eax,[multi+8]
	add	eax,ebx
	mov	[multi+8],eax

	pop	ebx
	pop	eax
	ret


;打印结果
resPrint:
	mov	eax,[multi]
	cmp	eax,0
	jz	ignore1
	call	iprint
ignore1:
	mov	eax,[multi+4]
	cmp	eax,0
	jz	ignore2
	call	iprint
ignore2:
	mov	eax,[multi+8]
	cmp	eax,0
	jz	ignore3
	call	iprint
ignore3:
	mov	eax,[multi+12]
	cmp	eax,0
	jz	ignore4
	call	iprint
ignore4:
	mov	eax,[multi+16]
	call	iprint
	ret





;------------------------------------------
; int slen(String message)
; String length calculation function
slen:
    push    ebx
    mov     ebx, eax
 
nextchar:
    cmp     byte [eax], 0
    jz      finished
    inc     eax
    jmp     nextchar
 
finished:
    sub     eax, ebx
    pop     ebx
    ret
 
 
;------------------------------------------
; void sprint(String message)
; String printing function
;dont't use it when you have use the 4 registers to keep values----------------------
sprint:
    push    edx
    push    ecx
    push    ebx
    push    eax
    call    slen
 
    mov     edx, eax
    pop     eax
 
    mov     ecx, eax
    mov     ebx, 1
    mov     eax, 4
    int     80h
 
    pop     ebx
    pop     ecx
    pop     edx
    ret
 
 
;退出
quit:
    mov     ebx, 0
    mov     eax, 1
    int     80h
    ret






	
;ebx getInt(edx) 获取整数

getInt:
	push	eax
	push	ecx
	
	mov	ecx,8

	 ;using edx as the address of current byte of x
	mov	ebx,0	 	;using ebx as the result

L1:
	movzx	eax,byte [edx]  ;--------------Finally, it works--
	sub	eax,0x30
	
	;X10^n
	push	ecx
	sub	ecx,0x1

	;如果是一位数，就直接跳过
	push	ecx
	cmp	ecx,0x0
	pop	ecx
	jz	oneDigit

	;计算幂：M*10^N
	push	edx

L_i:
	mov	edx,0xA
	mul	edx
	loop 	L_i
	pop	edx

oneDigit:	
	pop	ecx
	add	ebx,eax
	inc	edx
	loop	L1

	pop	ecx
	pop	eax
	ret
	

nextLine:
	push	eax
	push	ebx
	push	ecx
	push	edx
	mov	eax,4
	mov	ebx,1
	mov	ecx,newLine
	mov	edx,1
	int	80H
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret



;==========打印寄存器=========
; void iprint(Integer number)
; Integer printing function (itoa)
;参数在eax中

iprint:
    push    eax            
    push    ecx            
    push    edx            
    push    esi            
    mov     ecx, 0          ;整数的位数
	;参数：

divideLoop:
    inc     ecx             ; count each byte to print - number of characters
    mov     edx, 0          ; empty edx
    mov     esi, 10         ; mov 10 into esi
    idiv    esi             ; eax /=10  商在eax，余数在edx   余数就是当前位——最左边的位
    add     edx, 48         ;after a divide instruction
    push    edx             ; 
    cmp     eax, 0          ; 能不能再除
    jnz     divideLoop      ; 
 
printLoop:
    dec     ecx             ; count down each byte that we put on the stack
    mov     eax, esp        ; mov the stack pointer into eax for printing
    call    sprint          ; call our string print function
    pop     eax             ; remove last character from the stack to move esp forward
    cmp     ecx, 0          ; have we printed all bytes we pushed onto the stack?
    jnz     printLoop       ; jump is not zero to the label printLoop
 
    pop     esi            
    pop     edx            
    pop     ecx            
    pop     eax            
    ret
	
 
 
















