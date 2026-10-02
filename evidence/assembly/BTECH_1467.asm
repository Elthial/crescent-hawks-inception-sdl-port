;;; Segment 1467 (1467:0000)
1467:0000 5D CB                                           ].             

;; fn1467_0002: 1467:0002
;;   Called from:
;;     0800:4D92 (in fn0800_4D57)
;;     1CD3:0E93 (in Mech string references - Locust / Wasp / Chameleon Selection?)
fn1467_0002 proc
	push	bp
	mov	bp,sp
	mov	ax,32h
	call	far 207Fh:2FDCh
	push	di
	push	si
	cmp	word ptr [bp+6h],0h
	jz	005Bh

l1467_0015:
	mov	word ptr [bp-26h],0h

l1467_001A:
	mov	ax,11h
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	mov	byte ptr es:[bx+0C620h],8h
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],8h
	jl	001Ah

l1467_0035:
	mov	word ptr [bp-26h],0h
	jmp	003Fh

l1467_003C:
	inc	word ptr [bp-26h]

l1467_003F:
	cmp	word ptr [bp-26h],4h
	jge	00BDh

l1467_0045:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	si,ax
	mov	al,0FFh
	mov	es:[si+0C79Eh],al
	mov	es:[si+0C79Dh],al
	jmp	003Ch

l1467_005B:
	mov	word ptr [bp-26h],0h

l1467_0060:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	si,ax
	mov	es,[5528h]
	cmp	byte ptr es:[si+0C724h],0FFh
	jz	00B4h

l1467_0074:
	cmp	byte ptr es:[si+0C79Dh],0FFh
	jz	008Fh

l1467_007C:
	mov	al,[bp-26h]
	mov	cx,ax
	mov	al,11h
	mul	byte ptr es:[si+0C79Dh]
	mov	bx,ax
	mov	es:[bx+0C620h],cl

l1467_008F:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	al,es:[bx+0C79Eh]
	mov	[bp-32h],al
	cmp	al,0FFh
	jz	00B4h

l1467_00A3:
	mov	al,[bp-26h]
	mov	cx,ax
	mov	al,11h
	mul	byte ptr [bp-32h]
	mov	bx,ax
	mov	es:[bx+0C620h],cl

l1467_00B4:
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],4h
	jl	0060h

l1467_00BD:
	mov	word ptr [bp-26h],0h

l1467_00C2:
	mov	si,[bp-26h]
	shl	si,1h
	mov	word ptr [bp+si-18h],0FFFFh
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],8h
	jl	00C2h

l1467_00D5:
	mov	word ptr [bp-1Eh],0h

l1467_00DA:
	mov	ax,6h
	push	ax
	call	far 1E56h:0281h
	add	sp,2h
	call	far 1E56h:0388h
	sub	ax,ax
	push	ax
	call	far 1E56h:0004h
	add	sp,2h
	call	far 0800h:28A2h
	mov	ax,2622h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	es,[552Ah]
	mov	ax,es:[374Eh]
	mov	es,[552Ch]
	mov	es:[0202h],ax
	sub	ax,ax
	mov	[bp-20h],ax
	mov	[bp-26h],ax
	jmp	01C6h

l1467_0123:
	mov	es,[5530h]
	mov	byte ptr es:[0012h],20h
	mov	ax,7Dh
	imul	word ptr [bp-22h]
	mov	bx,ax
	lea	ax,[bx+0C724h]
	mov	dx,3092h
	push	dx
	push	ax
	mov	ax,13h
	mov	dx,3092h
	push	dx
	push	ax
	call	far 207Fh:3B68h
	add	sp,8h
	mov	ax,12h
	mov	dx,3092h
	push	dx
	push	ax
	call	far 207Fh:3B9Eh
	add	sp,4h
	dec	ax
	mov	[bp-2Ah],ax
	jmp	0183h

l1467_0164:
	mov	bx,[bp-2Ah]
	mov	es,[5530h]
	cmp	byte ptr es:[bx+12h],20h
	jnz	017Bh

l1467_0173:
	mov	byte ptr es:[bx+12h],0h
	jmp	0180h

l1467_017B:
	mov	word ptr [bp-2Ah],1h

l1467_0180:
	dec	word ptr [bp-2Ah]

l1467_0183:
	cmp	word ptr [bp-2Ah],0h
	jnz	0164h

