.globl _occam_start
.type _occam_start, @function
_occam_start:
# # x64 FPU init
# # x64 SSE2 FPU reset
# # x64 SSE2 FPU init
	leaq	.L15(%rip), %r11
	movq	%r11, (%r14)
	jmp	O_main

.align 8
.L0:
	# sourcefile "/tmp/nested.occ"
# # x64 SSE2 FPU reset
	subq	$112, %r14
	movq	120(%r14), %rax
	movq	104(%rax), %rax
	addq	$1, %rax
	jo	.L3
	movq	120(%r14), %rcx
	movq	%rax, 104(%rcx)
	movq	120(%r14), %rax
	movq	128(%rax), %rax
	movq	120(%r14), %rdi
	movq	104(%rdi), %rdi
	cmpq	$256, %rdi
	jb	.L16
	movq	%r15, %rsi
	movq	%r14, %rdx
	callq	*920(%r15)
.L16:
# # x64 SSE2 FPU reset
	movq	%rax, 8(%r15)
	movq	%r15, %rsi
	movq	%r14, %rdx
	callq	*184(%r15)
	addq	$112, %r14
# # x64 SSE2 FPU reset
	addq	$32, %r14
	jmp	*-32(%r14)
.globl O_main
.type O_main, @function
O_main:
# # x64 SSE2 FPU reset
	subq	$112, %r14
	movq	$65, 104(%r14)
	movq	%r14, %rax
	movq	%rax, -24(%r14)
	subq	$32, %r14
	leaq	0f(%rip), %r11
	movq	%r11, (%r14)
	jmp	.L0
0:
	movq	%r14, %rax
	movq	%rax, -24(%r14)
	subq	$32, %r14
	leaq	0f(%rip), %r11
	movq	%r11, (%r14)
	jmp	.L0
0:
	movq	%r14, %rax
	movq	%rax, -24(%r14)
	subq	$32, %r14
	leaq	0f(%rip), %r11
	movq	%r11, (%r14)
	jmp	.L0
0:
	movq	128(%r14), %rax
	movq	$10, %rdi
# # x64 SSE2 FPU reset
	movq	%rax, 8(%r15)
	movq	%r15, %rsi
	movq	%r14, %rdx
	callq	*184(%r15)
	addq	$112, %r14
# # x64 SSE2 FPU reset
	addq	$32, %r14
	jmp	*-32(%r14)
.size O_main, . - O_main

.align 8
.L15:
	subq	$32, %r14
	movb	$255, (%r14)
	movq	%r14, %rax
	movq	16(%r14), %rdi
# .MAGIC SCREEN
	movq	%rax, 8(%r15)
	movq	%r15, %rsi
	movq	%r14, %rdx
	callq	*176(%r15)
	addq	$16, %r14
	movq	%r15, %rsi
	movq	%r14, %rdx
	callq	*832(%r15)

.align 4

.align 4
.L3:
	movq	%r15, %rsi
	movq	%r14, %rdx
	callq	*904(%r15)
.data
.globl _wsbytes
_wsbytes: .quad 128
.globl _wsadjust
_wsadjust: .quad 256
.globl _vsbytes
_vsbytes: .quad 0
.globl _msbytes
_msbytes: .quad 0
.globl _occam_errormode
.type _occam_errormode, @object
_occam_errormode:
	.byte	0x00, 0x00, 0x00, 0x00 
.globl _occam_tlp_iface
.type _occam_tlp_iface, @object
_occam_tlp_iface:
	.byte	0x07, 0x00, 0x00, 0x00 

.align 4
.L11:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc1 
.L12:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x41 
.L13:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc3 
.L14:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x43 
.section .note.GNU-stack,"",%progbits
