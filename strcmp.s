	global __strcmp_asm

	section .text
__strcmp_asm:
	mov rcx, rdx
	shr rcx, 0x3 		; rcx = len / 8

	test rcx, rcx 		; if !rcx - process_back
	je .process_back

	repe cmpsq 		; compare strings
	je .process_back

	xor rax, rax 		; if not equal - return 0
	ret

.process_back:
	mov rcx, rdx      
	and rcx, 0b111 		; rcx = len % 8

	repe cmpsb 		; compare strings
	je .end

	xor rax, rax
	ret
.end:
	mov rax, 1
	ret

	
