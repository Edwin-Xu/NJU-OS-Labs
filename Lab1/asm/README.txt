using command dd:
	dd if=boot.bin of=a.img bs=512 count=1 conv=conv=notrunc
	watch out: no spaces 'of' 'if' and so on
