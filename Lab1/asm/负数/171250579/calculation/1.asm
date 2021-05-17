SECTION .data
msg1        db      'Please input a integer: ', 0h ; 
newLine	db	0xa
negSign db	45

SECTION .bss
x_i:     resb    40  ;输入。最后一位是0，用于求长度时检测
x:    	 resb    40  ;填补后
y_i:     resb    40  ;
y:     	 resb    40 

addation:	 resb	 40
multi:	 	 resb	 40
cache	         resb	 40
subRes   	 resb	 40

sign	resb	4	;X,Y的符号，1负0正
compare	resb	2

 
SECTION .text
global  _start
_start:

;#############################First Integer###########################
;-------input X:
        mov     eax, msg1
        call    sprint
 
        mov     edx, 40       ; 
        mov     ecx, x_i         ; 
        mov     ebx, 0          ; 
        mov     eax, 3          ; 
        int     80h

        mov     eax, x_i     ; 


;判断符号：
	mov	bl,[eax]
	cmp	bl,45
	jnz	positive
	mov	byte [sign],1
	mov	byte [x_i],48
positive:


;-------get length----------eax has len
	call	slen
	sub	eax,1;
	;eax是纯的长度，不包含换行


;-------填补：左边+0，直到21位
	;push	eax
	mov	ecx,40
	sub	ecx,eax 	;补的长度
	mov	ebx,x		;地址
L_0:
	mov	byte [ebx],0
	inc	ebx
	loop	L_0
	
	
	;ebx:第二步起始地址：
	mov	ecx,eax	;计数
	mov	edx,x_i ;x_i起始地址
L_0_r:
	push	ecx
	mov	cl, [edx]
	sub	cl,48
	mov	[ebx],cl
	inc	edx
	inc	ebx
	pop	ecx
	loop	L_0_r


;#############################Second Integer###########################
;-------input Y:
        mov     eax, msg1
        call    sprint
 
        mov     edx, 40
        mov     ecx, y_i       
        mov     ebx, 0         
        mov     eax, 3       
        int     80h

        mov     eax,y_i    

;判断符号：
	mov	bl,[eax]
	cmp	bl,45
	jnz	Y_positive
	mov	byte [sign+1],1
	mov	byte [y_i],48 
Y_positive:
 

;-------get length----------eax has len
	call	slen
	sub	eax,1;
	;eax是纯的长度，不包含换行

;-------填补：
	;push	eax	
	mov	ecx,40
	sub	ecx,eax 	;补的长度
	mov	ebx,y		;地址
L_0_y:
	mov	byte [ebx],0
	inc	ebx
	loop	L_0_y
	
	
	;ebx:第二步起始地址：
	mov	ecx,eax	;计数
	mov	edx,y_i ;y_i起始地址
L_0_r_y:
	push	ecx
	mov	cl, [edx]
	sub	cl,48
	mov	[ebx],cl
	inc	edx
	inc	ebx
	pop	ecx
	loop	L_0_r_y



;绝对值大小判断,[compare] = 1: X大，[compare] = 0,X小 
	mov	ecx,40
L_compare:
	mov	edx,40
	sub	edx,ecx
	mov	al,[x+edx]
	mov	bl,[y+edx]
	cmp	al,bl
	jz	nextComp
	jb	xSmaller
	mov	byte[compare],1

	mov	eax,x
	mov	ebx,y
	call	subtraction

	jmp	compareOver
xSmaller:
	mov	byte[compare],0
	;Y大，
	mov	eax,y
	mov	ebx,x

	call	subtraction

	jmp	compareOver
nextComp:

	loop	L_compare

compareOver:


;现在差在subRes
	call	multiplication
;现在积在multi
	call	XYaddation
;现在和在addation

;符号判断：

	mov	al,[sign]
	mov	bl,[sign+1]
	
	cmp	al,bl
	jz	sameSign
	;不同符号：
	jb	XPos_YNeg

	;X Neg, Y Pos:
	mov	al,[compare]
	cmp	al,0
	jnz	absY_small
	
	mov	ebx,subRes
	call	printNum

	jmp	exit

absY_small:	
	call	printNegSign
	mov	ebx,subRes
	call	printNum

	jmp	exit

sameSign:
	
	;X + Y
	mov	al,[sign]
	cmp	al,0
	jz	notNegSign
	call	printNegSign
notNegSign:
	mov	ebx,addation
	call	printNum
	jmp	exit

XPos_YNeg:
	mov	al,[compare]
	cmp	al,0
	jnz	absY_small2
	call	printNegSign	
	mov	ebx,subRes
	call	printNum
	jmp	exit
absY_small2:	

	mov	ebx,subRes
	call	printNum


exit:
	mov	al,[sign]
	mov	bl,[sign+1]
	
	cmp	al,bl
	jz	sameSign_x
	call	printNegSign

sameSign_x:
	mov	ebx,multi
	call	printNum



	call	quit



;>>>>>>>>>>>>>>>>>>>>>functions:
multiplication:
;-------------乘法----------
	;Y * (X的每一位)，相加
	;按照加法，每一位乘以另一个数，左移动一位，
	mov	ecx,20
	mov	eax,0;记录左移动多少位
L_multi:
	push	ecx

	;把Y复制到cache
	mov	eax,y
	mov	ebx,cache
	call	copy

	;cache X 左边一位
	mov	ebx,cache
	mov	eax,19
	pop	ecx
	push	ecx
	add	eax,ecx
	mov	edx,0
	mov	dl,[x+eax]

	call	bitMulti


	mov	eax,cache
	mov	ebx,20
	pop	ecx
	push	ecx
	sub	ebx,ecx
	call	leftShift
	
	mov	eax,cache
	mov	ebx,multi
	mov	edx,multi
	call	add
	

	pop	ecx
	loop	L_multi
	ret

