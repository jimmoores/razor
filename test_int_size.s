// aarch64 assembly output - FIXED VERSION
.text
.global __occam_start
__occam_start:
	// ARM64 FPU init (default IEEE 754)
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_pause
	// aarch64 FPU reset
	// ARM64 FPU init (default IEEE 754)
	adr	x16, L13
	str	x16, [x28]
	b	_O_main

.align 3
.global _O_main
_O_main:
	// aarch64 FPU reset
	sub	x28, x28, #16
	mov	x16, #1
	str	x16, [x28, #0]
	add	x28, x28, #16
	// aarch64 FPU reset
	add	x28, x28, #32
	ldr	x9, [x28, #-32]
	br	x9

.align 3
L13:
	sub	x28, x28, #32
	mov	w16, #255
	strb	w16, [x28, #0]
	mov	x0, x28
	ldr	x1, [x28, #16]
	// .MAGIC SCREEN
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_out8
	add	x28, x28, #16
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_shutdown

.align 2

.align 2
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_BNSeterr
.data
.globl __wsbytes
__wsbytes: .quad 48
.globl __wsadjust
__wsadjust: .quad 256
.globl __vsbytes
__vsbytes: .quad 0
.globl __msbytes
__msbytes: .quad 0
.text
.global __occam_errormode
__occam_errormode:
	.byte	0x00, 0x00, 0x00, 0x00 
.global __occam_tlp_iface
__occam_tlp_iface:
	.byte	0x07, 0x00, 0x00, 0x00 

.align 2
L9:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc1 
L10:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x41 
L11:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc3 
L12:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x43 
