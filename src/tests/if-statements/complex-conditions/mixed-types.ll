; ModuleID = 'src/tests/if-statements/complex-conditions/mixed-types.fm'
source_filename = "src/tests/if-statements/complex-conditions/mixed-types.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @"fumo.init.for: src/tests/if-statements/complex-conditions/mixed-types.fm"() local_unnamed_addr #0 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define range(i32 0, 2) i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
  %"main()::float_val.i" = alloca double, align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %"main()::float_val.i")
  store double 0.000000e+00, ptr %"main()::float_val.i", align 8
  store float 0x40091EB860000000, ptr %"main()::float_val.i", align 8
  %"main()::float_val.i.0.main()::float_val.i.0.main()::float_val.i.0.main()::float_val.0.main()::float_val.0.main()::float_val.0..i" = load double, ptr %"main()::float_val.i", align 8
  %1 = fcmp ogt double %"main()::float_val.i.0.main()::float_val.i.0.main()::float_val.i.0.main()::float_val.0.main()::float_val.0.main()::float_val.0..i", 3.000000e+00
  %..i = zext i1 %1 to i32
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %"main()::float_val.i")
  ret i32 %..i
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
