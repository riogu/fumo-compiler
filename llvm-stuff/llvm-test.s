	.intel_syntax noprefix
	.file	"llvm-test.c"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset rbp, -16
	mov	rbp, rsp
	.cfi_def_cfa_register rbp
	sub	rsp, 16
	mov	dword ptr [rbp - 4], 0
	lea	rdi, [rip + .L.str]
	mov	esi, 69420
	mov	al, 0
	call	printf@PLT
	mov	eax, 69
	add	rsp, 16
	pop	rbp
	.cfi_def_cfa rsp, 8
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"my cool var: %d\n"
	.size	.L.str, 17

	.ident	"clang version 20.1.7"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym printf
