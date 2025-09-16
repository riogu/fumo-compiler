; ModuleID = 'src/tests/pointer-tests/pointer-deref.fm'
source_filename = "src/tests/pointer-tests/pointer-deref.fm"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [187 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/pointer-tests/pointer-deref.fm' at line 6:\0A   |         if (*ptr > 40) {\0A   |                ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [186 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/pointer-tests/pointer-deref.fm' at line 7:\0A   |             *ptr = 100;\0A   |                ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [197 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/pointer-tests/pointer-deref.fm' at line 8:\0A   |             if (*ptr == 100) {\0A   |                    ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/pointer-tests/pointer-deref.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::value" = alloca i32, align 4
  store i32 0, ptr %"main()::value", align 4
  store i32 42, ptr %"main()::value", align 4
  %"main()::ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::ptr", align 8
  store ptr %"main()::value", ptr %"main()::ptr", align 8
  %1 = load ptr, ptr %"main()::ptr", align 8
  %tobool = icmp ne ptr %1, null
  br i1 %tobool, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %2 = load ptr, ptr %"main()::ptr", align 8
  %is_null = icmp eq ptr %2, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %begin.if
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %begin.if
  %3 = load i32, ptr %2, align 4
  %4 = icmp slt i32 40, %3
  br i1 %4, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %safe_deref
  %5 = load ptr, ptr %"main()::ptr", align 8
  %is_null5 = icmp eq ptr %5, null
  br i1 %is_null5, label %null_trap3, label %safe_deref4

null_trap3:                                       ; preds = %begin.if1
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref4:                                      ; preds = %begin.if1
  store i32 100, ptr %5, align 4
  %6 = load ptr, ptr %"main()::ptr", align 8
  %is_null8 = icmp eq ptr %6, null
  br i1 %is_null8, label %null_trap6, label %safe_deref7

null_trap6:                                       ; preds = %safe_deref4
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref7:                                      ; preds = %safe_deref4
  %7 = load i32, ptr %6, align 4
  %8 = icmp eq i32 %7, 100
  br i1 %8, label %begin.if9, label %end.if10

begin.if9:                                        ; preds = %safe_deref7
  ret i32 1

end.if10:                                         ; preds = %safe_deref7
  br label %end.if2

end.if2:                                          ; preds = %end.if10, %safe_deref
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
  call void @"fumo.init.for: src/tests/pointer-tests/pointer-deref.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
