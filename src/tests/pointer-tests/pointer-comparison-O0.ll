; ModuleID = 'src/tests/pointer-tests/pointer-comparison.fm'
source_filename = "src/tests/pointer-tests/pointer-comparison.fm"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [206 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/pointer-tests/pointer-comparison.fm' at line 10:\0A   |             if (*ptr1 < *ptr2) {\0A   |                     ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [214 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/pointer-tests/pointer-comparison.fm' at line 10:\0A   |             if (*ptr1 < *ptr2) {\0A   |                             ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/pointer-tests/pointer-comparison.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::a" = alloca i32, align 4
  store i32 0, ptr %"main()::a", align 4
  store i32 10, ptr %"main()::a", align 4
  %"main()::b" = alloca i32, align 4
  store i32 0, ptr %"main()::b", align 4
  store i32 20, ptr %"main()::b", align 4
  %"main()::ptr1" = alloca ptr, align 8
  store ptr null, ptr %"main()::ptr1", align 8
  store ptr %"main()::a", ptr %"main()::ptr1", align 8
  %"main()::ptr2" = alloca ptr, align 8
  store ptr null, ptr %"main()::ptr2", align 8
  store ptr %"main()::b", ptr %"main()::ptr2", align 8
  %"main()::ptr3" = alloca ptr, align 8
  store ptr null, ptr %"main()::ptr3", align 8
  store ptr %"main()::a", ptr %"main()::ptr3", align 8
  %1 = load ptr, ptr %"main()::ptr1", align 8
  %2 = load ptr, ptr %"main()::ptr3", align 8
  %3 = icmp eq ptr %1, %2
  br i1 %3, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %4 = load ptr, ptr %"main()::ptr1", align 8
  %5 = load ptr, ptr %"main()::ptr2", align 8
  %6 = icmp ne ptr %4, %5
  br i1 %6, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  %7 = load ptr, ptr %"main()::ptr1", align 8
  %is_null = icmp eq ptr %7, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %begin.if1
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %begin.if1
  %8 = load i32, ptr %7, align 4
  %9 = load ptr, ptr %"main()::ptr2", align 8
  %is_null5 = icmp eq ptr %9, null
  br i1 %is_null5, label %null_trap3, label %safe_deref4

null_trap3:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref4:                                      ; preds = %safe_deref
  %10 = load i32, ptr %9, align 4
  %11 = icmp slt i32 %8, %10
  br i1 %11, label %begin.if6, label %end.if7

begin.if6:                                        ; preds = %safe_deref4
  ret i32 1

end.if7:                                          ; preds = %safe_deref4
  br label %end.if2

end.if2:                                          ; preds = %end.if7, %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  ret i32 0
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/pointer-tests/pointer-comparison.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
