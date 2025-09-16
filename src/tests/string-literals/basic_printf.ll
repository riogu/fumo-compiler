; ModuleID = 'src/tests/string-literals/basic_printf.fm'
source_filename = "src/tests/string-literals/basic_printf.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [6 x i8] c"fumos\00", align 1
@.str.1 = private unnamed_addr constant [14 x i8] c"i have %d %s\0A\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/string-literals/basic_printf.fm"() local_unnamed_addr #1 {
  ret void
}

; Function Attrs: nofree nounwind
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.1, i32 69, ptr nonnull @.str)
  ret i32 0
}

attributes #0 = { nofree nounwind }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #2 = { nofree nounwind "used" }
