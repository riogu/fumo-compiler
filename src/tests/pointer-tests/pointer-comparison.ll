; ModuleID = 'src/tests/pointer-tests/pointer-comparison.fm'
source_filename = "src/tests/pointer-tests/pointer-comparison.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree
declare void @exit(i32) local_unnamed_addr #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/pointer-tests/pointer-comparison.fm"() local_unnamed_addr #2 {
  ret void
}

; Function Attrs: cold nofree noreturn
define void @fumo.runtime_error(ptr %0) local_unnamed_addr #3 {
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @0, ptr %0)
  tail call void @exit(i32 1) #4
  unreachable
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef range(i32 0, 2) i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
  ret i32 1
}

attributes #0 = { nofree nounwind }
attributes #1 = { nofree }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #3 = { cold nofree noreturn }
attributes #4 = { cold }
