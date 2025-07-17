    source_filename = "example.b6eb2c7a6b40b4d2-cgu.0"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; example::square
define i32 @_ZN7example6square17hb32bcde4463f37c3E(i32 %x) unnamed_addr #0 {
start:
  %0 = call { i32, i1 } @llvm.smul.with.overflow.i32(i32 %x, i32 %x)
  %_2.0 = extractvalue { i32, i1 } %0, 0
  %_2.1 = extractvalue { i32, i1 } %0, 1
  %1 = call i1 @llvm.expect.i1(i1 %_2.1, i1 false)
  br i1 %1, label %panic, label %bb1

bb1:
  ret i32 %_2.0

panic:
  ; core::panicking::panic
  call void @_ZN4core9panicking5panic17ha338a74a5d65bf6fE(
    ptr align 1 @str.0,
    i64 33,
    ptr align 8 @alloc_1368addac7d22933d93af2809439e507
  )
  unreachable
}

declare { i32, i1 } @llvm.smul.with.overflow.i32(i32, i32) #1
declare i1 @llvm.expect.i1(i1, i1) #2

; core::panicking::panic
declare void @_ZN4core9panicking5panic17ha338a74a5d65bf6fE(ptr align 1, i64, ptr align 8) unnamed_addr #3

@alloc_9be5c135c0f7c91e35e471f025924b11 = private unnamed_addr constant
  <{ [15 x i8] }>
  <{ [15 x i8] c"/app/example.rs" }>, align 1

@alloc_1368addac7d22933d93af2809439e507 = private unnamed_addr constant
  <{ ptr, [16 x i8] }> <{
    ptr @alloc_9be5c135c0f7c91e35e471f025924b11,
    [16 x i8] c"\0F\00\00\00\00\00\00\00\02\00\00\00\03\00\00\00"
  }>, align 8

@str.0 = internal constant [33 x i8] c"attempt to multiply with overflow"

attributes #0 = { nonlazybind uwtable }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nocallback nofree nosync nounwind willreturn memory(none) }
attributes #3 = { cold noinline noreturn nonlazybind uwtable }
