; ModuleID = 'src/tests/string-literals/escaped-string-test.fm'
source_filename = "src/tests/string-literals/escaped-string-test.fm"

@.str = private unnamed_addr constant [24 x i8] c"She said \22Hello\22 to me.\00", align 1

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

define void @"fumo.init.for: src/tests/string-literals/escaped-string-test.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::quoted" = alloca ptr, align 8
  store ptr null, ptr %"main()::quoted", align 8
  store ptr @.str, ptr %"main()::quoted", align 8
  %1 = load ptr, ptr %"main()::quoted", align 8
  %2 = call i32 @puts(ptr %1)
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/string-literals/escaped-string-test.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
