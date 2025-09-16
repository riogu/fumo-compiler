; ModuleID = 'src/tests/structs-and-postfix/example.fm'
source_filename = "src/tests/structs-and-postfix/example.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%"foo::SomeStruct" = type { %"foo::SomeStruct::InnerStruct", i32 }
%"foo::SomeStruct::InnerStruct" = type { i32, ptr }

@x = local_unnamed_addr global i32 0
@"foo::x" = local_unnamed_addr global i32 0
@"foo::inner::x" = local_unnamed_addr global i32 0
@global_example = local_unnamed_addr global %"foo::SomeStruct" zeroinitializer
@.str.3 = private unnamed_addr constant [8 x i8] c"example\00", align 1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"fumo.init.for: src/tests/structs-and-postfix/example.fm"() local_unnamed_addr #0 {
  store %"foo::SomeStruct" { %"foo::SomeStruct::InnerStruct" { i32 11111, ptr @.str.3 }, i32 66 }, ptr @global_example, align 16
  store i32 69420, ptr @"foo::x", align 4
  store i32 69, ptr @x, align 4
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

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, argmem: none, inaccessiblemem: none)
define i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #3 {
  store %"foo::SomeStruct" { %"foo::SomeStruct::InnerStruct" { i32 11111, ptr @.str.3 }, i32 66 }, ptr @global_example, align 16
  store i32 69420, ptr @"foo::x", align 4
  store i32 69, ptr @x, align 4
  %1 = load i32, ptr getelementptr inbounds nuw (i8, ptr @global_example, i64 16), align 16
  ret i32 %1
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, argmem: none, inaccessiblemem: none) "used" }
