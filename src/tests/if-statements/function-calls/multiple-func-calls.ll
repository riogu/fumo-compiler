; ModuleID = 'src/tests/if-statements/function-calls/multiple-func-calls.fm'
source_filename = "src/tests/if-statements/function-calls/multiple-func-calls.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/if-statements/function-calls/multiple-func-calls.fm"() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i32 @add(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = add i32 %1, %0
  ret i32 %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i32 @multiply(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = mul i32 %1, %0
  ret i32 %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef range(i32 0, 2) i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
  ret i32 0
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
