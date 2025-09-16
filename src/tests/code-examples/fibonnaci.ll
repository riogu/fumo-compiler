; ModuleID = 'src/tests/code-examples/fibonnaci.fm'
source_filename = "src/tests/code-examples/fibonnaci.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1
@.str.1 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [203 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/code-examples/fibonnaci.fm' at line 14:\0A   |     printf(\22%ld\\n\22, fib(*var));\0A   |                            ^ found null pointer dereference\0A\0A\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree
declare void @exit(i32) local_unnamed_addr #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/code-examples/fibonnaci.fm"() local_unnamed_addr #2 {
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @scanf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nosync nounwind memory(none)
define i64 @fib(i64 %0) local_unnamed_addr #3 {
  %2 = icmp slt i64 %0, 2
  br i1 %2, label %common.ret, label %end.if

common.ret:                                       ; preds = %end.if, %1
  %accumulator.tr.lcssa = phi i64 [ 0, %1 ], [ %6, %end.if ]
  %.tr.lcssa = phi i64 [ %0, %1 ], [ %5, %end.if ]
  %accumulator.ret.tr = add i64 %.tr.lcssa, %accumulator.tr.lcssa
  ret i64 %accumulator.ret.tr

end.if:                                           ; preds = %1, %end.if
  %.tr5 = phi i64 [ %5, %end.if ], [ %0, %1 ]
  %accumulator.tr4 = phi i64 [ %6, %end.if ], [ 0, %1 ]
  %3 = add nsw i64 %.tr5, -1
  %4 = tail call i64 @fib(i64 %3)
  %5 = add nsw i64 %.tr5, -2
  %6 = add i64 %4, %accumulator.tr4
  %7 = icmp samesign ult i64 %.tr5, 4
  br i1 %7, label %common.ret, label %end.if
}

; Function Attrs: cold nofree noreturn
define void @fumo.runtime_error(ptr %0) local_unnamed_addr #4 {
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @0, ptr %0)
  tail call void @exit(i32 1) #7
  unreachable
}

; Function Attrs: nofree
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #5 {
  %"main()::var.i" = alloca ptr, align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %"main()::var.i")
  store ptr null, ptr %"main()::var.i", align 8
  %1 = call i32 (ptr, ...) @scanf(ptr nonnull @.str, ptr nonnull %"main()::var.i")
  %2 = load ptr, ptr %"main()::var.i", align 8
  %is_null.i = icmp eq ptr %2, null
  br i1 %is_null.i, label %null_trap.i, label %fumo.user_main.exit

null_trap.i:                                      ; preds = %0
  call void @fumo.runtime_error(ptr nonnull @.str_error_msg)
  unreachable

fumo.user_main.exit:                              ; preds = %0
  %3 = load i64, ptr %2, align 4
  %4 = call i64 @fib(i64 %3)
  %5 = call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.1, i64 %4)
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %"main()::var.i")
  ret i32 0
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #6

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #6

attributes #0 = { nofree nounwind }
attributes #1 = { nofree }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #3 = { nofree nosync nounwind memory(none) }
attributes #4 = { cold nofree noreturn }
attributes #5 = { nofree "used" }
attributes #6 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #7 = { cold }
