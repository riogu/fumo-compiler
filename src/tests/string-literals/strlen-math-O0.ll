; ModuleID = 'src/tests/string-literals/strlen-math.fm'
source_filename = "src/tests/string-literals/strlen-math.fm"

@.str = private unnamed_addr constant [12 x i8] c"Programming\00", align 1
@.str.1 = private unnamed_addr constant [22 x i8] c"Length of '%s' is %d\0A\00", align 1

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

define void @"fumo.init.for: src/tests/string-literals/strlen-math.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::message" = alloca ptr, align 8
  store ptr null, ptr %"main()::message", align 8
  store ptr @.str, ptr %"main()::message", align 8
  %"main()::len" = alloca i32, align 4
  store i32 0, ptr %"main()::len", align 4
  %1 = load ptr, ptr %"main()::message", align 8
  %2 = call i64 @strlen(ptr %1)
  store i64 %2, ptr %"main()::len", align 4
  %3 = load ptr, ptr %"main()::message", align 8
  %4 = load i32, ptr %"main()::len", align 4
  %5 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr %3, i32 %4)
  %6 = load i32, ptr %"main()::len", align 4
  ret i32 %6
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/string-literals/strlen-math.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
