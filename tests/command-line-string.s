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
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	leaq	.L__unnamed_2(%rip), %rdi
	callq	fumo.runtime_error@PLT
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%s"
	.size	.L__unnamed_1, 3

	.type	.L__unnamed_2,@object           # @1
.L__unnamed_2:
	.asciz	"-> \033[38;2;235;67;54m[runtime error]\033[0m in file 'command-line-string.fm' at line 15:\n   |     return node1.get_next()->get_next()->get_next()->value;\n   |                                                    ^ found null pointer dereference\n\n"
	.size	.L__unnamed_2, 241

	.section	".note.GNU-stack","",@progbits
