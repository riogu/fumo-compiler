; ModuleID = 'src/tests/if-statements/function-calls/function-call-condition.fm'
source_filename = "src/tests/if-statements/function-calls/function-call-condition.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/if-statements/function-calls/function-call-condition.fm"() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i1 @is_positive(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 0
  ret i1 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i64 @get_value() local_unnamed_addr #1 {
  ret i64 42
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef range(i32 0, 2) i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
  ret i32 1
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
