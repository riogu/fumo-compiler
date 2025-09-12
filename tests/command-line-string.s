	.file	"command-line-string.fm"
	.text
	.globl	fumo.init                       # -- Begin function fumo.init
	.p2align	4
	.type	fumo.init,@function
fumo.init:                              # @fumo.init
# %bb.0:
	retq
.Lfunc_end0:
	.size	fumo.init, .Lfunc_end0-fumo.init
                                        # -- End function
	.globl	"node::get_next"                # -- Begin function node::get_next
	.p2align	4
	.type	"node::get_next",@function
"node::get_next":                       # @"node::get_next"
# %bb.0:
	movq	8(%rdi), %rax
	retq
.Lfunc_end1:
	.size	"node::get_next", .Lfunc_end1-"node::get_next"
                                        # -- End function
	.globl	"node::set_next"                # -- Begin function node::set_next
	.p2align	4
	.type	"node::set_next",@function
"node::set_next":                       # @"node::set_next"
# %bb.0:
	movq	%rsi, 8(%rdi)
	retq
.Lfunc_end2:
	.size	"node::set_next", .Lfunc_end2-"node::set_next"
                                        # -- End function
	.section	.text.unlikely.,"ax",@progbits
	.globl	fumo.runtime_error              # -- Begin function fumo.runtime_error
	.p2align	4
	.type	fumo.runtime_error,@function
fumo.runtime_error:                     # @fumo.runtime_error
	.cfi_startproc
# %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	%rdi, %rsi
	leaq	.L__unnamed_1(%rip), %rdi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$1, %edi
	callq	exit@PLT
.Lfunc_end3:
	.size	fumo.runtime_error, .Lfunc_end3-fumo.runtime_error
	.cfi_endproc
                                        # -- End function
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:
	movl	$40, %eax
	retq
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%s"
	.size	.L__unnamed_1, 3

	.section	".note.GNU-stack","",@progbits