l1467_0189:
	mov	ax,12h
	mov	dx,3092h
	push	dx
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,7Dh
	imul	word ptr [bp-22h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C79Dh]
	sub	ah,ah
	cmp	ax,[bp-26h]
	jnz	01B6h

l1467_01B1:
	mov	ax,264Dh
	jmp	01B9h

l1467_01B6:
	mov	ax,2655h

l1467_01B9:
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h

l1467_01C3:
	inc	word ptr [bp-26h]

l1467_01C6:
	cmp	word ptr [bp-26h],8h
	jge	0234h

l1467_01CC:
	mov	ax,11h
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C614h]
	mov	[bp-32h],al
	cmp	al,0FFh
	jz	01C3h

l1467_01E4:
	mov	si,[bp-20h]
	inc	word ptr [bp-20h]
	shl	si,1h
	mov	ax,[bp-26h]
	mov	[bp+si-18h],ax
	mov	al,[bp-32h]
	cbw
	mov	bx,ax
	shl	bx,1h
	shl	bx,1h
	mov	es,[552Eh]
	push	word ptr es:[bx+1CCh]
	push	word ptr es:[bx+1CAh]
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,11h
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C620h]
	cbw
	mov	[bp-22h],ax
	cmp	ax,8h
	jz	022Fh

l1467_022C:
	jmp	0123h

l1467_022F:
	mov	ax,2643h
	jmp	01B9h

l1467_0234:
	mov	ax,265Dh
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	es,[552Ch]
	mov	ax,[bp-20h]
	inc	ax
	mov	es:[0206h],ax
	mov	es,[552Ah]
	mov	word ptr es:[374Eh],0Ah
	mov	word ptr [bp-26h],0h
	jmp	02D4h

l1467_025F:
	mov	ax,2670h
	push	ds
	push	ax

l1467_0264:
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,2675h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C79Eh]
	mov	[bp-32h],al
	cmp	al,0FFh
	jz	02B4h

l1467_0291:
	mov	al,11h
	mul	byte ptr [bp-32h]
	mov	bx,ax
	mov	al,es:[bx+0C614h]
	cbw
	mov	bx,ax
	shl	bx,1h
	shl	bx,1h
	mov	es,[552Eh]
	push	word ptr es:[bx+1CCh]
	push	word ptr es:[bx+1CAh]
	jmp	02B9h

l1467_02B4:
	mov	ax,2682h
	push	ds
	push	ax

l1467_02B9:
	call	far 1E56h:03F5h
	add	sp,4h
	mov	es,[5534h]
	mov	word ptr es:[37FEh],0Fh
	call	far 1CD3h:181Eh

l1467_02D1:
	inc	word ptr [bp-26h]

l1467_02D4:
	cmp	word ptr [bp-26h],4h
	jl	02DDh

l1467_02DA:
	jmp	036Dh

l1467_02DD:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	cmp	byte ptr es:[bx+0C724h],0FFh
	jz	02D1h

l1467_02F1:
	mov	es,[5532h]
	cmp	word ptr es:[4FBAh],0h
	jnz	0308h

l1467_02FD:
	mov	es,[5534h]
	mov	word ptr es:[37FEh],1h

l1467_0308:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	lea	ax,[bx+0C724h]
	mov	dx,3092h
	push	dx
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,2663h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C79Dh]
	mov	[bp-32h],al
	cmp	al,0FFh
	jnz	0349h

l1467_0346:
	jmp	025Fh

l1467_0349:
	mov	al,11h
	mul	byte ptr [bp-32h]
	mov	bx,ax
	mov	al,es:[bx+0C614h]
	cbw
	mov	bx,ax
	shl	bx,1h
	shl	bx,1h
	mov	es,[552Eh]
	push	word ptr es:[bx+1CCh]
	push	word ptr es:[bx+1CAh]
	jmp	0264h

l1467_036D:
	mov	ax,17h
	push	ax
	call	far 1E56h:0B5Eh
	add	sp,2h
	mov	[bp-1Ah],ax
	mov	ax,[bp-20h]
	cmp	[bp-1Ah],ax
	jz	0387h

l1467_0384:
	jmp	0509h

l1467_0387:
	mov	word ptr [bp-1Eh],1h
	mov	word ptr [bp-26h],0h

