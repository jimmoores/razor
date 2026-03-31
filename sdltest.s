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
	adr	x16, L235
	str	x16, [x28]
	b	_O_sdltest

.align 3
L11:
.global _O_sdltest_proper
_O_sdltest_proper:
	// aarch64 FPU reset
	sub	x28, x28, #96
	mov	x16, #0
	str	x16, [x28, #64]
	mov	x16, #0
	str	x16, [x28, #72]
	mov	x16, #0
	str	x16, [x28, #80]
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_ldtimer
	str	x0, [x28, #48]
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_ldtimer
	str	x0, [x28, #8]
	ldr	x0, [x28, #8]
	mov	w0, w0
	lsr	x0, x0, #2
	adds	x0, x0, #1
	b.vs	L223
	str	x0, [x28, #8]
	mov	x16, #1
	str	x16, [x28, #56]
L12:
	movz	x16, #0
	ldr	x17, [x28, #56]
	cmp	x17, x16
	b.eq	L13
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_pause
	add	x0, x28, #72
	ldr	x1, [x28, #128]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_mt_in
	ldr	x0, [x28, #72]
	movz	x16, #0
	cmp	x0, x16
	b.eq	L15
	mov	x1, x0
	ldr	x1, [x1, #0]
	str	x1, [x28, #64]
	mov	x1, x0
	ldr	x1, [x1, #8]
	str	x1, [x28, #80]
	ldr	x0, [x0, #16]
	str	x0, [x28, #88]
	b	L14
L15:
	mov	x16, #0
	str	x16, [x28, #80]
L14:
	mov	x16, #10
	str	x16, [x28, #40]
L16:
	ldr	x0, [x28, #8]
	ldr	x1, [x28, #104]
	str	x0, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_random
0:
	str	x19, [x28, #24]
	str	x20, [x28, #8]
	ldr	x0, [x28, #8]
	ldr	x1, [x28, #112]
	str	x0, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_random
0:
	str	x19, [x28, #16]
	str	x20, [x28, #8]
	ldr	x0, [x28, #8]
	str	x0, [x28, #-16]
	movz	x16, #65535
	movk	x16, #255, lsl #16
	str	x16, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_random
0:
	str	x19, [x28, #0]
	str	x20, [x28, #8]
	ldr	x0, [x28, #16]
	ldr	x1, [x28, #80]
	mov	w1, w1
	ldr	x0, [x28, #16]
	mov	w0, w0
	cmp	x1, x0
	b.hi	L236
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_BasicRangeError
L236:
	ldr	x1, [x28, #88]
	mul	x0, x1, x0
	cmp	x0, x0
	mov	w0, w0
	ldr	x1, [x28, #24]
	ldr	x2, [x28, #88]
	mov	w2, w2
	ldr	x1, [x28, #24]
	mov	w1, w1
	cmp	x2, x1
	b.hi	L237
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_BasicRangeError
L237:
	adds	x0, x0, x1
	b.vs	L223
	mov	w0, w0
	lsl	x0, x0, #2
	mov	w0, w0
	ldr	x1, [x28, #64]
	add	x1, x1, x0
	ldr	x0, [x28, #0]
	str	w0, [x1, #0]
	ldr	x17, [x28, #40]
	subs	x17, x17, #1
	str	x17, [x28, #40]
	b.ne	L16
	add	x0, x28, #72
	ldr	x1, [x28, #120]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_mt_out
	mov	x16, #0
	str	x16, [x28, #64]
	mov	x16, #0
	str	x16, [x28, #72]
	mov	x16, #0
	str	x16, [x28, #80]
	ldr	x17, [x28, #48]
	movz	x16, #40000
	add	x17, x17, x16
	str	x17, [x28, #48]
	ldr	x0, [x28, #48]
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_tin
	ldr	x0, [x28, #136]
	movz	x16, #0
	ldr	x17, [x0]
	cmp	x17, x16
	b.ne	L19
	b	L21
L24:
	adr	x16, L21
	adr	x17, L19
	sub	x0, x16, x17
	mov	x16, #1
	str	x16, [x25, #8]
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_ndiss
L22:
	ldr	x0, [x28, #136]
	adr	x16, L19
	adr	x17, L19
	sub	x1, x16, x17
	mov	x16, #1
	str	x16, [x25, #8]
	str	x0, [x25, #16]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_ndisc
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_altend
L19:
	mov	x0, x28
	ldr	x1, [x28, #136]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_in32
	movz	x16, #1
	ldr	x17, [x28, #0]
	cmp	w17, w16
	b.ne	L27
	b	L26
L27:
	b	L25
L26:
	mov	x16, #0
	str	x16, [x28, #56]
L25:
	b	L18
L21:
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_pause
L18:
	b	L12
L13:
	movz	x16, #0
	ldr	x17, [x28, #72]
	cmp	x17, x16
	b.eq	L28
	ldr	x0, [x28, #72]
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_mt_release
	mov	x16, #0
	str	x16, [x28, #64]
	mov	x16, #0
	str	x16, [x28, #72]
	mov	x16, #0
	str	x16, [x28, #80]
L28:
	add	x28, x28, #96
	// aarch64 FPU reset
	add	x28, x28, #32
	ldr	x9, [x28, #-32]
	br	x9

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3
L59:
.global _O_out_control_key
_O_out_control_key:
	// aarch64 FPU reset
	sub	x28, x28, #16
	mov	x0, #273
	ldr	x1, [x28, #24]
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.gt	L90
	ldr	x0, [x28, #24]
	mov	x1, #319
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.gt	L89
	ldr	x0, [x28, #24]
	sub	x0, x0, #273
	mov	w0, w0
	mov	w0, w0
	adr	x1, L238
	ldr	x0, [x1, x0, lsl #3]
	add	x1, x1, x0
	br	x1
L238:
	.quad	(L79 - L238)
	.quad	(L80 - L238)
	.quad	(L82 - L238)
	.quad	(L81 - L238)
	.quad	(L87 - L238)
	.quad	(L83 - L238)
	.quad	(L84 - L238)
	.quad	(L85 - L238)
	.quad	(L86 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L63 - L238)
	.quad	(L64 - L238)
	.quad	(L65 - L238)
	.quad	(L66 - L238)
	.quad	(L67 - L238)
	.quad	(L68 - L238)
	.quad	(L69 - L238)
	.quad	(L70 - L238)
	.quad	(L71 - L238)
	.quad	(L72 - L238)
	.quad	(L73 - L238)
	.quad	(L75 - L238)
	.quad	(L74 - L238)
	.quad	(L78 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L89 - L238)
	.quad	(L76 - L238)
	.quad	(L89 - L238)
	.quad	(L77 - L238)
L90:
	movz	x16, #0
	ldr	x17, [x28, #24]
	cmp	x17, x16
	b.eq	L61
	mov	x0, #8
	ldr	x1, [x28, #24]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L62
	movz	x16, #127
	ldr	x17, [x28, #24]
	cmp	w17, w16
	b.ne	L89
	b	L88
L89:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L30
	str	x0, [x28, #-8]
	mov	x16, #8
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #48]
	str	x0, [x28, #0]
	ldr	x0, [x28, #40]
	ldr	x1, [x28, #32]
	subs	x1, x1, #8
	b.vs	L223
	ldr	x2, [x28, #24]
	str	x0, [x28, #-8]
	str	x1, [x28, #-16]
	str	x2, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	b	L60
L61:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L58
	str	x0, [x28, #-8]
	mov	x16, #7
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L62:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L57
	str	x0, [x28, #-8]
	mov	x16, #9
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L88:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L31
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L79:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L40
	str	x0, [x28, #-8]
	mov	x16, #2
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L80:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L39
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L82:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L37
	str	x0, [x28, #-8]
	mov	x16, #5
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L81:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L38
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L87:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L32
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L83:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L36
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L84:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L35
	str	x0, [x28, #-8]
	mov	x16, #3
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L85:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L34
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L86:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L33
	str	x0, [x28, #-8]
	mov	x16, #8
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L63:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L56
	str	x0, [x28, #-8]
	mov	x16, #7
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L64:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L55
	str	x0, [x28, #-8]
	mov	x16, #8
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L65:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L54
	str	x0, [x28, #-8]
	mov	x16, #9
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L66:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L53
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L67:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L52
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L68:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L51
	str	x0, [x28, #-8]
	mov	x16, #5
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L69:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L50
	str	x0, [x28, #-8]
	mov	x16, #5
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L70:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L49
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L71:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L48
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L72:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L47
	str	x0, [x28, #-8]
	mov	x16, #5
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L73:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L46
	str	x0, [x28, #-8]
	mov	x16, #5
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L75:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L44
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L74:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L45
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L78:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L41
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L76:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L43
	str	x0, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L60
L77:
	ldr	x0, [x28, #40]
	str	x0, [x28, #0]
	ldr	x0, [x28, #32]
	adr	x1, L42
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
L60:
	add	x28, x28, #16
	// aarch64 FPU reset
	add	x28, x28, #32
	ldr	x9, [x28, #-32]
	br	x9

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3

.align 3
L140:
.global _O_event_handler
_O_event_handler:
	// aarch64 FPU reset
	sub	x28, x28, #96
	adr	x16, L123
	str	x16, [x28, #80]
	mov	x16, #1
	str	x16, [x28, #40]
L142:
	movz	x16, #0
	ldr	x17, [x28, #40]
	cmp	x17, x16
	b.eq	L143
	add	x0, x28, #16
	ldr	x1, [x28, #104]
	// aarch64 FPU reset
	str	x1, [x25, #8]
	str	x0, [x25, #16]
	mov	x0, #24
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_in
	add	x0, x28, #16
	adds	x0, x0, #4
	b.vs	L223
	ldr	w0, [x0, #0]
	str	x0, [x28, #8]
	mov	x1, #8
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.le	L156
	ldr	x0, [x28, #8]
	mov	x1, #64
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.le	L157
	mov	x0, #128
	ldr	x1, [x28, #8]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L148
	movz	x16, #256
	ldr	x17, [x28, #8]
	cmp	w17, w16
	b.ne	L155
	b	L145
L157:
	mov	x0, #16
	ldr	x1, [x28, #8]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L150
	mov	x0, #32
	ldr	x1, [x28, #8]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L146
	movz	x16, #64
	ldr	x17, [x28, #8]
	cmp	w17, w16
	b.ne	L155
	b	L147
L156:
	ldr	x0, [x28, #8]
	mov	x1, #0
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.le	L158
	mov	x0, #4
	ldr	x1, [x28, #8]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L151
	movz	x16, #8
	ldr	x17, [x28, #8]
	cmp	w17, w16
	b.ne	L155
	b	L149
L158:
	movz	x0, #65534
	movk	x0, #65535, lsl #16
	movk	x0, #65535, lsl #32
	movk	x0, #65535, lsl #48
	ldr	x1, [x28, #8]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L154
	movz	x0, #65535
	movk	x0, #65535, lsl #16
	movk	x0, #65535, lsl #32
	movk	x0, #65535, lsl #48
	ldr	x1, [x28, #8]
	sub	x0, x0, x1
	mov	w0, w0
	orr	x0, x0, x0
	tst	x0, x0
	b.eq	L153
	movz	x16, #0
	ldr	x17, [x28, #8]
	cmp	x17, x16
	b.eq	L152
L155:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L124
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #15
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	add	x1, x28, #16
	adds	x1, x1, #4
	b.vs	L223
	ldr	w1, [x1, #0]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, #10
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_outbyte
	b	L144
L154:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L126
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #21
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	add	x1, x28, #16
	adds	x1, x1, #8
	b.vs	L223
	ldr	w1, [x1, #0]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	L59
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L125
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #1
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L144
L153:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L127
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #19
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	add	x1, x28, #16
	adds	x1, x1, #8
	b.vs	L223
	ldr	w1, [x1, #0]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	L59
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L125
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #1
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L144
L152:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L128
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #15
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L144
L151:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L129
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #11
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #80]
	ldr	x1, [x28, #112]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_out32
	mov	x16, #0
	str	x16, [x28, #40]
	b	L144
L149:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L132
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #11
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	mov	x0, #65408
	add	x1, x28, #16
	adds	x1, x1, #8
	b.vs	L223
	ldr	w1, [x1, #0]
	ands	x0, x0, x1
	orr	x0, x0, x0
	tst	x0, x0
	b.ne	L168
	ldr	x0, [x28, #120]
	mov	x1, #255
	add	x2, x28, #16
	adds	x2, x2, #8
	b.vs	L223
	ldr	w2, [x2, #0]
	ands	x1, x1, x2
	mov	w1, w1
	movz	x16, #256
	cmp	w1, w16
	b.lo	L239
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_BasicRangeError
L239:
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_outbyte
	b	L167
L168:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L130
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #9
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #120]
	add	x1, x28, #16
	adds	x1, x1, #8
	b.vs	L223
	ldr	w1, [x1, #0]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_hex
0:
L167:
	ldr	x0, [x28, #120]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, #10
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_outbyte
	b	L144
L150:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L131
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #13
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	mov	x0, #65408
	add	x1, x28, #16
	adds	x1, x1, #8
	b.vs	L223
	ldr	w1, [x1, #0]
	ands	x0, x0, x1
	orr	x0, x0, x0
	tst	x0, x0
	b.ne	L172
	ldr	x0, [x28, #120]
	mov	x1, #255
	add	x2, x28, #16
	adds	x2, x2, #8
	b.vs	L223
	ldr	w2, [x2, #0]
	ands	x1, x1, x2
	mov	w1, w1
	movz	x16, #256
	cmp	w1, w16
	b.lo	L240
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_BasicRangeError
L240:
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_outbyte
	b	L171
L172:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L130
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #9
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #120]
	add	x1, x28, #16
	adds	x1, x1, #8
	b.vs	L223
	ldr	w1, [x1, #0]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_hex
0:
L171:
	ldr	x0, [x28, #120]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, #10
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_outbyte
	b	L144
L146:
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_XPOS
0:
	str	x19, [x28, #72]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_YPOS
0:
	str	x19, [x28, #64]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_BUTTON
0:
	str	x19, [x28, #56]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_KSTATE
0:
	str	x19, [x28, #48]
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L138
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #20
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #56]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L135
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #72]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L134
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #64]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L133
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #7
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #48]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L125
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #1
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L144
L147:
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_XPOS
0:
	str	x19, [x28, #72]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_YPOS
0:
	str	x19, [x28, #64]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_BUTTON
0:
	str	x19, [x28, #56]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_KSTATE
0:
	str	x19, [x28, #48]
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L137
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #20
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #56]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L135
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #72]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L134
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #64]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L133
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #7
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #48]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L125
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #1
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L144
L148:
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_XPOS
0:
	str	x19, [x28, #72]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_YPOS
0:
	str	x19, [x28, #64]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_BSTATE
0:
	str	x19, [x28, #56]
	add	x0, x28, #16
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLEVENT_KSTATE
0:
	str	x19, [x28, #48]
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L136
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #19
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #56]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L135
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #72]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L134
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #6
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #64]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L133
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #7
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	ldr	x0, [x28, #128]
	str	x0, [x28, #0]
	ldr	x0, [x28, #120]
	ldr	x1, [x28, #48]
	str	x0, [x28, #-8]
	mov	x16, #0
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_int
0:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L125
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #1
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	b	L144
L145:
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L139
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #13
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
L144:
	b	L142
L143:
	add	x28, x28, #96
	// aarch64 FPU reset
	add	x28, x28, #32
	ldr	x9, [x28, #-32]
	br	x9
L191:
.global _O_control_delta
_O_control_delta:
	// aarch64 FPU reset
	sub	x28, x28, #48
	mov	x16, #1
	str	x16, [x28, #32]
L192:
	movz	x16, #0
	ldr	x17, [x28, #32]
	cmp	x17, x16
	b.eq	L193
	add	x0, x28, #24
	ldr	x1, [x28, #56]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_in32
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_getpas
	str	x0, [x28, #16]
	mov	x16, #2
	str	x16, [x28, #8]
	adr	x16, L194
	str	x16, [x28]
	adr	x16, L197
	adr	x17, L196
	sub	x0, x16, x17
	movz	x16, #65504
	movk	x16, #65535, lsl #16
	movk	x16, #65535, lsl #32
	movk	x16, #65535, lsl #48
	add	x1, x28, x16
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_startp
L196:
	add	x0, x28, #24
	ldr	x1, [x28, #64]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_out32
	mov	x0, x28
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_endp
L197:
	add	x0, x28, #56
	ldr	x1, [x28, #104]
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_out32
	add	x0, x28, #32
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_endp
L194:
	movz	x16, #1
	ldr	x17, [x28, #24]
	cmp	w17, w16
	b.ne	L200
	b	L199
L200:
	b	L198
L199:
	mov	x16, #0
	str	x16, [x28, #32]
L198:
	b	L192
L193:
	add	x28, x28, #48
	// aarch64 FPU reset
	add	x28, x28, #32
	ldr	x9, [x28, #-32]
	br	x9

.align 3

.align 3

.align 3

.align 3

.align 3
.global _O_sdltest
_O_sdltest:
	// aarch64 FPU reset
	sub	x28, x28, #96
	ldr	x0, [x28, #120]
	str	x0, [x28, #0]
	adr	x0, L206
	mov	x16, #0
	str	x16, [x28, #-8]
	mov	x16, #38
	str	x16, [x28, #-16]
	str	x0, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_out_string
0:
	mov	x16, #4
	str	x16, [x28, #0]
	ldr	x0, [x28, #104]
	str	x0, [x28, #8]
	ldr	x0, [x28, #112]
	str	x0, [x28, #16]
	ldr	x0, [x28, #128]
	str	x0, [x28, #24]
	add	x0, x28, #48
	adr	x1, L205
	str	x0, [x28, #-8]
	mov	x16, #15
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_ask_int
0:
	mov	x16, #4
	str	x16, [x28, #0]
	ldr	x0, [x28, #104]
	str	x0, [x28, #8]
	ldr	x0, [x28, #112]
	str	x0, [x28, #16]
	ldr	x0, [x28, #128]
	str	x0, [x28, #24]
	add	x0, x28, #40
	adr	x1, L204
	str	x0, [x28, #-8]
	mov	x16, #16
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_ask_int
0:
	mov	x16, #2
	str	x16, [x28, #0]
	ldr	x0, [x28, #104]
	str	x0, [x28, #8]
	ldr	x0, [x28, #112]
	str	x0, [x28, #16]
	ldr	x0, [x28, #128]
	str	x0, [x28, #24]
	add	x0, x28, #32
	adr	x1, L203
	str	x0, [x28, #-8]
	mov	x16, #14
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_ask_int
0:
	ldr	x0, [x28, #32]
	mov	x1, #16
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.le	L213
	mov	x16, #16
	str	x16, [x28, #32]
	b	L214
L213:
	ldr	x0, [x28, #32]
	mov	x1, #0
	sxtw	x1, w1
	sxtw	x0, w0
	cmp	x0, x1
	b.gt	L214
	mov	x16, #1
	str	x16, [x28, #32]
L214:
	mov	x16, #0
	str	x16, [x28, #88]
	mov	x16, #0
	str	x16, [x28, #80]
	mov	x16, #0
	str	x16, [x28, #72]
	mov	x16, #0
	str	x16, [x28, #64]
	mov	x16, #0
	str	x16, [x28, #56]
	mov	x16, #0
	str	x16, [x28, #24]
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_X_getpas
	str	x0, [x28, #16]
	mov	x16, #4
	str	x16, [x28, #8]
	adr	x16, L215
	str	x16, [x28]
	adr	x16, L218
	adr	x17, L217
	sub	x0, x16, x17
	movz	x16, #63392
	movk	x16, #65535, lsl #16
	movk	x16, #65535, lsl #32
	movk	x16, #65535, lsl #48
	add	x1, x28, x16
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_startp
L217:
	adr	x16, L220
	adr	x17, L219
	sub	x0, x16, x17
	movz	x16, #63120
	movk	x16, #65535, lsl #16
	movk	x16, #65535, lsl #32
	movk	x16, #65535, lsl #48
	add	x1, x28, x16
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_startp
L219:
	adr	x16, L222
	adr	x17, L221
	sub	x0, x16, x17
	movz	x16, #62736
	movk	x16, #65535, lsl #16
	movk	x16, #65535, lsl #32
	movk	x16, #65535, lsl #48
	add	x1, x28, x16
	// aarch64 FPU reset
	str	x0, [x25, #8]
	mov	x0, x1
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_startp
L221:
	sub	x28, x28, #64
	ldr	x0, [x28, #104]
	str	x0, [x28, #0]
	ldr	x0, [x28, #96]
	str	x0, [x28, #8]
	add	x0, x28, #152
	str	x0, [x28, #16]
	add	x0, x28, #144
	str	x0, [x28, #24]
	add	x0, x28, #136
	str	x0, [x28, #32]
	add	x0, x28, #88
	str	x0, [x28, #40]
	ldr	x0, [x28, #192]
	str	x0, [x28, #48]
	ldr	x0, [x28, #112]
	adr	x1, L202
	str	x0, [x28, #-8]
	mov	x16, #4
	str	x16, [x28, #-16]
	str	x1, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	_O_SDLRaster
0:
	add	x0, x28, #64
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_endp
L218:
	add	x0, x28, #2224
	str	x0, [x28, #0]
	add	x0, x28, #2200
	str	x0, [x28, #8]
	ldr	x0, [x28, #2256]
	str	x0, [x28, #16]
	add	x0, x28, #2232
	ldr	x1, [x28, #2184]
	ldr	x2, [x28, #2192]
	str	x0, [x28, #-8]
	str	x1, [x28, #-16]
	str	x2, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	L11
0:
	add	x0, x28, #2144
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_endp
L220:
	ldr	x0, [x28, #2544]
	add	x0, x0, #4080
	str	x0, [x28, #0]
	ldr	x0, [x28, #2536]
	add	x1, x28, #2480
	add	x2, x28, #2488
	str	x0, [x28, #-8]
	str	x1, [x28, #-16]
	str	x2, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	L140
0:
	add	x0, x28, #2416
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_endp
L222:
	add	x0, x28, #2856
	add	x1, x28, #2824
	add	x2, x28, #2864
	str	x0, [x28, #-8]
	str	x1, [x28, #-16]
	str	x2, [x28, #-24]
	sub	x28, x28, #32
	adr	x16, 0f
	str	x16, [x28]
	b	L191
0:
	add	x0, x28, #2800
	// aarch64 FPU reset
	mov	x0, x0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_endp
L215:
	add	x28, x28, #96
	// aarch64 FPU reset
	add	x28, x28, #32
	ldr	x9, [x28, #-32]
	br	x9

.align 3
L235:
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
L223:
	mov	x0, #0
	mov	x1, x25
	mov	x2, x28
	bl	_kernel_Y_BNSeterr
.data
.globl __wsbytes
__wsbytes: .quad 3040
.globl __wsadjust
__wsadjust: .quad 256
.globl __vsbytes
__vsbytes: .quad 4160
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
L30:
	.byte	0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x20 
L31:
	.byte	0x64, 0x65, 0x6c, 0x65, 0x74, 0x65, 0x00, 0x00 
L32:
	.byte	0x69, 0x6e, 0x73, 0x65, 0x72, 0x74, 0x00, 0x00 
L33:
	.byte	0x70, 0x61, 0x67, 0x65, 0x64, 0x6f, 0x77, 0x6e 
L34:
	.byte	0x70, 0x61, 0x67, 0x65, 0x75, 0x70, 0x00, 0x00 
L35:
	.byte	0x65, 0x6e, 0x64, 0x00 
L36:
	.byte	0x68, 0x6f, 0x6d, 0x65 
L37:
	.byte	0x72, 0x69, 0x67, 0x68, 0x74, 0x00, 0x00, 0x00 
L38:
	.byte	0x6c, 0x65, 0x66, 0x74 
L39:
	.byte	0x64, 0x6f, 0x77, 0x6e 
L40:
	.byte	0x75, 0x70, 0x00, 0x00 
L41:
	.byte	0x6d, 0x6f, 0x64, 0x65 
L42:
	.byte	0x6d, 0x65, 0x6e, 0x75 
L43:
	.byte	0x73, 0x79, 0x73, 0x72, 0x65, 0x71, 0x00, 0x00 
L44:
	.byte	0x6c, 0x73, 0x75, 0x70, 0x65, 0x72, 0x00, 0x00 
L45:
	.byte	0x72, 0x73, 0x75, 0x70, 0x65, 0x72, 0x00, 0x00 
L46:
	.byte	0x6c, 0x6d, 0x65, 0x74, 0x61, 0x00, 0x00, 0x00 
L47:
	.byte	0x72, 0x6d, 0x65, 0x74, 0x61, 0x00, 0x00, 0x00 
L48:
	.byte	0x6c, 0x61, 0x6c, 0x74 
L49:
	.byte	0x72, 0x61, 0x6c, 0x74 
L50:
	.byte	0x6c, 0x63, 0x74, 0x72, 0x6c, 0x00, 0x00, 0x00 
L51:
	.byte	0x72, 0x63, 0x74, 0x72, 0x6c, 0x00, 0x00, 0x00 
L52:
	.byte	0x6c, 0x73, 0x68, 0x69, 0x66, 0x74, 0x00, 0x00 
L53:
	.byte	0x72, 0x73, 0x68, 0x69, 0x66, 0x74, 0x00, 0x00 
L54:
	.byte	0x73, 0x63, 0x72, 0x6f, 0x6c, 0x6c, 0x6f, 0x63 
	.byte	0x6b, 0x00, 0x00, 0x00 
L55:
	.byte	0x63, 0x61, 0x70, 0x73, 0x6c, 0x6f, 0x63, 0x6b 
L56:
	.byte	0x6e, 0x75, 0x6d, 0x6c, 0x6f, 0x63, 0x6b, 0x00 
L57:
	.byte	0x62, 0x61, 0x63, 0x6b, 0x73, 0x70, 0x61, 0x63 
	.byte	0x65, 0x00, 0x00, 0x00 
L58:
	.byte	0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x00 
L123:
	.byte	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
L124:
	.byte	0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x20 
	.byte	0x65, 0x76, 0x65, 0x6e, 0x74, 0x3a, 0x20, 0x00 
L125:
	.byte	0x0a, 0x00, 0x00, 0x00 
L126:
	.byte	0x63, 0x6f, 0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x2d 
	.byte	0x6b, 0x65, 0x79, 0x20, 0x72, 0x65, 0x6c, 0x65 
	.byte	0x61, 0x73, 0x65, 0x3a, 0x20, 0x00, 0x00, 0x00 
L127:
	.byte	0x63, 0x6f, 0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x2d 
	.byte	0x6b, 0x65, 0x79, 0x20, 0x70, 0x72, 0x65, 0x73 
	.byte	0x73, 0x3a, 0x20, 0x00 
L128:
	.byte	0x75, 0x6e, 0x6b, 0x6e, 0x6f, 0x77, 0x6e, 0x20 
	.byte	0x65, 0x76, 0x65, 0x6e, 0x74, 0x21, 0x0a, 0x00 
L129:
	.byte	0x71, 0x75, 0x69, 0x74, 0x20, 0x65, 0x76, 0x65 
	.byte	0x6e, 0x74, 0x0a, 0x00 
L130:
	.byte	0x75, 0x6e, 0x69, 0x63, 0x6f, 0x64, 0x65, 0x20 
	.byte	0x23, 0x00, 0x00, 0x00 
L131:
	.byte	0x6b, 0x65, 0x79, 0x20, 0x72, 0x65, 0x6c, 0x65 
	.byte	0x61, 0x73, 0x65, 0x3a, 0x20, 0x00, 0x00, 0x00 
L132:
	.byte	0x6b, 0x65, 0x79, 0x20, 0x70, 0x72, 0x65, 0x73 
	.byte	0x73, 0x3a, 0x20, 0x00 
L133:
	.byte	0x2c, 0x20, 0x6b, 0x73, 0x20, 0x3d, 0x20, 0x00 
L134:
	.byte	0x2c, 0x20, 0x79, 0x20, 0x3d, 0x20, 0x00, 0x00 
L135:
	.byte	0x2c, 0x20, 0x78, 0x20, 0x3d, 0x20, 0x00, 0x00 
L136:
	.byte	0x6d, 0x6f, 0x75, 0x73, 0x65, 0x20, 0x6d, 0x6f 
	.byte	0x74, 0x69, 0x6f, 0x6e, 0x2c, 0x20, 0x62, 0x73 
	.byte	0x20, 0x3d, 0x20, 0x00 
L137:
	.byte	0x62, 0x75, 0x74, 0x74, 0x6f, 0x6e, 0x20, 0x72 
	.byte	0x65, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x2c, 0x20 
	.byte	0x62, 0x20, 0x3d, 0x20 
L138:
	.byte	0x62, 0x75, 0x74, 0x74, 0x6f, 0x6e, 0x20, 0x70 
	.byte	0x72, 0x65, 0x73, 0x73, 0x2c, 0x20, 0x20, 0x20 
	.byte	0x62, 0x20, 0x3d, 0x20 
L139:
	.byte	0x65, 0x78, 0x70, 0x6f, 0x73, 0x65, 0x20, 0x65 
	.byte	0x76, 0x65, 0x6e, 0x74, 0x0a, 0x00, 0x00, 0x00 
L202:
	.byte	0x74, 0x65, 0x73, 0x74 
L203:
	.byte	0x6e, 0x75, 0x6d, 0x20, 0x62, 0x75, 0x66, 0x66 
	.byte	0x65, 0x72, 0x73, 0x3f, 0x3a, 0x20, 0x00, 0x00 
L204:
	.byte	0x72, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x68 
	.byte	0x65, 0x69, 0x67, 0x68, 0x74, 0x3f, 0x3a, 0x20 
L205:
	.byte	0x72, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x77 
	.byte	0x69, 0x64, 0x74, 0x68, 0x3f, 0x3a, 0x20, 0x00 
L206:
	.byte	0x44, 0x45, 0x42, 0x55, 0x47, 0x3a, 0x20, 0x44 
	.byte	0x65, 0x74, 0x65, 0x63, 0x74, 0x65, 0x64, 0x20 
	.byte	0x36, 0x34, 0x2d, 0x62, 0x69, 0x74, 0x20, 0x74 
	.byte	0x61, 0x72, 0x67, 0x65, 0x74, 0x20, 0x28, 0x42 
	.byte	0x50, 0x57, 0x3d, 0x38, 0x29, 0x0a, 0x00, 0x00 
L231:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc1 
L232:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x41 
L233:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc3 
L234:
	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x43 
