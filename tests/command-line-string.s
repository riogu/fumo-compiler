	.file	"command-line-string.fm"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:
	movq	y@GOTPCREL(%rip), %rax
	movl	$35, (%rax)
	xorl	%eax, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        # -- End function
	.type	x,@object                       # @x
	.bss
	.globl	x
	.p2align	2, 0x0
x:
	.long	0                               # 0x0
	.size	x, 4

	.type	y,@object                       # @y
	.globl	y
	.p2align	2, 0x0
y:
	.long	0                               # 0x0
	.size	y, 4

	.section	".note.GNU-stack","",@progbits
