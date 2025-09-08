	.file	"command-line-string.fm"
	.text
	.globl	fumo.init                       # -- Begin function fumo.init
	.p2align	4
	.type	fumo.init,@function
fumo.init:                              # @fumo.init
# %bb.0:
	movq	x@GOTPCREL(%rip), %rax
	movl	$23, (%rax)
	movq	y@GOTPCREL(%rip), %rcx
	movq	%rax, (%rcx)
	retq
.Lfunc_end0:
	.size	fumo.init, .Lfunc_end0-fumo.init
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:                                # %entry
	pushq	%rax
	callq	fumo.init@PLT
	movl	$123, %eax
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
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
	.p2align	3, 0x0
y:
	.quad	0
	.size	y, 8

	.section	".note.GNU-stack","",@progbits
