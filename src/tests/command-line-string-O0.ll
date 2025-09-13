; ModuleID = 'tests/command-line-string.out'
source_filename = "tests/structs-and-postfix/calculator.fm"

%calculator = type { i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [219 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'tests/structs-and-postfix/calculator.fm' at line 12:\0A   |     let result: i32 = calc_ptr->double();\0A   |                               ^ found null pointer dereference\0A\0A\00", align 1

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

define i32 @"calculator::get_value"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %calculator, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define i32 @"calculator::double"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = call i32 @"calculator::get_value"()
  %4 = mul i32 %3, 2
  ret i32 %4
}

define i32 @"calculator::triple"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = call i32 @"calculator::get_value"()
  %4 = mul i32 %3, 3
  ret i32 %4
}

define internal i32 @fumo.user_main() {
  %"main()::calc" = alloca %calculator, align 8
  store %calculator zeroinitializer, ptr %"main()::calc", align 4
  %1 = alloca %calculator, align 8
  store %calculator zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %calculator, ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %calculator, ptr %1, i32 0, i32 0
  store i32 34, ptr %3, align 4
  %4 = load %calculator, ptr %1, align 4
  store %calculator %4, ptr %"main()::calc", align 4
  %"main()::calc_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::calc_ptr", align 8
  store ptr %"main()::calc", ptr %"main()::calc_ptr", align 8
  %"main()::result" = alloca i32, align 4
  store i32 0, ptr %"main()::result", align 4
  %5 = load ptr, ptr %"main()::calc_ptr", align 8
  %is_null = icmp eq ptr %5, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %0
  %6 = call i32 @"calculator::double"(ptr %5)
  %7 = alloca i32, align 4
  store i32 %6, ptr %7, align 4
  %8 = load i32, ptr %7, align 4
  store i32 %8, ptr %"main()::result", align 4
  %9 = load i32, ptr %"main()::result", align 4
  %10 = add i32 %9, 1
  ret i32 %10
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
