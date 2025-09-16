; ModuleID = 'src/tests/string-literals/namespace-strings.fm'
source_filename = "src/tests/string-literals/namespace-strings.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [16 x i8] c"Error occurred!\00", align 1
@.str.1 = private unnamed_addr constant [22 x i8] c"Operation successful!\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/string-literals/namespace-strings.fm"() local_unnamed_addr #1 {
  ret void
}

; Function Attrs: nofree nounwind
define void @"messages::config::print_error"(ptr nocapture readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = tail call i32 @puts(ptr nonnull dereferenceable(1) %2)
  ret void
}

; Function Attrs: nofree nounwind
define void @"messages::config::print_success"(ptr nocapture readonly %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %3 = load ptr, ptr %2, align 8
  %4 = tail call i32 @puts(ptr nonnull dereferenceable(1) %3)
  ret void
}

; Function Attrs: nofree nounwind
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
  %1 = tail call i32 @puts(ptr nonnull dereferenceable(1) @.str.1)
  %2 = tail call i32 @puts(ptr nonnull dereferenceable(1) @.str)
  ret i32 0
}

attributes #0 = { nofree nounwind }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #2 = { nofree nounwind "used" }
