global myPrint
;void myPrint(char *,int len,int color);
;color: 0-default, 1-red
;EdwinXu
section .data
	color_default:  db  1Bh, '[37;0m', 0 
	.length            equ $ - color_default
	color_red:      db  1Bh, '[31;1m', 0
	.length            equ $ - color_red  

section .text

myPrint:
	mov eax, [esp+12]
	cmp eax, 0
	je default_color
	
;set red 
	mov eax, 4
	mov ebx, 1
	mov ecx, color_red
	mov edx, color_red.length
	int 80h
	jmp color_end
;set default color:
default_color:
	mov eax, 4
	mov ebx, 1
	mov ecx, color_default
	mov edx, color_default.length
	int 80h

;print the str
color_end:
	mov eax, 4
	mov ebx, 1
	mov ecx, [esp+4] ;str
	mov edx, [esp+8] ;len
	int 80h
;recover the color
	mov eax, 4
	mov ebx, 1
	mov ecx, color_default
	mov edx, color_default.length
	int 80h
	
	ret

