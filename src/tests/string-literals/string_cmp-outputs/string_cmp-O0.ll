; ModuleID = 'src/tests/string-literals/string_cmp-outputs/string_cmp'
source_filename = "src/tests/string-literals/string_cmp.fm"

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

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  ret i32 0
}

attributes #0 = { "used" }