l1467_0391:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	si,ax
	mov	es,[5528h]
	cmp	byte ptr es:[si+0C724h],0FFh
	jz	03B2h

l1467_03A5:
	cmp	byte ptr es:[si+0C79Dh],0FFh
	jnz	03B2h

l1467_03AD:
	mov	word ptr [bp-1Eh],0h

l1467_03B2:
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],4h
	jl	0391h

l1467_03BB:
	cmp	word ptr [bp-1Eh],0h
	jz	03C4h

l1467_03C1:
	jmp	0878h

l1467_03C4:
	mov	ax,4h
	push	ax
	call	far 1E56h:0281h
	add	sp,2h
	call	far 1E56h:0388h
	mov	ax,2687h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	sub	ax,ax
	mov	[bp-28h],ax
	mov	[bp-26h],ax
	jmp	040Eh

l1467_03EC:
	mov	ax,11h
	imul	word ptr [bp-26h]
	mov	si,ax
	mov	es,[5528h]
	cmp	byte ptr es:[si+0C614h],0FFh
	jz	040Bh

l1467_0400:
	cmp	byte ptr es:[si+0C61Ch],0h
	jz	040Bh

l1467_0408:
	inc	word ptr [bp-28h]

l1467_040B:
	inc	word ptr [bp-26h]

l1467_040E:
	cmp	word ptr [bp-26h],8h
	jl	03ECh

l1467_0414:
	sub	ax,ax
	mov	[bp-30h],ax
	mov	[bp-26h],ax
	jmp	0438h

l1467_041E:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	cmp	byte ptr es:[bx+0C724h],0FFh
	jz	0435h

l1467_0432:
	inc	word ptr [bp-30h]

l1467_0435:
	inc	word ptr [bp-26h]

l1467_0438:
	cmp	word ptr [bp-26h],4h
	jl	041Eh

l1467_043E:
	mov	ax,[bp-30h]
	cmp	[bp-28h],ax
	jl	0449h

l1467_0446:
	jmp	04F4h

l1467_0449:
	call	far 1F3Dh:086Ah
	call	far 1F3Dh:0259h
	call	far 1E56h:0388h
	mov	ax,26AFh
	push	ds
	push	ax
	call	far 1CD3h:17EAh
	add	sp,4h
	call	far 1F3Dh:0259h
	call	far 1E56h:0388h
	mov	ax,26EDh
	push	ds
	push	ax
	call	far 1CD3h:17EAh
	add	sp,4h
	sub	ax,ax
	push	ax
	call	far 0800h:1A13h
	add	sp,2h
	or	ax,ax
	jnz	048Eh

l1467_048B:
	jmp	0873h

l1467_048E:
	mov	word ptr [bp-26h],0h

l1467_0493:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	si,ax
	mov	es,[5528h]
	cmp	byte ptr es:[si+0C724h],0FFh
	jz	04E8h

l1467_04A7:
	cmp	byte ptr es:[si+0C79Dh],0FFh
	jnz	04E8h

l1467_04AF:
	mov	byte ptr es:[si+0C724h],0FFh
	mov	word ptr [bp-30h],0h

l1467_04BA:
	mov	ax,11h
	imul	word ptr [bp-30h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C620h]
	cbw
	cmp	ax,[bp-26h]
	jnz	04DFh

l1467_04D1:
	mov	ax,11h
	imul	word ptr [bp-30h]
	mov	bx,ax
	mov	byte ptr es:[bx+0C620h],8h

l1467_04DF:
	inc	word ptr [bp-30h]
	cmp	word ptr [bp-30h],8h
	jl	04BAh

l1467_04E8:
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],4h
	jl	0493h

l1467_04F1:
	jmp	0873h

l1467_04F4:
	mov	ax,271Eh
	push	ds
	push	ax
	call	far 1CD3h:17EAh
	add	sp,4h
	call	far 1F3Dh:0259h
	jmp	0873h

l1467_0509:
	mov	ax,4h
	push	ax
	call	far 1E56h:0281h
	add	sp,2h
	call	far 1E56h:0388h
	mov	es,[5534h]
	mov	word ptr es:[37FEh],0Fh
	mov	si,[bp-1Ah]
	shl	si,1h
	mov	ax,11h
	imul	word ptr [bp+si-18h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C614h]
	cbw
	mov	bx,ax
	shl	bx,1h
	shl	bx,1h
	mov	es,[552Eh]
	push	word ptr es:[bx+1CCh]
	push	word ptr es:[bx+1CAh]
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,2736h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	si,[bp-1Ah]
	shl	si,1h
	mov	ax,11h
	imul	word ptr [bp+si-18h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C61Ch]
	cbw
	mov	[bp-2Eh],ax
	or	ax,ax
	jnz	0595h

