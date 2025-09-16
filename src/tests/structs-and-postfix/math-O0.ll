; ModuleID = 'src/tests/structs-and-postfix/math.fm'
source_filename = "src/tests/structs-and-postfix/math.fm"

%"math::vector" = type { i32, i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [175 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/math.fm' at line 15:\0A   |     v_ptr->set_x(5);\0A   |          ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [214 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/math.fm' at line 16:\0A   |     let x_ptr: i32* = v_ptr->get_x_ptr();\0A   |                            ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [170 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/math.fm' at line 17:\0A   |     *x_ptr = 6;\0A   |          ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.3 = private unnamed_addr constant [200 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/math.fm' at line 18:\0A   |     return v_ptr->magnitude_squared();\0A   |                 ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/structs-and-postfix/math.fm"() #0 {
  ret void
}

define i32 @"math::vector::magnitude_squared"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"math::vector", ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %"math::vector", ptr %5, i32 0, i32 0
  %7 = load i32, ptr %6, align 4
  %8 = mul i32 %4, %7
  %9 = load ptr, ptr %this, align 8
  %10 = getelementptr inbounds nuw %"math::vector", ptr %9, i32 0, i32 1
  %11 = load i32, ptr %10, align 4
  %12 = load ptr, ptr %this, align 8
  %13 = getelementptr inbounds nuw %"math::vector", ptr %12, i32 0, i32 1
  %14 = load i32, ptr %13, align 4
  %15 = mul i32 %11, %14
  %16 = add i32 %8, %15
  ret i32 %16
}

define void @"math::vector::set_x"(ptr %0, i32 %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"math::vector::set_x()::new_x" = alloca i32, align 4
  store i32 0, ptr %"math::vector::set_x()::new_x", align 4
  store i32 %1, ptr %"math::vector::set_x()::new_x", align 4
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %"math::vector", ptr %3, i32 0, i32 0
  %5 = load i32, ptr %"math::vector::set_x()::new_x", align 4
  store i32 %5, ptr %4, align 4
  ret void
}

define ptr @"math::vector::get_x_ptr"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"math::vector", ptr %2, i32 0, i32 0
  ret ptr %3
}

define internal i32 @fumo.user_main() {
  %"main()::v" = alloca %"math::vector", align 8
  store %"math::vector" zeroinitializer, ptr %"main()::v", align 4
  %1 = alloca %"math::vector", align 8
  store %"math::vector" zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %"math::vector", ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %"math::vector", ptr %1, i32 0, i32 1
  store i32 0, ptr %3, align 4
  %4 = getelementptr inbounds nuw %"math::vector", ptr %1, i32 0, i32 0
  store i32 3, ptr %4, align 4
  %5 = getelementptr inbounds nuw %"math::vector", ptr %1, i32 0, i32 1
  store i32 4, ptr %5, align 4
  %6 = load %"math::vector", ptr %1, align 4
  store %"math::vector" %6, ptr %"main()::v", align 4
  %"main()::v_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::v_ptr", align 8
  store ptr %"main()::v", ptr %"main()::v_ptr", align 8
  %7 = load ptr, ptr %"main()::v_ptr", align 8
  %is_null = icmp eq ptr %7, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %0
  call void @"math::vector::set_x"(ptr %7, i32 5)
  %"main()::x_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::x_ptr", align 8
  %8 = load ptr, ptr %"main()::v_ptr", align 8
  %is_null3 = icmp eq ptr %8, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %9 = call ptr @"math::vector::get_x_ptr"(ptr %8)
  %10 = alloca ptr, align 8
  store ptr %9, ptr %10, align 8
  %11 = load ptr, ptr %10, align 8
  store ptr %11, ptr %"main()::x_ptr", align 8
  %12 = load ptr, ptr %"main()::x_ptr", align 8
  %is_null6 = icmp eq ptr %12, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  store i32 6, ptr %12, align 4
  %13 = load ptr, ptr %"main()::v_ptr", align 8
  %is_null9 = icmp eq ptr %13, null
  br i1 %is_null9, label %null_trap7, label %safe_deref8

null_trap7:                                       ; preds = %safe_deref5
  call void @fumo.runtime_error(ptr @.str_error_msg.3)
  unreachable

safe_deref8:                                      ; preds = %safe_deref5
  %14 = call i32 @"math::vector::magnitude_squared"(ptr %13)
  %15 = alloca i32, align 4
  store i32 %14, ptr %15, align 4
  %16 = load i32, ptr %15, align 4
  ret i32 %16
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/structs-and-postfix/math.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
