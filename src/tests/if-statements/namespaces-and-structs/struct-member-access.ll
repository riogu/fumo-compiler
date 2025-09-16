; ModuleID = 'src/tests/if-statements/namespaces-and-structs/struct-member-access.fm'
source_filename = "src/tests/if-statements/namespaces-and-structs/struct-member-access.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/if-statements/namespaces-and-structs/struct-member-access.fm"() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define i1 @"Point::is_origin"(ptr nocapture readonly %0) local_unnamed_addr #1 {
  %2 = load i32, ptr %0, align 4
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %logic_and.rhs, label %logic_and.end

logic_and.rhs:                                    ; preds = %1
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 4
  %5 = load i32, ptr %4, align 4
  %6 = icmp eq i32 %5, 0
  br label %logic_and.end

logic_and.end:                                    ; preds = %logic_and.rhs, %1
  %phi.result = phi i1 [ false, %1 ], [ %6, %logic_and.rhs ]
  ret i1 %phi.result
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define i32 @"Point::distance_from_origin"(ptr nocapture readonly %0) local_unnamed_addr #1 {
  %2 = load i32, ptr %0, align 4
  %3 = mul i32 %2, %2
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 4
  %5 = load i32, ptr %4, align 4
  %6 = mul i32 %5, %5
  %7 = add i32 %6, %3
  ret i32 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef range(i32 0, 3) i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
  ret i32 1
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
