    .attribute    4, 16
    .attribute    5, "rv64i2p1_m2p0_a2p1_c2p0_zmmul1p0_zaamo1p0_zalrsc1p0"
    .file    "command-line-string.fm"
    .text
    .p2align    1                               # -- Begin function fumo._start
    .type    fumo._start,@function
fumo._start:                            # @fumo._start
    .cfi_startproc
# %bb.0:
    lui    a1, %hi(y)
    lui    a0, 30
    addiw    a0, a0, 243
    sw    a0, %lo(y)(a1)
    ret
.Lfunc_end0:
    .size    fumo._start, .Lfunc_end0-fumo._start
    .cfi_endproc
                                        # -- End function
    .globl    func                            # -- Begin function func
    .p2align    1
    .type    func,@function
func:                                   # @func
    .cfi_startproc
# %bb.0:
    addi    sp, sp, -16
    .cfi_def_cfa_offset 16
    lui    a0, 17
    addiw    a0, a0, -212
    sw    a0, 12(sp)
    addi    sp, sp, 16
    .cfi_def_cfa_offset 0
    ret
.Lfunc_end1:
    .size    func, .Lfunc_end1-func
    .cfi_endproc
                                        # -- End function
    .globl    main                            # -- Begin function main
    .p2align    1
    .type    main,@function
main:                                   # @main
    .cfi_startproc
# %bb.0:
    addi    sp, sp, -16
    .cfi_def_cfa_offset 16
    sd    ra, 8(sp)                       # 8-byte Folded Spill
    .cfi_offset ra, -8
    call    fumo._start
    li    a0, 123
    ld    ra, 8(sp)                       # 8-byte Folded Reload
    .cfi_restore ra
    addi    sp, sp, 16
    .cfi_def_cfa_offset 0
    ret
.Lfunc_end2:
    .size    main, .Lfunc_end2-main
    .cfi_endproc
                                        # -- End function
    .type    x,@object                       # @x
    .bss
    .globl    x
    .p2align    2, 0x0
x:
    .word    0                               # 0x0
    .size    x, 4

    .type    y,@object                       # @y
    .globl    y
    .p2align    2, 0x0
y:
    .word    0                               # 0x0
    .size    y, 4

    .section    ".note.GNU-stack","",@progbits
    .addrsig
    .addrsig_sym fumo._start
    .addrsig_sym y
