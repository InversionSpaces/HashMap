	global __xor_hash

	section .text
__xor_hash:
	xor rax, rax 		; rax = 0

	mov rcx, rsi 		; rcx = len
	shr rcx, 0x3 		; rcx = len / 8
	
	test rcx, rcx
	je .process_back 	; if rcx == 0 - skip

.process_front_loop:
	xor rax, [rdi] 			; rax ^= [rdi] (64 bits)
	add rdi, 0x8 			; rdi += 8
	
	loop .process_front_loop

.process_back:
	mov rcx, rsi 			; rcx = len
	and rcx, 0b111 			; rcx = len % 8

	test rcx, rcx
	je .end

	add rdi, rcx
	dec rdi

	xor rdx, rdx

.process_back_loop:
	shl rdx, 0x8
	or dl, [rdi]
	dec rdi

	loop .process_back_loop
	
	xor rax, rdx
.end:
	ret