l1467_0583:
	call	far 0800h:28A2h
	mov	ax,273Bh
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h

l1467_0595:
	mov	ax,2740h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,3h
	push	ax
	call	far 1E56h:0281h
	add	sp,2h
	call	far 1E56h:0388h
	mov	ax,275Bh
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	es,[552Ah]
	mov	ax,es:[374Eh]
	mov	es,[552Ch]
	mov	es:[00C2h],ax
	mov	word ptr [bp-26h],0h

l1467_05D5:
	mov	si,[bp-26h]
	shl	si,1h
	mov	word ptr [bp+si-8h],0FFFFh
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],4h
	jl	05D5h

l1467_05E8:
	sub	ax,ax
	mov	[bp-24h],ax
	mov	[bp-26h],ax
	jmp	065Bh

l1467_05F2:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	mov	es,[5528h]
	cmp	byte ptr es:[bx+0C724h],0FFh
	jz	0658h

l1467_0606:
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	lea	ax,[bx+0C724h]
	mov	dx,3092h
	push	dx
	push	ax
	mov	ax,12h
	mov	dx,3092h
	push	dx
	push	ax
	call	far 207Fh:3B68h
	add	sp,8h
	mov	es,[5530h]
	mov	byte ptr es:[001Dh],0h
	mov	ax,7Dh
	imul	word ptr [bp-26h]
	mov	bx,ax
	lea	ax,[bx+0C724h]
	mov	dx,3092h
	push	dx
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	si,[bp-24h]
	inc	word ptr [bp-24h]
	shl	si,1h
	mov	ax,[bp-26h]
	mov	[bp+si-8h],ax

l1467_0658:
	inc	word ptr [bp-26h]

l1467_065B:
	cmp	word ptr [bp-26h],4h
	jl	05F2h

l1467_0661:
	mov	ax,2775h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	es,[552Ch]
	mov	ax,[bp-24h]
	inc	ax
	mov	es:[00C6h],ax
	mov	word ptr [bp-2Ah],0h

l1467_067F:
	mov	ax,7Dh
	imul	word ptr [bp-2Ah]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C79Dh]
	sub	ah,ah
	mov	si,[bp-1Ah]
	shl	si,1h
	cmp	ax,[bp+si-18h]
	jnz	06AAh

l1467_069C:
	mov	ax,7Dh
	imul	word ptr [bp-2Ah]
	mov	bx,ax
	mov	byte ptr es:[bx+0C79Dh],0FFh

l1467_06AA:
	mov	ax,7Dh
	imul	word ptr [bp-2Ah]
	mov	bx,ax
	mov	al,es:[bx+0C79Eh]
	sub	ah,ah
	mov	si,[bp-1Ah]
	shl	si,1h
	cmp	ax,[bp+si-18h]
	jnz	06D1h

l1467_06C3:
	mov	ax,7Dh
	imul	word ptr [bp-2Ah]
	mov	bx,ax
	mov	byte ptr es:[bx+0C79Eh],0FFh

l1467_06D1:
	inc	word ptr [bp-2Ah]
	cmp	word ptr [bp-2Ah],4h
	jl	067Fh

l1467_06DA:
	mov	ax,3h
	push	ax
	call	far 1E56h:0B5Eh
	add	sp,2h
	mov	[bp-26h],ax
	mov	ax,[bp-24h]
	cmp	[bp-26h],ax
	jnz	070Bh

l1467_06F1:
	mov	si,[bp-1Ah]
	shl	si,1h
	mov	ax,11h
	imul	word ptr [bp+si-18h]
	mov	bx,ax
	mov	es,[5528h]
	mov	byte ptr es:[bx+0C620h],8h
	jmp	0873h

l1467_070B:
	mov	word ptr [bp-1Ch],7Ah
	cmp	word ptr [bp-2Eh],0h
	jz	0756h

l1467_0716:
	mov	si,[bp-26h]
	shl	si,1h
	mov	ax,7Dh
	imul	word ptr [bp+si-8h]
	mov	bx,ax
	mov	es,[5528h]
	cmp	byte ptr es:[bx+0C79Dh],0FFh
	jz	0751h

