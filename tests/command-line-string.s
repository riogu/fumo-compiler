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
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:
	xorl	%eax, %eax
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
