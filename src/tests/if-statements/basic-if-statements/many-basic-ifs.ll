; ModuleID = 'src/tests/if-statements/basic-if-statements/many-basic-ifs.fm'
source_filename = "src/tests/if-statements/basic-if-statements/many-basic-ifs.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@x = local_unnamed_addr global i32 0
@y = local_unnamed_addr global i32 0
@z = local_unnamed_addr global i32 0
@result = local_unnamed_addr global i32 0
@a = local_unnamed_addr global i32 0
@b = local_unnamed_addr global i32 0
@flag = local_unnamed_addr global i1 false
@condition = local_unnamed_addr global i1 false
@outer_condition = local_unnamed_addr global i1 false
@inner_condition = local_unnamed_addr global i1 false

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"fumo.init.for: src/tests/if-statements/basic-if-statements/many-basic-ifs.fm"() local_unnamed_addr #0 {
  store i1 false, ptr @inner_condition, align 1
  store i1 true, ptr @outer_condition, align 1
  store i1 true, ptr @condition, align 1
  store i1 true, ptr @flag, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
fumo.user_main.exit:
  store i1 false, ptr @inner_condition, align 1
  store i1 true, ptr @outer_condition, align 1
  store i1 true, ptr @condition, align 1
  store i1 true, ptr @flag, align 1
  store i32 2, ptr @y, align 4
  store i32 3, ptr @z, align 4
  store i32 1, ptr @a, align 4
  store i32 2, ptr @b, align 4
  store i32 1, ptr @result, align 4
  store i32 2, ptr @x, align 4
  ret i32 0
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
