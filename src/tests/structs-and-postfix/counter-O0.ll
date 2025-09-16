; ModuleID = 'src/tests/structs-and-postfix/counter.fm'
source_filename = "src/tests/structs-and-postfix/counter.fm"

%counter = type { i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [181 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/counter.fm' at line 15:\0A   |     c_ptr->increment();\0A   |          ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [229 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/counter.fm' at line 16:\0A   |     let count_ptr: i32* = c_ptr->get_count_ptr();\0A   |                                ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [191 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/counter.fm' at line 17:\0A   |     return *count_ptr;\0A   |                     ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/structs-and-postfix/counter.fm"() #0 {
  ret void
}

define void @"counter::increment"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %counter, ptr %2, i32 0, i32 0
  %4 = load ptr, ptr %this, align 8
  %5 = getelementptr inbounds nuw %counter, ptr %4, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = add i32 %6, 1
  store i32 %7, ptr %3, align 4
  ret void
}

define i32 @"counter::get_count"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %counter, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define ptr @"counter::get_count_ptr"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %counter, ptr %2, i32 0, i32 0
  ret ptr %3
}

define internal i32 @fumo.user_main() {
  %"main()::c" = alloca %counter, align 8
  store %counter zeroinitializer, ptr %"main()::c", align 4
  %1 = alloca %counter, align 8
  store %counter zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %counter, ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %counter, ptr %1, i32 0, i32 0
  store i32 68, ptr %3, align 4
  %4 = load %counter, ptr %1, align 4
  store %counter %4, ptr %"main()::c", align 4
  %"main()::c_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::c_ptr", align 8
  store ptr %"main()::c", ptr %"main()::c_ptr", align 8
  %5 = load ptr, ptr %"main()::c_ptr", align 8
  %is_null = icmp eq ptr %5, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %0
  call void @"counter::increment"(ptr %5)
  %"main()::count_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::count_ptr", align 8
  %6 = load ptr, ptr %"main()::c_ptr", align 8
  %is_null3 = icmp eq ptr %6, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %7 = call ptr @"counter::get_count_ptr"(ptr %6)
  %8 = alloca ptr, align 8
  store ptr %7, ptr %8, align 8
  %9 = load ptr, ptr %8, align 8
  store ptr %9, ptr %"main()::count_ptr", align 8
  %10 = load ptr, ptr %"main()::count_ptr", align 8
  %is_null6 = icmp eq ptr %10, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %11 = load i32, ptr %10, align 4
  ret i32 %11
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/structs-and-postfix/counter.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
