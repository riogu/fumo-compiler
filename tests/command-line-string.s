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
	.globl	"calculator::get_value"         # -- Begin function calculator::get_value
	.p2align	4
	.type	"calculator::get_value",@function
"calculator::get_value":                # @"calculator::get_value"
# %bb.0:
	movl	(%rdi), %eax
	retq
.Lfunc_end1:
	.size	"calculator::get_value", .Lfunc_end1-"calculator::get_value"
                                        # -- End function
	.globl	"calculator::double"            # -- Begin function calculator::double
	.p2align	4
	.type	"calculator::double",@function
"calculator::double":                   # @"calculator::double"
# %bb.0:
	movl	(%rdi), %eax
	addl	%eax, %eax
	retq
.Lfunc_end2:
	.size	"calculator::double", .Lfunc_end2-"calculator::double"
                                        # -- End function
	.globl	"calculator::triple"            # -- Begin function calculator::triple
	.p2align	4
	.type	"calculator::triple",@function
"calculator::triple":                   # @"calculator::triple"
# %bb.0:
	movl	(%rdi), %eax
	leal	(%rax,%rax,2), %eax
	retq
.Lfunc_end3:
	.size	"calculator::triple", .Lfunc_end3-"calculator::triple"
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:
	movl	$69, %eax
	retq
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
