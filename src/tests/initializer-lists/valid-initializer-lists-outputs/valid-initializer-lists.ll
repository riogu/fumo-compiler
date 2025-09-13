; ModuleID = 'src/tests/initializer-lists/valid-initializer-lists-outputs/valid-initializer-lists'
source_filename = "src/tests/initializer-lists/valid-initializer-lists.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @fumo.init() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i32 @get_value() local_unnamed_addr #1 {
  ret i32 42
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i32 @another_func() local_unnamed_addr #1 {
  ret i32 24
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
  ret i32 0
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
