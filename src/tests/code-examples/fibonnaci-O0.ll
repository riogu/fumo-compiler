; ModuleID = 'src/tests/code-examples/fibonnaci.fm'
source_filename = "src/tests/code-examples/fibonnaci.fm"

@.str = private unnamed_addr constant [4 x i8] c"%ld\00", align 1
@.str.1 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [203 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/code-examples/fibonnaci.fm' at line 14:\0A   |     printf(\22%ld\\n\22, fib(*var));\0A   |                            ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/code-examples/fibonnaci.fm"() #0 {
  ret void
}

declare i32 @scanf(ptr, ...)

define i64 @fib(i64 %0) {
  %"fib()::n" = alloca i64, align 8
  store i64 0, ptr %"fib()::n", align 4
  store i64 %0, ptr %"fib()::n", align 4
  %2 = load i64, ptr %"fib()::n", align 4
  %3 = icmp slt i64 %2, 2
  br i1 %3, label %begin.if, label %end.if

begin.if:                                         ; preds = %1
  %4 = load i64, ptr %"fib()::n", align 4
  ret i64 %4

end.if:                                           ; preds = %1
  %5 = load i64, ptr %"fib()::n", align 4
  %6 = sub i64 %5, 1
  %7 = call i64 @fib(i64 %6)
  %8 = load i64, ptr %"fib()::n", align 4
  %9 = sub i64 %8, 2
  %10 = call i64 @fib(i64 %9)
  %11 = add i64 %7, %10
  ret i64 %11
}

define internal void @fumo.user_main() {
  %"main()::var" = alloca ptr, align 8
  store ptr null, ptr %"main()::var", align 8
  %1 = call i32 (ptr, ...) @scanf(ptr @.str, ptr %"main()::var")
  %2 = load ptr, ptr %"main()::var", align 8
  %is_null = icmp eq ptr %2, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %0
  %3 = load i64, ptr %2, align 4
  %4 = call i64 @fib(i64 %3)
  %5 = call i32 (ptr, ...) @printf(ptr @.str.1, i64 %4)
  ret void
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/code-examples/fibonnaci.fm"()
  call void @fumo.user_main()
  ret i32 0
}

attributes #0 = { "used" }
