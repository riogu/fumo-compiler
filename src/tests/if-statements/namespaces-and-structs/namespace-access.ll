; ModuleID = 'src/tests/if-statements/namespaces-and-structs/namespace-access.fm'
source_filename = "src/tests/if-statements/namespaces-and-structs/namespace-access.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@"math::pi" = local_unnamed_addr global double 0.000000e+00

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"fumo.init.for: src/tests/if-statements/namespaces-and-structs/namespace-access.fm"() local_unnamed_addr #0 {
  store float 0x400921FA00000000, ptr @"math::pi", align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define range(i32 0, -2147483647) i32 @"math::abs"(i32 %0) local_unnamed_addr #1 {
common.ret:
  %common.ret.op = tail call i32 @llvm.abs.i32(i32 %0, i1 false)
  ret i32 %common.ret.op
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, argmem: none, inaccessiblemem: none)
define range(i32 0, 2) i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
  store float 0x400921FA00000000, ptr @"math::pi", align 8
  %1 = load double, ptr @"math::pi", align 8
  %2 = fcmp ogt double %1, 3.000000e+00
  %common.ret.op.i = zext i1 %2 to i32
  ret i32 %common.ret.op.i
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.abs.i32(i32, i1 immarg) #3

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, argmem: none, inaccessiblemem: none) "used" }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
