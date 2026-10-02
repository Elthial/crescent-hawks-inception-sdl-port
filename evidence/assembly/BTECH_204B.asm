;;; Segment 204B (204B:0000)
204B:0000 57 26 39 36 A2 39 7F BC                         W&96.9..       

l1FC5_0868:
	pop	si
	mov	sp,bp
	pop	bp
	retf
1FC5:086D                                        90 00 00              ...
1FC5:0870 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................

;; fn204B_0020  - odd code #1: 204B:0020
fn204B_0020  - odd code #1 proc
	call	dword ptr cs:[0016h]
	push	ax
	mov	ax,cs:[000Eh]
	cmp	al,0h
	jnz	003Ch

l204B_002E:
	mov	ax,cs:[0010h]
	mov	cs:[000Eh],ax
	pop	ax
	jmp	dword ptr cs:[0012h]

;; fn204B_003C - odd code #2: 204B:003C
;;   Called from:
;;     204B:002C (in fn204B_0020  - odd code #1)
fn204B_003C - odd code #2 proc
	dec	al
	mov	cs:[000Eh],ax
	mov	al,20h
	out	20h,al
	pop	ax
	iret

;; Vector and output to I/O port 40, 43 #2: 204B:0048
;;   Called from:
;;     0800:4813 (in fn0800_476D)
Vector and output to I/O port 40, 43 #2 proc
	push	ds
	mov	ax,0h
	mov	ds,ax
	mov	ax,[0020h]
	mov	cs:[0012h],ax
	mov	ax,[0022h]
	mov	cs:[0014h],ax
	mov	word ptr cs:[0010h],10h
	mov	word ptr cs:[000Eh],0h
	mov	ax,186h
	mov	cs:[0016h],ax
	push	cs
	pop	ax
	mov	cs:[0018h],ax
	mov	dx,20h
	push	cs
	pop	ds
	mov	ah,25h
	mov	al,8h
	int	21h
	mov	al,36h
	out	43h,al
	mov	ax,0FFFh
	out	40h,al
	mov	al,ah
	out	40h,al
	pop	ds
	retf

;; Vector and output to I/O port 40, 43: 204B:0091
;;   Called from:
;;     0800:48AE (in fn0800_476D)
Vector and output to I/O port 40, 43 proc
	push	ds
	mov	dx,cs:[0012h]
	mov	ax,cs:[0014h]
	push	ax
	pop	ds
	mov	ah,25h
	mov	al,8h
	int	21h
	mov	al,36h
	out	43h,al
	mov	ax,0FFFFh
	out	40h,al
	mov	al,ah
	out	40h,al
	pop	ds
	retf

;; Timer 8253-5: 204B:00B2
;;   Called from:
;;     204B:0197 (in fn204B_0187)
;;     204B:02D6 (in fn204B_0298)
Timer 8253-5 proc
	push	ax
	mov	al,cl
	out	42h,al
	mov	al,ch
	out	42h,al
	pop	ax
	ret

;; Programmable Peripheral Interface #6 - Speaker? Keyboard?: 204B:00BD
;;   Called from:
;;     204B:02F3 (in fn204B_02E4)
;;     204B:0315 (in fn204B_0306)
Programmable Peripheral Interface #6 - Speaker? Keyboard? proc
	push	ax
	in	al,61h
	or	al,3h
	out	61h,al
	pop	ax
	ret

;; Programmable Peripheral Interface #5 - Speaker? Keyboard?: 204B:00C6
;;   Called from:
;;     204B:0168 (in DMA controller, 8237A-5 #3)
;;     204B:0181 (in fn204B_0179)
Programmable Peripheral Interface #5 - Speaker? Keyboard? proc
	push	ax
	in	al,61h
	and	al,0FCh
	out	61h,al
	pop	ax
	ret

;; fn204B_00CF: 204B:00CF
;;   Called from:
;;     204B:0150 (in fn204B_0139)
fn204B_00CF proc
	push	dx
	push	ax
	mov	dx,12h
	mov	ax,34DEh
	div	cx
	mov	cx,ax
	pop	ax
	pop	dx
	ret
204B:00DE                                           52 50               RP
204B:00E0 2E A1 1C 00 2E 2B 06 1A 00 F7 E1 8B CA 2E 03 0E .....+..........
204B:00F0 1A 00 58 5A C3 2E 8B 0E 1E 00 81 C1 48 92 D1 C9 ..XZ........H...
204B:0100 D1 C9 D1 C9 2E 89 0E 1E 00 C3 2E 89 1E 1A 00 2E ................
204B:0110 89 0E 1C 00 E8 DE FF E8 C4 FF E8 B2 FF E8 92 FF ................
204B:0120 C3 5A 10 53 11 5B 12 72 13 9A 14 D4 15 20 17 80 .Z.S.[.r..... ..
204B:0130 18 F5 19 80 1B 23 1D DE 1E                      .....#...      

;; fn204B_0139: 204B:0139
;;   Called from:
;;     204B:0192 (in fn204B_0187)
;;     204B:02D1 (in fn204B_0298)
fn204B_0139 proc
	push	cx
	push	bx
	push	ax
	mov	ah,0h
	mov	cl,0Ch
	div	cl
	mov	dl,al
	mov	al,ah
	cbw
	shl	ax,1h
	mov	bx,ax
	mov	cx,cs:[bx+121h]
	call	00CFh
	xchg	dx,cx
	neg	cl
	add	cl,8h
	shl	dx,cl
	pop	ax
	pop	bx
	pop	cx
	ret

;; DMA controller, 8237A-5 #3: 204B:0160
;;   Called from:
;;     204B:0294 (in fn204B_024F)
DMA controller, 8237A-5 #3 proc
	push	ax
	mov	ax,186h
	mov	cs:[0016h],ax
	call	00C6h
	mov	al,9Fh
	out	0C0h,al
	mov	al,0BFh
	out	0C0h,al
	mov	al,0DFh
	out	0C0h,al
	pop	ax
	ret

;; fn204B_0179: 204B:0179
;;   Called from:
;;     204B:02E0 (in fn204B_0298)
fn204B_0179 proc
	push	ax
	mov	ax,186h
	mov	cs:[0016h],ax
	call	00C6h
	pop	ax
	ret
204B:0186                   CB                                  .        

;; fn204B_0187: 204B:0187
;;   Called from:
;;     204B:0279 (in fn204B_024F)
fn204B_0187 proc
	push	dx
	push	cx
	test	al,80h
	jz	0192h

l204B_018D:
	mov	cx,0Eh
	jmp	0197h

l204B_0192:
	call	0139h
	mov	cx,dx

l204B_0197:
	call	00B2h
	pop	cx
	pop	dx
	ret
204B:019D                                        CB AC 06              ...
204B:01A0 4C 06 F0 05 98 05 4C 05 FE 04 B6 04 74 04 32 04 L.....L.....t.2.
204B:01B0 F6 03 BA 03 84 03 56 03                         ......V.       

;; DMA controller, 8237A-5 #2: 204B:01B8
;;   Called from:
;;     204B:0275 (in fn204B_024F)
;;     204B:027F (in fn204B_024F)
;;     204B:0285 (in fn204B_024F)
DMA controller, 8237A-5 #2 proc
	push	dx
	push	cx
	push	bx
	push	ax
	mov	ah,0h
	mov	cl,0Ch
	div	cl
	mov	cl,al
	mov	al,ah
	cbw
	shl	ax,1h
	mov	bx,ax
	mov	dx,cs:[bx+19Eh]
	sub	cl,2h
	sar	dx,cl
	pop	ax
	mov	al,80h
	cmp	ah,0h
	jz	01E6h

l204B_01DD:
	mov	al,0A0h
	cmp	ah,1h
	jz	01E6h

l204B_01E4:
	mov	al,0C0h

l204B_01E6:
	mov	ah,dl
	and	ah,0Fh
	or	al,ah
	out	0C0h,al
	mov	cl,4h
	shr	dx,cl
	and	dl,3Fh
	mov	al,dl
	out	0C0h,al
	pop	bx
	pop	cx
	pop	dx
	ret
204B:01FE                                           8B 46               .F
204B:0200 0A E8 B4 FF C3 00 00 00 00 00 00                ...........    

;; DMA controller, 8237A-5: 204B:020B
DMA controller, 8237A-5 proc
	mov	ax,[bp+8h]
	mov	cs:[0205h],ax
	mov	ax,[bp+0Ah]
	mov	cs:[0207h],ax
	mov	ax,[bp+0Ch]
	mov	cs:[020Ah],al
	mov	byte ptr cs:[0209h],1h
	mov	al,93h
	out	0C0h,al
	mov	al,0B3h
	out	0C0h,al
	mov	al,0D3h
	out	0C0h,al
	ret

;; fn204B_0233: 204B:0233
fn204B_0233 proc
	mov	ax,[bp+8h]
	mov	cs:[0205h],ax
	mov	ax,[bp+0Ah]
	mov	cs:[0207h],ax
	mov	ax,[bp+0Ch]
	mov	cs:[020Ah],al
	mov	byte ptr cs:[0209h],1h
	ret

;; fn204B_024F: 204B:024F
fn204B_024F proc
	dec	byte ptr cs:[0209h]
	cmp	byte ptr cs:[0209h],0h
	jnz	0290h

l204B_025C:
	push	ax
	mov	al,cs:[020Ah]
	mov	cs:[0209h],al
	push	ds
	push	si
	lds	si,cs:[0205h]
	xor	ah,ah
	lodsb
	cmp	al,0h
	jz	0291h

l204B_0273:
	xor	ah,ah
	call	01B8h
	lodsb
	call	0187h
	lodsb
	mov	ah,1h
	call	01B8h
	lodsb
	mov	ah,2h
	call	01B8h
	mov	cs:[0205h],si
	pop	si
	pop	ds
	pop	ax

l204B_0290:
	retf

l204B_0291:
	pop	si
	pop	ds
	pop	ax
	call	0160h
	retf

;; fn204B_0298: 204B:0298
fn204B_0298 proc
	dec	byte ptr cs:[0209h]
	cmp	byte ptr cs:[0209h],0h
	jnz	02DCh

l204B_02A5:
	push	ax
	mov	al,cs:[020Ah]
	mov	cs:[0209h],al
	push	ds
	push	si
	lds	si,cs:[0205h]
	lodsb
	cmp	al,0h
	jnz	02BFh

l204B_02BA:
	lodsb
	cmp	al,0h
	jz	02DDh

l204B_02BF:
	mov	cs:[0205h],si
	pop	si
	pop	ds
	push	dx
	push	cx
	test	al,80h
	jz	02D1h

l204B_02CC:
	mov	cx,0Eh
	jmp	02D6h

l204B_02D1:
	call	0139h
	mov	cx,dx

l204B_02D6:
	call	00B2h
	pop	cx
	pop	dx
	pop	ax

l204B_02DC:
	retf

l204B_02DD:
	pop	si
	pop	ds
	pop	ax
	call	0179h
	retf

;; fn204B_02E4: 204B:02E4
;;   Called from:
;;     0800:4827 (in fn0800_476D)
;;     0800:483F (in fn0800_476D)
;;     0800:489C (in fn0800_476D)
fn204B_02E4 proc
	push	bp
	mov	bp,sp
	mov	bx,[bp+6h]
	cmp	bx,0Dh
	jge	0304h

l204B_02EF:
	add	bx,bx
	add	bx,bx
	call	00BDh
	call	word ptr cs:[bx+328h]
	mov	ax,cs:[bx+32Ah]
	mov	cs:[0016h],ax

l204B_0304:
	pop	bp
	retf

;; fn204B_0306: 204B:0306
;;   Called from:
;;     0800:4849 (in fn0800_476D)
;;     0800:4861 (in fn0800_476D)
;;     0800:48A6 (in fn0800_476D)
fn204B_0306 proc
	push	bp
	mov	bp,sp
	mov	bx,[bp+6h]
	cmp	bx,0Dh
	jge	0326h

l204B_0311:
	add	bx,bx
	add	bx,bx
	call	00BDh
	call	word ptr cs:[bx+332h]
	mov	ax,cs:[bx+334h]
	mov	cs:[0016h],ax

l204B_0326:
	pop	bp
	retf
204B:0328                         60 01 86 01 0B 02 4F 02         `.....O.
204B:0330 00 00 79 01 86 01 33 02 98 02 00 00             ..y...3.....   

;; fn204B_033C: 204B:033C
;;   Called from:
;;     0800:4881 (in fn0800_476D)
fn204B_033C proc
	mov	ax,cs:[0016h]
