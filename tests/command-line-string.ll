; ModuleID = 'tests/command-line-string.fm'
source_filename = "command-line-string.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@x = local_unnamed_addr global i32 0
@y = local_unnamed_addr global i32 0

; Function Attrs: mustprogress nofree noinline norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @fumo.init() local_unnamed_addr #0 {
  store i32 123123, ptr @y, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @func() local_unnamed_addr #1 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, inaccessiblemem: none)
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
entry:
  tail call void @fumo.init()
  ret i32 0
}

attributes #0 = { mustprogress nofree noinline norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, inaccessiblemem: none) "used" }