l1467_072F:
	call	far 1E56h:0388h
	mov	ax,277Ah
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,1h
	push	ax
	call	far 0800h:1A13h
	add	sp,2h
	or	ax,ax
	jz	0756h

l1467_0751:
	mov	word ptr [bp-1Ch],79h

l1467_0756:
	mov	di,[bp-26h]
	shl	di,1h
	mov	ax,7Dh
	imul	word ptr [bp+di-8h]
	mov	si,ax
	mov	bx,[bp-1Ch]
	add	bx,si
	mov	es,[5528h]
	mov	al,es:[bx+0C724h]
	sub	ah,ah
	mov	[bp-2Ch],ax
	cmp	ax,0FFh
	jz	07BFh

l1467_077B:
	cmp	word ptr [bp-1Ch],79h
	jnz	07B1h

l1467_0781:
	cmp	byte ptr es:[si+0C79Eh],0FFh
	jz	0798h

l1467_0789:
	mov	al,11h
	mul	byte ptr es:[si+0C79Eh]
	mov	bx,ax
	mov	byte ptr es:[bx+0C620h],8h

l1467_0798:
	mov	al,[bp-2Ch]
	mov	si,[bp-26h]
	shl	si,1h
	mov	cx,ax
	mov	ax,7Dh
	imul	word ptr [bp+si-8h]
	mov	bx,ax
	mov	es:[bx+0C79Eh],cl
	jmp	07BFh

l1467_07B1:
	mov	ax,11h
	imul	word ptr [bp-2Ch]
	mov	bx,ax
	mov	byte ptr es:[bx+0C620h],8h

l1467_07BF:
	cmp	byte ptr es:[0D333h],0h
	jz	0838h

l1467_07C7:
	mov	al,es:[0D331h]
	cbw
	mov	si,[bp-1Ah]
	shl	si,1h
	cmp	[bp+si-18h],ax
	jnz	0838h

l1467_07D6:
	call	far 1CD3h:17C6h
	mov	ax,279Dh
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	si,[bp-1Ah]
	shl	si,1h
	mov	ax,11h
	imul	word ptr [bp+si-18h]
	mov	bx,ax
	mov	es,[5528h]
	mov	al,es:[bx+0C614h]
	cbw
	mov	bx,ax
	shl	bx,1h
	shl	bx,1h
	mov	es,[552Eh]
	push	word ptr es:[bx+1CCh]
	push	word ptr es:[bx+1CAh]
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,27AEh
	push	ds
	push	ax
	call	far 1CD3h:17EAh
	add	sp,4h
	mov	es,[5536h]
	mov	word ptr es:[374Ah],1h
	call	far 1F3Dh:0259h

l1467_0838:
	mov	bx,[bp-1Ah]
	shl	bx,1h
	add	bx,bp
	mov	si,[bx-18h]
	mov	bx,[bp-26h]
	shl	bx,1h
	add	bx,bp
	mov	di,[bx-8h]
	mov	ax,si
	mov	cx,ax
	mov	ax,7Dh
	imul	di
	mov	bx,ax
	add	bx,[bp-1Ch]
	mov	es,[5528h]
	mov	es:[bx+0C724h],cl
	mov	ax,di
	mov	cx,ax
	mov	ax,11h
	imul	si
	mov	bx,ax
	mov	es:[bx+0C620h],cl

l1467_0873:
	call	far 1E56h:0388h

l1467_0878:
	cmp	word ptr [bp-1Eh],0h
	jnz	0881h

l1467_087E:
	jmp	00DAh

l1467_0881:
	mov	word ptr [bp-26h],0h

l1467_0886:
	mov	bx,[bp-26h]
	mov	es,[5528h]
	mov	byte ptr es:[bx+0D452h],0FFh
	inc	word ptr [bp-26h]
	cmp	word ptr [bp-26h],4h
	jl	0886h

l1467_089C:
	mov	byte ptr es:[0D324h],0h
	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	retf
