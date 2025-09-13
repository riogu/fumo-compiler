; ModuleID = 'src/tests/command-line-string.out'
source_filename = "src/tests/structs-and-postfix/example.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%"foo::SomeStruct" = type { i32, %"foo::SomeStruct::InnerStruct" }
%"foo::SomeStruct::InnerStruct" = type { i32, ptr }

@x = local_unnamed_addr global i32 0
@"foo::x" = local_unnamed_addr global i32 0
@"foo::inner::x" = local_unnamed_addr global i32 0

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @fumo.init() local_unnamed_addr #0 {
  store i32 69, ptr @x, align 4
  store i32 69420, ptr @"foo::x", align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"foo::inner::func"() local_unnamed_addr #1 {
  store i32 222222, ptr @"foo::inner::x", align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"foo::func"(i32 %0, double %1) local_unnamed_addr #1 {
  %3 = mul i32 %0, %0
  %4 = add i32 %3, 111111
  store i32 %4, ptr @"foo::x", align 4
  store i32 333333, ptr @"foo::inner::x", align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define i32 @"foo::SomeStruct::InnerStruct::gaming_func"(ptr nocapture readonly %0) local_unnamed_addr #2 {
  %2 = load i32, ptr %0, align 4
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noalias noundef nonnull ptr @some_func() local_unnamed_addr #3 {
  %"some_func()::global_example" = alloca %"foo::SomeStruct", align 8
  ret ptr %"some_func()::global_example"
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
  store i32 69, ptr @x, align 4
  store i32 69420, ptr @"foo::x", align 4
  ret i32 0
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
