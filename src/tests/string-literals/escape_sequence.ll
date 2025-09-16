; ModuleID = 'src/tests/string-literals/escape_sequence.fm'
source_filename = "src/tests/string-literals/escape_sequence.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [21 x i8] c"Line 1\0ALine 2\09Tabbed\00", align 1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/string-literals/escape_sequence.fm"() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: nofree nounwind
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #1 {
  %puts.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @.str)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { nofree nounwind "used" }
attributes #2 = { nofree nounwind }
