; ModuleID = 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm'
source_filename = "src/tests/if-statements/complex-conditions/complex-fumo-test.fm"

%"math::Point" = type { double, double }
%"math::utils::Stats" = type { i32, double }

@global_counter = global i32 0
@.str = private unnamed_addr constant [21 x i8] c"Point: (%.2f, %.2f)\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [260 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm' at line 35:\0A   |     printf(\22Point: (%.2f, %.2f)\\n\22, pt->x, pt->y);\0A   |                                       ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [267 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm' at line 35:\0A   |     printf(\22Point: (%.2f, %.2f)\\n\22, pt->x, pt->y);\0A   |                                              ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [251 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm' at line 37:\0A   |     if (let dist: f64 = pt->distance_from_origin()) {\0A   |                           ^ found null pointer dereference\0A\0A\00", align 1
@.str.3 = private unnamed_addr constant [32 x i8] c"Point is far from origin: %.2f\0A\00", align 1
@.str.4 = private unnamed_addr constant [32 x i8] c"Point is close to origin: %.2f\0A\00", align 1
@.str.5 = private unnamed_addr constant [33 x i8] c"Global counter: %d, Address: %p\0A\00", align 1
@.str_error_msg.6 = private unnamed_addr constant [282 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/complex-fumo-test.fm' at line 57:\0A   |     printf(\22Global counter: %d, Address: %p\\n\22, *ptr, ptr);\0A   |                                                    ^ found null pointer dereference\0A\0A\00", align 1
@.str.7 = private unnamed_addr constant [15 x i8] c"Values match!\0A\00", align 1
@.str.8 = private unnamed_addr constant [37 x i8] c"Fumo language feature demonstration\0A\00", align 1
@.str.9 = private unnamed_addr constant [15 x i8] c"Average: %.2f\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

declare i64 @strlen(ptr)

declare i32 @strcmp(ptr, ptr)

declare ptr @strcpy(ptr, ptr)

declare ptr @strcat(ptr, ptr)

declare ptr @malloc(i64)

declare void @free(ptr)

declare void @exit(i32)

declare void @abort()

declare ptr @memcpy(ptr, ptr, i64)

declare ptr @memset(ptr, i32, i64)

define void @"fumo.init.for: src/tests/if-statements/complex-conditions/complex-fumo-test.fm"() #0 {
  store i32 100, ptr @global_counter, align 4
  ret void
}

define double @"math::Point::distance_from_origin"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"math::Point", ptr %2, i32 0, i32 0
  %4 = load double, ptr %3, align 8
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %"math::Point", ptr %5, i32 0, i32 0
  %7 = load double, ptr %6, align 8
  %8 = fmul double %4, %7
  %9 = load ptr, ptr %this, align 8
  %10 = getelementptr inbounds nuw %"math::Point", ptr %9, i32 0, i32 1
  %11 = load double, ptr %10, align 8
  %12 = load ptr, ptr %this, align 8
  %13 = getelementptr inbounds nuw %"math::Point", ptr %12, i32 0, i32 1
  %14 = load double, ptr %13, align 8
  %15 = fmul double %11, %14
  %16 = fadd double %8, %15
  ret double %16
}

define void @"math::utils::Stats::add_value"(ptr %0, double %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"math::utils::Stats::add_value()::val" = alloca double, align 8
  store double 0.000000e+00, ptr %"math::utils::Stats::add_value()::val", align 8
  store double %1, ptr %"math::utils::Stats::add_value()::val", align 8
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %"math::utils::Stats", ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %"math::utils::Stats", ptr %5, i32 0, i32 0
  %7 = load i32, ptr %6, align 4
  %8 = add i32 %7, 1
  store i32 %8, ptr %4, align 4
  %9 = load ptr, ptr %this, align 8
  %10 = getelementptr inbounds nuw %"math::utils::Stats", ptr %9, i32 0, i32 1
  %11 = load ptr, ptr %this, align 8
  %12 = getelementptr inbounds nuw %"math::utils::Stats", ptr %11, i32 0, i32 1
  %13 = load double, ptr %12, align 8
  %14 = load double, ptr %"math::utils::Stats::add_value()::val", align 8
  %15 = fadd double %13, %14
  store double %15, ptr %10, align 8
  ret void
}

