; ModuleID = 'src/tests/string-literals/empty-string-outputs/empty-string'
source_filename = "src/tests/string-literals/empty-string.fm"

@.str = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"Empty string length: %d\0A\00", align 1

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

define internal i32 @fumo.user_main() {
  %"main()::empty" = alloca ptr, align 8
  store ptr null, ptr %"main()::empty", align 8
  store ptr @.str, ptr %"main()::empty", align 8
  %1 = load ptr, ptr %"main()::empty", align 8
  %2 = call i64 @strlen(ptr %1)
  %3 = call i32 (ptr, ...) @printf(ptr @.str.1, i64 %2)
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
