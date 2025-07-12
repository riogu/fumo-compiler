; ModuleID = 'some-llvm-ir.ll'
source_filename = "llvm-test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define i32 @pow(i32 %x, i32 %y) {
  br label %loop_start

loop_start:                                       ; preds = %loop, %0
  %i.0 = phi i32 [ 0, %0 ], [ %i.new, %loop ]
  %r.0 = phi i32 [ 1, %0 ], [ %r.new, %loop ]
  %done = icmp eq i32 %i.0, %y
  br i1 %done, label %exit, label %loop

loop:                                             ; preds = %loop_start
  %r.new = mul i32 %r.0, %x
  %i.new = add i32 %i.0, 1
  br label %loop_start

exit:                                             ; preds = %loop_start
  ret i32 %r.0
}