define double @"math::utils::Stats::average"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"math::utils::Stats", ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  %5 = icmp slt i32 0, %4
  br i1 %5, label %begin.if, label %end.if

begin.if:                                         ; preds = %1
  %6 = load ptr, ptr %this, align 8
  %7 = getelementptr inbounds nuw %"math::utils::Stats", ptr %6, i32 0, i32 1
  %8 = load double, ptr %7, align 8
  %9 = load ptr, ptr %this, align 8
  %10 = getelementptr inbounds nuw %"math::utils::Stats", ptr %9, i32 0, i32 0
  %11 = load i32, ptr %10, align 4
  %12 = sitofp i32 %11 to double
  %13 = fdiv double %8, %12
  ret double %13

end.if:                                           ; preds = %1
  ret double 0.000000e+00
}

define void @process_point(ptr %0) {
  %"process_point()::pt" = alloca ptr, align 8
  store ptr null, ptr %"process_point()::pt", align 8
  store ptr %0, ptr %"process_point()::pt", align 8
  %2 = load ptr, ptr %"process_point()::pt", align 8
  %is_null = icmp eq ptr %2, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %1
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %1
  %3 = getelementptr inbounds nuw %"math::Point", ptr %2, i32 0, i32 0
  %4 = load double, ptr %3, align 8
  %5 = load ptr, ptr %"process_point()::pt", align 8
  %is_null3 = icmp eq ptr %5, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %6 = getelementptr inbounds nuw %"math::Point", ptr %5, i32 0, i32 1
  %7 = load double, ptr %6, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @.str, double %4, double %7)
  %"process_point()::0::dist" = alloca double, align 8
  store double 0.000000e+00, ptr %"process_point()::0::dist", align 8
  %9 = load ptr, ptr %"process_point()::pt", align 8
  %is_null6 = icmp eq ptr %9, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %10 = call double @"math::Point::distance_from_origin"(ptr %9)
  %11 = alloca double, align 8
  store double %10, ptr %11, align 8
  %12 = load double, ptr %11, align 8
  store double %12, ptr %"process_point()::0::dist", align 8
  %tobool = fcmp one double %12, 0.000000e+00
  br i1 %tobool, label %begin.if, label %end.if

begin.if:                                         ; preds = %safe_deref5
  %13 = load double, ptr %"process_point()::0::dist", align 8
  %14 = fcmp olt double 2.500000e+01, %13
  br i1 %14, label %begin.if7, label %else

begin.if7:                                        ; preds = %begin.if
  %15 = load double, ptr %"process_point()::0::dist", align 8
  %16 = call i32 (ptr, ...) @printf(ptr @.str.3, double %15)
  br label %end.if8

else:                                             ; preds = %begin.if
  %17 = load double, ptr %"process_point()::0::dist", align 8
  %18 = call i32 (ptr, ...) @printf(ptr @.str.4, double %17)
  br label %end.if8

end.if8:                                          ; preds = %else, %begin.if7
  br label %end.if

end.if:                                           ; preds = %end.if8, %safe_deref5
  ret void
}

