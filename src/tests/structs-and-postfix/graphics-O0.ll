; ModuleID = 'src/tests/structs-and-postfix/graphics.fm'
source_filename = "src/tests/structs-and-postfix/graphics.fm"

%"graphics::shapes::circle" = type { i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [233 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/graphics.fm' at line 18:\0A   |     let radius_ptr: i32* = c_ptr->get_radius_ptr();\0A   |                                 ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [185 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/graphics.fm' at line 19:\0A   |     *radius_ptr = 10;\0A   |               ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [202 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/graphics.fm' at line 20:\0A   |     return c_ptr->get_area_approx();\0A   |                 ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/structs-and-postfix/graphics.fm"() #0 {
  ret void
}

define i32 @"graphics::shapes::circle::get_radius"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"graphics::shapes::circle", ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define i32 @"graphics::shapes::circle::get_area_approx"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = call i32 @"graphics::shapes::circle::get_radius"(ptr %2)
  %4 = load ptr, ptr %this, align 8
  %5 = call i32 @"graphics::shapes::circle::get_radius"(ptr %4)
  %6 = mul i32 %3, %5
  %7 = mul i32 %6, 3
  ret i32 %7
}

define ptr @"graphics::shapes::circle::get_radius_ptr"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"graphics::shapes::circle", ptr %2, i32 0, i32 0
  ret ptr %3
}

define internal i32 @fumo.user_main() {
  %"main()::c" = alloca %"graphics::shapes::circle", align 8
  store %"graphics::shapes::circle" zeroinitializer, ptr %"main()::c", align 4
  %1 = alloca %"graphics::shapes::circle", align 8
  store %"graphics::shapes::circle" zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %"graphics::shapes::circle", ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %"graphics::shapes::circle", ptr %1, i32 0, i32 0
  store i32 5, ptr %3, align 4
  %4 = load %"graphics::shapes::circle", ptr %1, align 4
  store %"graphics::shapes::circle" %4, ptr %"main()::c", align 4
  %"main()::c_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::c_ptr", align 8
  store ptr %"main()::c", ptr %"main()::c_ptr", align 8
  %"main()::radius_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::radius_ptr", align 8
  %5 = load ptr, ptr %"main()::c_ptr", align 8
  %is_null = icmp eq ptr %5, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %0
  %6 = call ptr @"graphics::shapes::circle::get_radius_ptr"(ptr %5)
  %7 = alloca ptr, align 8
  store ptr %6, ptr %7, align 8
  %8 = load ptr, ptr %7, align 8
  store ptr %8, ptr %"main()::radius_ptr", align 8
  %9 = load ptr, ptr %"main()::radius_ptr", align 8
  %is_null3 = icmp eq ptr %9, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  store i32 10, ptr %9, align 4
  %10 = load ptr, ptr %"main()::c_ptr", align 8
  %is_null6 = icmp eq ptr %10, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %11 = call i32 @"graphics::shapes::circle::get_area_approx"(ptr %10)
  %12 = alloca i32, align 4
  store i32 %11, ptr %12, align 4
  %13 = load i32, ptr %12, align 4
  ret i32 %13
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/structs-and-postfix/graphics.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