1467:08A8                         55 8B EC B8 18 00 9A DC         U.......
1467:08B0 2F 7F 20 57 56 8E 06 38 55 26 83 3E 94 45 00 75 /. WV..8U&.>.E.u
1467:08C0 21 B8 01 00 50 9A CC 28 00 08 83 C4 02 B8 A0 00 !...P..(........
1467:08D0 BA 92 30 52 50 B8 E6 27 1E 50 9A 3B 06 3D 1F 83 ..0RP..'.P.;.=..
1467:08E0 C4 08 8E 06 3A 55 26 C7 06 BC 4F 00 00 B8 00 00 ....:U&...O.....
1467:08F0 BA 56 30 52 50 9A D1 00 7F 20 83 C4 04 B8 4B 24 .V0RP.... ....K$
1467:0900 BA 6C 24 52 50 B8 A0 00 BA 92 30 52 50 9A 9D 04 .l$RP.....0RP...
1467:0910 3D 1F 83 C4 08 8E 06 32 55 26 83 3E BA 4F 02 75 =......2U&.>.O.u
1467:0920 14 B8 00 A8 50 B8 4B 24 BA 6C 24 52 50 9A 60 02 ....P.K$.l$RP.`.
1467:0930 7F 20 83 C4 06 B8 06 00 50 9A 81 02 56 1E 83 C4 . ......P...V...
1467:0940 02 9A 88 03 56 1E 2B C0 50 9A 04 00 56 1E 83 C4 ....V.+.P...V...
1467:0950 02 B8 F2 27 1E 50 9A EA 17 D3 1C 83 C4 04 9A 59 ...'.P.........Y
1467:0960 02 3D 1F C7 46 EE 00 00 B8 FF FF 89 46 F8 89 46 .=..F.......F..F
1467:0970 F6 89 46 F4 C7 46 F2 00 00 E9 FC 00 8B 76 EC D1 ..F..F.......v..
1467:0980 E6 8B 46 E8 39 42 F4 75 05 C7 46 EA 01 00 FF 46 ..F.9B.u..F....F
1467:0990 EC 8B 46 F2 39 46 EC 7C E3 83 7E EA 00 74 03 E9 ..F.9F.|..~..t..
1467:09A0 7E 01 8B F8 D1 E7 8B 46 E8 89 43 F4 8B 76 E8 D1 ~......F..C..v..
1467:09B0 E6 8B 84 C4 28 2D 03 00 89 46 FC 8B 84 D8 28 2D ....(-...F....(-
1467:09C0 03 00 89 46 FA B8 0F 00 50 FF 76 FA 8B 46 FC 05 ...F....P.v..F..
1467:09D0 08 00 50 FF 76 FA FF 76 FC 9A 1C 03 3D 1F 83 C4 ..P.v..v....=...
1467:09E0 0A 8B 76 FA 83 C6 08 B8 0F 00 50 56 8B 46 FC 05 ..v.......PV.F..
1467:09F0 08 00 50 56 FF 76 FC 9A 1C 03 3D 1F 83 C4 0A B8 ..PV.v....=.....
1467:0A00 0F 00 50 8B 46 FA 05 08 00 50 FF 76 FC FF 76 FA ..P.F....P.v..v.
1467:0A10 FF 76 FC 9A 1C 03 3D 1F 83 C4 0A 8B 76 FC 83 C6 .v....=.....v...
1467:0A20 08 B8 0F 00 50 8B 46 FA 05 08 00 50 56 FF 76 FA ....P.F....PV.v.
1467:0A30 56 9A 1C 03 3D 1F 83 C4 0A B8 0F 00 50 8B 46 FA V...=.......P.F.
1467:0A40 05 06 00 50 FF 76 FC 8B 46 FA 40 40 50 B8 80 00 ...P.v..F.@@P...
1467:0A50 50 9A FB 01 3D 1F 83 C4 0A B8 25 00 50 9A 5E 0B P...=.....%.P.^.
1467:0A60 56 1E 83 C4 02 8B 5E E8 D1 E3 39 87 EC 28 74 05 V.....^...9..(t.
1467:0A70 C7 46 EE 01 00 FF 46 F2 83 7E F2 03 7C 03 E9 BA .F....F..~..|...
1467:0A80 00 B8 C8 00 50 B8 28 00 50 2B C0 50 50 B8 4B 24 ....P.(.P+.PP.K$
1467:0A90 BA 6C 24 52 50 9A 86 00 3D 1F 83 C4 0C 8E 06 32 .l$RP...=......2
1467:0AA0 55 26 83 3E BA 4F 01 1B C0 F7 D8 05 09 00 50 B8 U&.>.O........P.
1467:0AB0 C0 00 50 B8 80 00 50 2B C0 50 B8 80 00 50 9A 1C ..P...P+.P...P..
1467:0AC0 03 3D 1F 83 C4 0A B8 08 00 50 9A 81 02 56 1E 83 .=.......P...V..
1467:0AD0 C4 02 9A 88 03 56 1E B8 6C 28 1E 50 9A F5 03 56 .....V..l(.P...V
1467:0AE0 1E 83 C4 04 C7 46 F0 00 00 2B C0 50 B8 0F 00 50 .....F...+.P...P
1467:0AF0 8B 46 F0 05 04 00 50 2B C0 50 8B 5E F0 D1 E3 D1 .F....P+.P.^....
1467:0B00 E3 8E 06 3C 55 26 FF B7 E4 4D 26 FF B7 E2 4D 9A ...<U&...M&...M.
1467:0B10 D5 00 3D 1F 83 C4 0C FF 46 F0 83 7E F0 14 7C C9 ..=.....F..~..|.
1467:0B20 C7 46 EA 00 00 9A C0 0B 7F 20 99 B9 0A 00 F7 F9 .F....... ......
1467:0B30 89 56 E8 C7 46 EC 00 00 E9 56 FE 9A BE 1F 7F 20 .V..F....V..... 
1467:0B40 B8 01 00 50 9A AC 4C 00 08 83 C4 02 9A 88 03 56 ...P..L........V
1467:0B50 1E 83 7E EE 00 74 38 B8 06 00 50 9A 81 02 56 1E ..~..t8...P...V.
1467:0B60 83 C4 02 9A 88 03 56 1E 2B C0 50 9A 04 00 56 1E ......V.+.P...V.
1467:0B70 83 C4 02 B8 86 28 1E 50 9A EA 17 D3 1C 83 C4 04 .....(.P........
1467:0B80 8E 06 28 55 26 C6 06 20 D3 FF 9A 59 02 3D 1F 8B ..(U&.. ...Y.=..
1467:0B90 46 EE 5E 5F 8B E5 5D CB                         F.^_..].       

;; fn1467_0B98: 1467:0B98
;;   Called from:
;;     0800:3800 (in fn0800_378D)
;;     0FDC:1A84 (in fn0FDC_1A26)
;;     11B8:0020 (in fn11B8_0002)
;;     11B8:0827 (in fn11B8_080A)
;;     11B8:1781 (in fn11B8_1762)
fn1467_0B98 proc
	push	bp
	mov	bp,sp
	mov	ax,4h
	call	far 207Fh:2FDCh
	mov	word ptr [bp-4h],0h

l1467_0BA8:
	mov	bx,[bp-4h]
	mov	es,[553Eh]
	mov	byte ptr es:[bx+0D558h],0h
	inc	word ptr [bp-4h]
	cmp	word ptr [bp-4h],4h
	jl	0BA8h

l1467_0BBE:
	mov	es,[5540h]
	sub	ax,ax
	mov	es:[0068h],ax
	mov	[bp-2h],ax
	mov	es,[5528h]
	cmp	byte ptr es:[0D324h],0h
	jz	0C08h

l1467_0BD7:
	mov	[bp-4h],ax
	jmp	0BDFh

l1467_0BDC:
	inc	word ptr [bp-4h]

l1467_0BDF:
	cmp	word ptr [bp-4h],4h
	jge	0C3Ch

l1467_0BE5:
	mov	bx,[bp-4h]
	mov	es,[5528h]
	cmp	byte ptr es:[bx+0D452h],0FFh
	jz	0BDCh

l1467_0BF4:
	mov	al,[bp-4h]
	mov	bx,[bp-2h]
	inc	word ptr [bp-2h]
	mov	es,[553Eh]
	mov	es:[bx+0D558h],al
	jmp	0BDCh

l1467_0C08:
	mov	word ptr [bp-4h],0h

l1467_0C0D:
	mov	ax,7Dh
	imul	word ptr [bp-4h]
	mov	bx,ax
	mov	es,[5528h]
	cmp	byte ptr es:[bx+0C724h],0FFh
	jz	0C33h

l1467_0C21:
	mov	al,[bp-4h]
	mov	bx,[bp-2h]
	inc	word ptr [bp-2h]
	mov	es,[553Eh]
	mov	es:[bx+0D558h],al

l1467_0C33:
	inc	word ptr [bp-4h]
	cmp	word ptr [bp-4h],4h
	jl	0C0Dh

l1467_0C3C:
	mov	es,[553Eh]
	mov	al,es:[0D558h]
	cbw
	mov	es,[5540h]
	mov	es:[0068h],ax
	mov	es,[552Ah]
	mov	ax,es:[374Eh]
	mov	es,[552Ch]
	mov	es:[0202h],ax
	mov	word ptr [bp-4h],0h
	jmp	0C8Bh

l1467_0C64:
	mov	es,[5540h]
	mov	ax,7Dh
	imul	word ptr es:[0068h]
	mov	bx,ax
	lea	ax,[bx+0C724h]
	mov	dx,3092h

l1467_0C79:
	push	dx
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	call	far 1CD3h:181Eh
	inc	word ptr [bp-4h]

l1467_0C8B:
	mov	ax,[bp-2h]
	cmp	[bp-4h],ax
	jl	0C96h

l1467_0C93:
	jmp	0D3Dh

l1467_0C96:
	mov	bx,[bp-4h]
	mov	es,[553Eh]
	mov	al,es:[bx+0D558h]
	cbw
	mov	es,[5540h]
	mov	es:[0068h],ax
	mov	ax,7Dh
	imul	word ptr es:[0068h]
	mov	bx,ax
	mov	es,[5528h]
	mov	bl,es:[bx+0C79Dh]
	sub	bh,bh
	shl	bx,1h
	shl	bx,1h
	mov	es,[552Eh]
	push	word ptr es:[bx+1CCh]
	push	word ptr es:[bx+1CAh]
	call	far 1E56h:03F5h
	add	sp,4h
	mov	ax,2900h
	push	ds
	push	ax
	call	far 1E56h:03F5h
	add	sp,4h
	mov	es,[5528h]
	cmp	byte ptr es:[0D324h],0h
	jnz	0CF6h

l1467_0CF3:
	jmp	0C64h

l1467_0CF6:
	mov	es,[5540h]
	mov	bx,es:[0068h]
	mov	es,[5528h]
	mov	al,es:[bx+0D452h]
	mov	es,[5530h]
	mov	es:[0012h],al
	mov	es,[5540h]
	mov	ax,7Dh
	imul	bx
	mov	bx,ax
	lea	ax,[bx+0C725h]
	mov	dx,3092h
	push	dx
	push	ax
	mov	ax,13h
	mov	dx,3092h
	push	dx
	push	ax
	call	far 207Fh:3B68h
	add	sp,8h
	mov	ax,12h
	mov	dx,3092h
	jmp	0C79h

l1467_0D3D:
	mov	es,[552Ch]
	mov	ax,[bp-2h]
	mov	es:[0206h],ax
	mov	word ptr es:[0208h],0h
	mov	ax,17h
	push	ax
	call	far 1E56h:0B5Eh
	mov	bx,ax
	mov	es,[553Eh]
	mov	al,es:[bx+0D558h]
	cbw
	mov	es,[5540h]
	mov	es:[0068h],ax
	mov	es,[552Ch]
	mov	word ptr es:[0202h],1h
	mov	ax,[bp-2h]
	mov	sp,bp
	pop	bp
	retf

;; fn1467_0D7E: 1467:0D7E
;;   Called from:
;;     1AE8:0C3C (in fn1AE8_000C)
fn1467_0D7E proc
	push	bp
	mov	bp,sp
	mov	ax,4h
	call	far 207Fh:2FDCh
	mov	word ptr [bp-2h],0h
	jmp	0DB3h

l1467_0D90:
	inc	word ptr [bp-4h]

l1467_0D93:
	cmp	word ptr [bp-4h],18h
	jge	0DB0h

l1467_0D99:
	mov	ax,18h
	imul	word ptr [bp-2h]
	mov	bx,ax
	add	bx,[bp-4h]
	mov	es,[5542h]
	mov	byte ptr es:[bx+40B4h],0h
	jmp	0D90h

l1467_0DB0:
	inc	word ptr [bp-2h]

l1467_0DB3:
	cmp	word ptr [bp-2h],18h
	jge	0DC0h

l1467_0DB9:
	mov	word ptr [bp-4h],0h
	jmp	0D93h
