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
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:
	movl	$30, %eax
	retq
.Lfunc_end3:
	.size	main, .Lfunc_end3-main
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