;-------x+y = addation
XYaddation:
	mov	eax,x
	mov	ebx,y
	mov	edx,addation
	call	add

	ret

add:
	;参数：
	;  	加数首地址：eax,ebx
	;	和首地址:   edx
	push	ecx
	mov	ecx,40
L_add:
	push	ecx
	push	ebx
	push	eax
	push	edx

	add	eax,40
	sub	eax,ecx
	mov	al,[eax]

	add	ebx,40
	sub	ebx,ecx
	mov	bl,[ebx]

	add	al,bl

	add	edx,40
	sub	edx,ecx
	mov	[edx],al

	pop	edx
	pop	eax
	pop	ebx
	pop	ecx

	loop	L_add
	pop	ecx

	mov	eax,edx
	call	handleCarry

	ret

;处理进位
;处理的首地址：eax
handleCarry:
	push	ecx
	push	ebx
	push	edx

	mov	ecx,40
	add	eax,39	;定位到最后一个
L_handleCarry:
	;只能用除法
	mov	bl,[eax]
	mov	edx,0
	push	eax
	mov	eax,0
	mov	al,bl
	mov	ebx,10
	div	ebx
	mov	ebx,eax	
	
	pop	eax
	mov	[eax],dl

	mov	dl,[eax-1]
	add	bl,dl
	mov	[eax-1],bl

	sub	eax,1

	loop	L_handleCarry

	pop	edx
	pop	ebx
	pop	ecx	
	ret

leftShift:
	;左移
	;参数：1,移动数的首地址：eax,2.移动位数：ebx
	push	ecx
	push	edx
	push	eax

	mov	ecx,40
	sub	ecx,ebx
	
L_ls:
	mov	dl,[eax+ebx]
	mov	[eax],dl
	inc	eax

	loop	L_ls

	pop	edx
	add	edx,40;末地址

L_fillWith0:

	cmp	eax,edx
	jz	L_fillEnd
	mov	byte [eax],0
	inc	eax
	jmp	L_fillWith0
L_fillEnd:

	pop	edx
	pop	ecx

	ret

;一个数乘以N(0-9)
;参数：数首地址:ebx, N ：edx

bitMulti:
	push	ecx
	push	ebx

	mov	ecx,40
L_bitMul:
	mov	al,[ebx]
	mul	dl
	mov	[ebx],al
	inc	ebx
	
	loop	L_bitMul

	pop	ebx	
	pop	ecx
	mov	eax,ebx
	call	handleCarry

	ret

;参数:首地址ebx
printNum:
	push	eax
	push	ecx
	push	edx
	
	mov	edx,0;标志位，判断左到右边第一个非0

	mov	ecx,40
L_print_num:
	mov	eax,0
	mov	al,[ebx]
	
	cmp	al,0
	jz	is0
	mov	edx,1
	call	iprint
	jmp	moveOn_p
is0:
	cmp	edx,1

	jb	moveOn_p	
	call	iprint

moveOn_p:
	inc	ebx


	cmp	edx,1
	jz	cont
	cmp	ecx,1
	jnz	cont
	mov	eax,0
	call	iprint
cont:
	loop	L_print_num

	call	nextLine

	pop	edx
	pop	ecx
	pop	eax
	ret

;复制
;参数：原：eax,目标地址：ebx
copy:
	push	ecx
	push	edx
	mov	ecx,40
L_copy:
	mov	dl,[eax]
	mov	[ebx],dl
	inc	eax
	inc	ebx
	loop	L_copy
	
	pop	edx
	pop	ecx
	ret

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

printNegSign:
	push	eax
	push	ebx
	push	ecx
	push	edx
	mov	eax,4
	mov	ebx,1
	mov	ecx,negSign
	mov	edx,1
	int	80H
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret



;参数：X(eax) -- Y(ebx) [X>=Y],存到subRes
subtraction:
	push	edx
	push	ecx

	mov	ecx,39
	mov	edx,0 ;carry 判断位
L_sub:
	
	push	eax
	push	ebx
	mov	al,[eax+ecx]
	mov	bl,[ebx+ecx]

	add	bl,dl  ;b+c
	cmp	al,bl
	jb	borrowBit
	;不借位：
	sub	al,bl
	mov	[subRes+ecx],al
	mov	edx,0

	jmp	nextLoop

borrowBit:	;借位:
	add	al,10
	sub	al,bl
	mov	[subRes+ecx],al
	mov	edx,1
nextLoop:
	

	pop	ebx
	pop	eax
	
	loop	L_sub

	mov	byte [subRes],0
	
	pop	ecx
	pop	edx
	ret

;==========打印寄存器=========
;参数在eax中

iprint:
    push    eax            
    push    ecx            
    push    edx            
    push    esi            
    mov     ecx, 0          ;整数的位数
	;参数：

divideLoop:
    inc     ecx             ; 
    mov     edx, 0          ; 
    mov     esi, 10         ; 
    idiv    esi             ; eax /=10  商在eax，余数在edx   余数就是当前位——最左边的位
    add     edx, 48         ;
    push    edx             ; 
    cmp     eax, 0          ; 能不能再除
    jnz     divideLoop      ; 
 
printLoop:
    dec     ecx             
    mov     eax, esp     
    call    sprint         
    pop     eax            
    cmp     ecx, 0         
    jnz     printLoop       
 
    pop     esi            
    pop     edx            
    pop     ecx            
    pop     eax            
    ret