; ModuleID = 'src/tests/string-literals/struct_member-outputs/struct_member'
source_filename = "src/tests/string-literals/struct_member.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [20 x i8] c"Hi, I'm %s, age %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [4 x i8] c"Bob\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @fumo.init() local_unnamed_addr #1 {
  ret void
}

; Function Attrs: nofree nounwind
define void @"person::introduce"(ptr nocapture readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %4 = load i32, ptr %3, align 4
  %5 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, ptr %2, i32 %4)
  ret void
}

; Function Attrs: nofree nounwind
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, ptr nonnull @.str.2, i32 30)
  ret i32 0
}

attributes #0 = { nofree nounwind }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #2 = { nofree nounwind "used" }
