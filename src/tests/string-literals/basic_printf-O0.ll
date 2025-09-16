; ModuleID = 'src/tests/string-literals/basic_printf.fm'
source_filename = "src/tests/string-literals/basic_printf.fm"

@.str = private unnamed_addr constant [6 x i8] c"fumos\00", align 1
@.str.1 = private unnamed_addr constant [14 x i8] c"i have %d %s\0A\00", align 1

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

define void @"fumo.init.for: src/tests/string-literals/basic_printf.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::fumofumo" = alloca ptr, align 8
  store ptr null, ptr %"main()::fumofumo", align 8
  store ptr @.str, ptr %"main()::fumofumo", align 8
  %"main()::amount" = alloca i32, align 4
  store i32 0, ptr %"main()::amount", align 4
  store i32 69, ptr %"main()::amount", align 4
  %1 = load i32, ptr %"main()::amount", align 4
  %2 = load ptr, ptr %"main()::fumofumo", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %1, ptr %2)
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/string-literals/basic_printf.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
