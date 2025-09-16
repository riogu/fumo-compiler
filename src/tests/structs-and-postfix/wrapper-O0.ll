; ModuleID = 'src/tests/structs-and-postfix/wrapper.fm'
source_filename = "src/tests/structs-and-postfix/wrapper.fm"

%wrapper = type { ptr }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [254 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/wrapper.fm' at line 6:\0A   |     fn deref_twice() -> i32 { return **get_ptr_ptr(); }\0A   |                                                    ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [254 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/wrapper.fm' at line 6:\0A   |     fn deref_twice() -> i32 { return **get_ptr_ptr(); }\0A   |                                                    ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [181 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/wrapper.fm' at line 14:\0A   |     **ptr_ptr = 200;\0A   |             ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.3 = private unnamed_addr constant [181 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/wrapper.fm' at line 14:\0A   |     **ptr_ptr = 200;\0A   |             ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/structs-and-postfix/wrapper.fm"() #0 {
  ret void
}

define void @"wrapper::set_ptr"(ptr %0, ptr %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"wrapper::set_ptr()::p" = alloca ptr, align 8
  store ptr null, ptr %"wrapper::set_ptr()::p", align 8
  store ptr %1, ptr %"wrapper::set_ptr()::p", align 8
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %wrapper, ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %"wrapper::set_ptr()::p", align 8
  store ptr %5, ptr %4, align 8
  ret void
}

define ptr @"wrapper::get_ptr_ptr"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %wrapper, ptr %2, i32 0, i32 0
  ret ptr %3
}

define i32 @"wrapper::deref_twice"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = call ptr @"wrapper::get_ptr_ptr"(ptr %2)
  %is_null = icmp eq ptr %3, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %1
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %1
  %4 = load ptr, ptr %3, align 8
  %is_null3 = icmp eq ptr %4, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %5 = load i32, ptr %4, align 4
  ret i32 %5
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 0, ptr %"main()::x", align 4
  store i32 100, ptr %"main()::x", align 4
  %"main()::w" = alloca %wrapper, align 8
  store %wrapper zeroinitializer, ptr %"main()::w", align 8
  %1 = alloca %wrapper, align 8
  store %wrapper zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %wrapper, ptr %1, i32 0, i32 0
  store ptr null, ptr %2, align 8
  %3 = load %wrapper, ptr %1, align 8
  store %wrapper %3, ptr %"main()::w", align 8
  call void @"wrapper::set_ptr"(ptr %"main()::w", ptr %"main()::x")
  %"main()::ptr_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::ptr_ptr", align 8
  %4 = call ptr @"wrapper::get_ptr_ptr"(ptr %"main()::w")
  %5 = alloca ptr, align 8
  store ptr %4, ptr %5, align 8
  %6 = load ptr, ptr %5, align 8
  store ptr %6, ptr %"main()::ptr_ptr", align 8
  %7 = load ptr, ptr %"main()::ptr_ptr", align 8
  %is_null = icmp eq ptr %7, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref:                                       ; preds = %0
  %8 = load ptr, ptr %7, align 8
  %is_null3 = icmp eq ptr %8, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.3)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  store i32 200, ptr %8, align 4
  %9 = call i32 @"wrapper::deref_twice"(ptr %"main()::w")
  %10 = alloca i32, align 4
  store i32 %9, ptr %10, align 4
  %11 = load i32, ptr %10, align 4
  ret i32 %11
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/structs-and-postfix/wrapper.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
