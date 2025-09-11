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
	.globl	"counter::get_count"            # -- Begin function counter::get_count
	.p2align	4
	.type	"counter::get_count",@function
"counter::get_count":                   # @"counter::get_count"
# %bb.0:
	movl	(%rdi), %eax
	retq
.Lfunc_end1:
	.size	"counter::get_count", .Lfunc_end1-"counter::get_count"
                                        # -- End function
	.globl	"counter::get_count_ptr"        # -- Begin function counter::get_count_ptr
	.p2align	4
	.type	"counter::get_count_ptr",@function
"counter::get_count_ptr":               # @"counter::get_count_ptr"
# %bb.0:
	movq	%rdi, %rax
	retq
.Lfunc_end2:
	.size	"counter::get_count_ptr", .Lfunc_end2-"counter::get_count_ptr"
                                        # -- End function
	.globl	"counter::gaming"               # -- Begin function counter::gaming
	.p2align	4
	.type	"counter::gaming",@function
"counter::gaming":                      # @"counter::gaming"
# %bb.0:
	incl	(%rdi)
	retq
.Lfunc_end3:
	.size	"counter::gaming", .Lfunc_end3-"counter::gaming"
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$0, (%rsp)
	movq	%rsp, %rdi
	callq	"counter::increment"@PLT
	movl	(%rsp), %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
