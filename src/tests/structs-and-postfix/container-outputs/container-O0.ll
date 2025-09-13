; ModuleID = 'src/tests/structs-and-postfix/container-outputs/container'
source_filename = "src/tests/structs-and-postfix/container.fm"

%"container::inner" = type { i32 }
%"container::outer" = type { %"container::inner", ptr }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [198 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/container.fm' at line 19:\0A   |     inner_ptr->modify_data(delta);\0A   |              ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [204 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/container.fm' at line 20:\0A   |     return inner_ptr->get_data();\0A   |                     ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [186 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/container.fm' at line 25:\0A   |     outer_ptr->init();\0A   |              ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.3 = private unnamed_addr constant [225 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/container.fm' at line 26:\0A   |     return outer_ptr->get_inner_via_ptr()->get_data();\0A   |                     ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.4 = private unnamed_addr constant [246 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/container.fm' at line 26:\0A   |     return outer_ptr->get_inner_via_ptr()->get_data();\0A   |                                          ^ found null pointer dereference\0A\0A\00", align 1

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

define void @fumo.init() #0 {
  ret void
}

define i32 @"container::inner::get_data"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"container::inner", ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define void @"container::inner::modify_data"(ptr %0, i32 %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"container::inner::modify_data()::delta" = alloca i32, align 4
  store i32 0, ptr %"container::inner::modify_data()::delta", align 4
  store i32 %1, ptr %"container::inner::modify_data()::delta", align 4
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %"container::inner", ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %"container::inner", ptr %5, i32 0, i32 0
  %7 = load i32, ptr %6, align 4
  %8 = load i32, ptr %"container::inner::modify_data()::delta", align 4
  %9 = add i32 %7, %8
  store i32 %9, ptr %4, align 4
  ret void
}

define void @"container::outer::init"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"container::outer", ptr %2, i32 0, i32 1
  %4 = load ptr, ptr %this, align 8
  %5 = getelementptr inbounds nuw %"container::outer", ptr %4, i32 0, i32 0
  store ptr %5, ptr %3, align 8
  ret void
}

define ptr @"container::outer::get_inner_via_ptr"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"container::outer", ptr %2, i32 0, i32 1
  %4 = load ptr, ptr %3, align 8
  ret ptr %4
}

define i32 @"container::outer::cascade_modify"(ptr %0, i32 %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"container::outer::cascade_modify()::delta" = alloca i32, align 4
  store i32 0, ptr %"container::outer::cascade_modify()::delta", align 4
  store i32 %1, ptr %"container::outer::cascade_modify()::delta", align 4
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %"container::outer", ptr %3, i32 0, i32 1
  %5 = load ptr, ptr %4, align 8
  %is_null = icmp eq ptr %5, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %2
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %2
  %6 = load i32, ptr %"container::outer::cascade_modify()::delta", align 4
  call void @"container::inner::modify_data"(ptr %5, i32 %6)
  %7 = load ptr, ptr %this, align 8
  %8 = getelementptr inbounds nuw %"container::outer", ptr %7, i32 0, i32 1
  %9 = load ptr, ptr %8, align 8
  %is_null3 = icmp eq ptr %9, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %10 = call i32 @"container::inner::get_data"(ptr %9)
  %11 = alloca i32, align 4
  store i32 %10, ptr %11, align 4
  %12 = load i32, ptr %11, align 4
  ret i32 %12
}

define internal i32 @fumo.user_main() {
  %"main()::outer_obj" = alloca %"container::outer", align 8
  store %"container::outer" zeroinitializer, ptr %"main()::outer_obj", align 8
  %1 = alloca %"container::outer", align 8
  store %"container::outer" zeroinitializer, ptr %1, align 8
  %2 = alloca %"container::inner", align 8
  store %"container::inner" zeroinitializer, ptr %2, align 4
  %3 = getelementptr inbounds nuw %"container::inner", ptr %2, i32 0, i32 0
  store i32 0, ptr %3, align 4
  %4 = load %"container::inner", ptr %2, align 4
  %5 = getelementptr inbounds nuw %"container::outer", ptr %1, i32 0, i32 0
  store %"container::inner" %4, ptr %5, align 4
  %6 = getelementptr inbounds nuw %"container::outer", ptr %1, i32 0, i32 1
  store ptr null, ptr %6, align 8
  %7 = alloca %"container::inner", align 8
  store %"container::inner" zeroinitializer, ptr %7, align 4
  %8 = getelementptr inbounds nuw %"container::inner", ptr %7, i32 0, i32 0
  store i32 0, ptr %8, align 4
  %9 = getelementptr inbounds nuw %"container::inner", ptr %7, i32 0, i32 0
  store i32 42, ptr %9, align 4
  %10 = load %"container::inner", ptr %7, align 4
  %11 = getelementptr inbounds nuw %"container::outer", ptr %1, i32 0, i32 0
  store %"container::inner" %10, ptr %11, align 4
  %12 = load %"container::outer", ptr %1, align 8
  store %"container::outer" %12, ptr %"main()::outer_obj", align 8
  %"main()::outer_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::outer_ptr", align 8
  store ptr %"main()::outer_obj", ptr %"main()::outer_ptr", align 8
  %13 = load ptr, ptr %"main()::outer_ptr", align 8
  %is_null = icmp eq ptr %13, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref:                                       ; preds = %0
  call void @"container::outer::init"(ptr %13)
  %14 = load ptr, ptr %"main()::outer_ptr", align 8
  %is_null3 = icmp eq ptr %14, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.3)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %15 = call ptr @"container::outer::get_inner_via_ptr"(ptr %14)
  %is_null6 = icmp eq ptr %15, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.4)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %16 = call i32 @"container::inner::get_data"(ptr %15)
  %17 = alloca i32, align 4
  store i32 %16, ptr %17, align 4
  %18 = load i32, ptr %17, align 4
  ret i32 %18
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
