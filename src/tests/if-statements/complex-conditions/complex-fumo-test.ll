; ModuleID = 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm'
source_filename = "src/tests/if-statements/complex-conditions/complex-fumo-test.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@global_counter = global i32 0
@.str = private unnamed_addr constant [21 x i8] c"Point: (%.2f, %.2f)\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [260 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm' at line 35:\0A   |     printf(\22Point: (%.2f, %.2f)\\n\22, pt->x, pt->y);\0A   |                                       ^ found null pointer dereference\0A\0A\00", align 1
@.str.3 = private unnamed_addr constant [32 x i8] c"Point is far from origin: %.2f\0A\00", align 1
@.str.4 = private unnamed_addr constant [32 x i8] c"Point is close to origin: %.2f\0A\00", align 1
@.str.5 = private unnamed_addr constant [33 x i8] c"Global counter: %d, Address: %p\0A\00", align 1
@.str.9 = private unnamed_addr constant [15 x i8] c"Average: %.2f\0A\00", align 1
@str = private unnamed_addr constant [14 x i8] c"Values match!\00", align 1
@str.10 = private unnamed_addr constant [36 x i8] c"Fumo language feature demonstration\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree
declare void @exit(i32) local_unnamed_addr #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"fumo.init.for: src/tests/if-statements/complex-conditions/complex-fumo-test.fm"() local_unnamed_addr #2 {
  store i32 100, ptr @global_counter, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define double @"math::Point::distance_from_origin"(ptr nocapture readonly %0) local_unnamed_addr #3 {
  %2 = load double, ptr %0, align 8
  %3 = fmul double %2, %2
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %5 = load double, ptr %4, align 8
  %6 = fmul double %5, %5
  %7 = fadd double %3, %6
  ret double %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @"math::utils::Stats::add_value"(ptr nocapture %0, double %1) local_unnamed_addr #4 {
  %3 = load i32, ptr %0, align 4
  %4 = add i32 %3, 1
  store i32 %4, ptr %0, align 4
  %5 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %6 = load double, ptr %5, align 8
  %7 = fadd double %1, %6
  store double %7, ptr %5, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define double @"math::utils::Stats::average"(ptr nocapture readonly %0) local_unnamed_addr #3 {
  %2 = load i32, ptr %0, align 4
  %3 = icmp sgt i32 %2, 0
  br i1 %3, label %begin.if, label %common.ret

common.ret:                                       ; preds = %1, %begin.if
  %common.ret.op = phi double [ %7, %begin.if ], [ 0.000000e+00, %1 ]
  ret double %common.ret.op

begin.if:                                         ; preds = %1
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %5 = load double, ptr %4, align 8
  %6 = uitofp nneg i32 %2 to double
  %7 = fdiv double %5, %6
  br label %common.ret
}

; Function Attrs: nofree
define void @process_point(ptr readonly %0) local_unnamed_addr #1 {
  %is_null = icmp eq ptr %0, null
  br i1 %is_null, label %null_trap, label %safe_deref2

null_trap:                                        ; preds = %1
  tail call void @fumo.runtime_error(ptr nonnull @.str_error_msg)
  unreachable

safe_deref2:                                      ; preds = %1
  %2 = load double, ptr %0, align 8
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %4 = load double, ptr %3, align 8
  %5 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, double %2, double %4)
  %6 = load double, ptr %0, align 8
  %7 = fmul double %6, %6
  %8 = load double, ptr %3, align 8
  %9 = fmul double %8, %8
  %10 = fadd double %7, %9
  %tobool = fcmp ueq double %10, 0.000000e+00
  br i1 %tobool, label %end.if, label %begin.if

begin.if:                                         ; preds = %safe_deref2
  %11 = fcmp ogt double %10, 2.500000e+01
  %.str.3..str.4 = select i1 %11, ptr @.str.3, ptr @.str.4
  %12 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) %.str.3..str.4, double %10)
  br label %end.if

end.if:                                           ; preds = %begin.if, %safe_deref2
  ret void
}

; Function Attrs: nofree nounwind
define void @demonstrate_features() local_unnamed_addr #0 {
process_point.exit:
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, double 3.000000e+00, double 4.000000e+00)
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.4, double 2.500000e+01)
  %2 = load i32, ptr @global_counter, align 4
  %3 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.5, i32 %2, ptr nonnull @global_counter)
  %4 = load i32, ptr @global_counter, align 4
  %5 = icmp eq i32 %4, 42
  br i1 %5, label %begin.if, label %end.if

begin.if:                                         ; preds = %process_point.exit
  %puts = tail call i32 @puts(ptr nonnull dereferenceable(1) @str)
  br label %end.if

end.if:                                           ; preds = %begin.if, %process_point.exit
  ret void
}

; Function Attrs: cold nofree noreturn
define void @fumo.runtime_error(ptr %0) local_unnamed_addr #5 {
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @0, ptr %0)
  tail call void @exit(i32 1) #7
  unreachable
}

; Function Attrs: nofree nounwind
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #6 {
  store i32 100, ptr @global_counter, align 4
  %puts.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @str.10)
  %1 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, double 3.000000e+00, double 4.000000e+00)
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.4, double 2.500000e+01)
  %3 = load i32, ptr @global_counter, align 4
  %4 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.5, i32 %3, ptr nonnull @global_counter)
  %5 = load i32, ptr @global_counter, align 4
  %6 = icmp eq i32 %5, 42
  br i1 %6, label %begin.if.i.i, label %fumo.user_main.exit

begin.if.i.i:                                     ; preds = %0
  %puts.i.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @str)
  br label %fumo.user_main.exit

fumo.user_main.exit:                              ; preds = %0, %begin.if.i.i
  %7 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.9, double 2.000000e+01)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
attributes #1 = { nofree }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #4 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { cold nofree noreturn }
attributes #6 = { nofree nounwind "used" }
attributes #7 = { cold }
