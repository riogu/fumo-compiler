; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%counter = type { i32 }

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @fumo.init() local_unnamed_addr #0 {
  ret void
}

declare i32 @"counter::increment"(ptr) local_unnamed_addr

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define i32 @"counter::get_count"(ptr nocapture readonly %0) local_unnamed_addr #1 {
  %2 = load i32, ptr %0, align 4
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define ptr @"counter::get_count_ptr"(ptr readnone returned %0) local_unnamed_addr #2 {
  ret ptr %0
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @"counter::gaming"(ptr nocapture %0) local_unnamed_addr #3 {
  %2 = load i32, ptr %0, align 4
  %3 = add i32 %2, 1
  store i32 %3, ptr %0, align 4
  ret void
}

define i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #4 {
  %"main()::c.i" = alloca %counter, align 8
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %"main()::c.i")
  store i32 0, ptr %"main()::c.i", align 8
  %1 = call i32 @"counter::increment"(ptr nonnull %"main()::c.i")
  %2 = load i32, ptr %"main()::c.i", align 8
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %"main()::c.i")
  ret i32 %2
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #5

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #5

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #4 = { "used" }
attributes #5 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
