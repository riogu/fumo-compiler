	.file	"command-line-string.fm"
	.text
	.globl	fumo._start                     # -- Begin function fumo._start
	.p2align	4
	.type	fumo._start,@function
fumo._start:                            # @fumo._start
# %bb.0:
	movq	y@GOTPCREL(%rip), %rax
	movl	$123123, (%rax)                 # imm = 0x1E0F3
	retq
.Lfunc_end0:
	.size	fumo._start, .Lfunc_end0-fumo._start
                                        # -- End function
	.globl	func                            # -- Begin function func
	.p2align	4
	.type	func,@function
func:                                   # @func
# %bb.0:
	retq
.Lfunc_end1:
	.size	func, .Lfunc_end1-func
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
# %bb.0:
	pushq	%rax
	callq	fumo._start@PLT
	movl	$123, %eax
	popq	%rcx
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
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