define void @demonstrate_features() {
  %"demonstrate_features()::point" = alloca %"math::Point", align 8
  store %"math::Point" zeroinitializer, ptr %"demonstrate_features()::point", align 8
  %1 = alloca %"math::Point", align 8
  store %"math::Point" zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %"math::Point", ptr %1, i32 0, i32 0
  store double 3.000000e+00, ptr %2, align 8
  %3 = getelementptr inbounds nuw %"math::Point", ptr %1, i32 0, i32 1
  store double 4.000000e+00, ptr %3, align 8
  %4 = load %"math::Point", ptr %1, align 8
  store %"math::Point" %4, ptr %"demonstrate_features()::point", align 8
  %"demonstrate_features()::stats" = alloca %"math::utils::Stats", align 8
  store %"math::utils::Stats" zeroinitializer, ptr %"demonstrate_features()::stats", align 8
  %5 = alloca %"math::utils::Stats", align 8
  store %"math::utils::Stats" zeroinitializer, ptr %5, align 8
  %6 = getelementptr inbounds nuw %"math::utils::Stats", ptr %5, i32 0, i32 0
  store i32 0, ptr %6, align 4
  %7 = getelementptr inbounds nuw %"math::utils::Stats", ptr %5, i32 0, i32 1
  store double 0.000000e+00, ptr %7, align 8
  %8 = load %"math::utils::Stats", ptr %5, align 8
  store %"math::utils::Stats" %8, ptr %"demonstrate_features()::stats", align 8
  %9 = getelementptr inbounds nuw %"math::Point", ptr %"demonstrate_features()::point", i32 0, i32 0
  %10 = load double, ptr %9, align 8
  call void @"math::utils::Stats::add_value"(ptr %"demonstrate_features()::stats", double %10)
  %11 = getelementptr inbounds nuw %"math::Point", ptr %"demonstrate_features()::point", i32 0, i32 1
  %12 = load double, ptr %11, align 8
  %13 = fmul double %12, 2.000000e+00
  call void @"math::utils::Stats::add_value"(ptr %"demonstrate_features()::stats", double %13)
  call void @process_point(ptr %"demonstrate_features()::point")
  %"demonstrate_features()::ptr" = alloca ptr, align 8
  store ptr null, ptr %"demonstrate_features()::ptr", align 8
  store ptr @global_counter, ptr %"demonstrate_features()::ptr", align 8
  %14 = load ptr, ptr %"demonstrate_features()::ptr", align 8
  %is_null = icmp eq ptr %14, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg.6)
  unreachable

safe_deref:                                       ; preds = %0
  %15 = load i32, ptr %14, align 4
  %16 = load ptr, ptr %"demonstrate_features()::ptr", align 8
  %17 = call i32 (ptr, ...) @printf(ptr @.str.5, i32 %15, ptr %16)
  %"demonstrate_features()::small_int" = alloca i8, align 1
  store i8 0, ptr %"demonstrate_features()::small_int", align 1
  store i32 42, ptr %"demonstrate_features()::small_int", align 4
  %"demonstrate_features()::big_int" = alloca i64, align 8
  store i64 0, ptr %"demonstrate_features()::big_int", align 4
  %18 = load i8, ptr %"demonstrate_features()::small_int", align 1
  store i8 %18, ptr %"demonstrate_features()::big_int", align 1
  %19 = load i64, ptr %"demonstrate_features()::big_int", align 4
  %20 = load i32, ptr @global_counter, align 4
  %21 = sext i32 %20 to i64
  %22 = icmp eq i64 %19, %21
  br i1 %22, label %begin.if, label %end.if

begin.if:                                         ; preds = %safe_deref
  %23 = call i32 (ptr, ...) @printf(ptr @.str.7)
  br label %end.if

end.if:                                           ; preds = %begin.if, %safe_deref
  ret void
}

define internal i32 @fumo.user_main() {
  %1 = call i32 (ptr, ...) @printf(ptr @.str.8)
  call void @demonstrate_features()
  %"main()::nested_stats" = alloca %"math::utils::Stats", align 8
  store %"math::utils::Stats" zeroinitializer, ptr %"main()::nested_stats", align 8
  %2 = alloca %"math::utils::Stats", align 8
  store %"math::utils::Stats" zeroinitializer, ptr %2, align 8
  %3 = getelementptr inbounds nuw %"math::utils::Stats", ptr %2, i32 0, i32 0
  store i32 5, ptr %3, align 4
  %4 = getelementptr inbounds nuw %"math::utils::Stats", ptr %2, i32 0, i32 1
  store double 1.000000e+02, ptr %4, align 8
  %5 = load %"math::utils::Stats", ptr %2, align 8
  store %"math::utils::Stats" %5, ptr %"main()::nested_stats", align 8
  %6 = call double @"math::utils::Stats::average"(ptr %"main()::nested_stats")
  %7 = alloca double, align 8
  store double %6, ptr %7, align 8
  %8 = load double, ptr %7, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @.str.9, double %8)
  ret i32 0
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/complex-conditions/complex-fumo-test.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
