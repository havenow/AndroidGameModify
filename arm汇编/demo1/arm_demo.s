	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 10	@ Tag_CPU_arch
	.eabi_attribute	7, 65	@ Tag_CPU_arch_profile
	.eabi_attribute	8, 1	@ Tag_ARM_ISA_use
	.eabi_attribute	9, 2	@ Tag_THUMB_ISA_use
	.fpu	neon
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	15, 1	@ Tag_ABI_PCS_RW_data
	.eabi_attribute	16, 1	@ Tag_ABI_PCS_RO_data
	.eabi_attribute	17, 2	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 0	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	18, 4	@ Tag_ABI_PCS_wchar_t
	.eabi_attribute	26, 2	@ Tag_ABI_enum_size
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"demo.c"
	.globl	main                            @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                              @ @main
main:
	.fnstart
@ %bb.0:
	.save	{r11, lr}
	push	{r11, lr}		@保持寄存器r11 lr到栈里面		; <main+0>
	.setfp	r11, sp
	mov	r11, sp				@sp赋值给r11					; <main+4>
	.pad	#16
	sub	sp, sp, #16			@sp = sp -16
	ldr	r2, .LCPI0_0		@ldr	r2, [pc, #56]	; <main+76>
.LPC0_0:
	add	r2, pc, r2
	ldr	r3, .LCPI0_1		@ldr	r3, [pc, #52]	; <main+80>
.LPC0_1:
	add	r3, pc, r3
	movw	r12, #0
	str	r12, [r11, #-4]
	str	r0, [sp, #8]
	str	r1, [sp, #4]
	mov	r0, r2
	mov	r1, r3
	bl	printf
	movw	r1, #0
	str	r0, [sp]                        @ 4-byte Spill
	mov	r0, r1
	mov	sp, r11
	pop	{r11, pc}
	.p2align	2
@ %bb.1:
.LCPI0_0:
	.long	.L.str-(.LPC0_0+8)
.LCPI0_1:
	.long	.L.str.1-(.LPC0_1+8)
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	.L.str,%object                  @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"fuck %s\n"
	.size	.L.str, 9

	.type	.L.str.1,%object                @ @.str.1
.L.str.1:
	.asciz	"armv7a"
	.size	.L.str.1, 7

	.ident	"Android (6875598, based on r399163b) clang version 11.0.5 (https://android.googlesource.com/toolchain/llvm-project 87f1315dfbea7c137aa2e6d362dbb457e388158d)"
	.section	".note.GNU-stack","",%progbits
