; ModuleID = 'src/tests/string-literals/func_params-outputs/func_params'
source_filename = "src/tests/string-literals/func_params.fm"

@.str = private unnamed_addr constant [14 x i8] c"Greeting: %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"Welcome!\00", align 1

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

define void @print_greeting(ptr %0) {
  %"print_greeting()::msg" = alloca ptr, align 8
  store ptr null, ptr %"print_greeting()::msg", align 8
  store ptr %0, ptr %"print_greeting()::msg", align 8
  %2 = load ptr, ptr %"print_greeting()::msg", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @.str, ptr %2)
  ret void
}

define internal i32 @fumo.user_main() {
  call void @print_greeting(ptr @.str.1)
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
