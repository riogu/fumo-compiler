; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [241 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'command-line-string.fm' at line 15:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                                                    ^ found null pointer dereference\0A\0A\00", align 1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @fumo.init() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define ptr @"node::get_next"(ptr nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write)
define void @"node::set_next"(ptr nocapture writeonly initializes((8, 16)) %0, ptr %1) local_unnamed_addr #2 {
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  store ptr %1, ptr %3, align 8
  ret void
}

; Function Attrs: cold nofree noreturn
define void @fumo.runtime_error(ptr %0) local_unnamed_addr #3 {
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @0, ptr %0)
  tail call void @exit(i32 1) #7
  unreachable
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #4

; Function Attrs: nofree
declare void @exit(i32) local_unnamed_addr #5

; Function Attrs: cold nofree noreturn
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #6 {
  tail call void @fumo.runtime_error(ptr nonnull @1)
  unreachable
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #3 = { cold nofree noreturn }
attributes #4 = { nofree nounwind }
attributes #5 = { nofree }
attributes #6 = { cold nofree noreturn "used" }
attributes #7 = { cold }
