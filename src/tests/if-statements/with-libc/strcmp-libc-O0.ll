; ModuleID = 'src/tests/if-statements/with-libc/strcmp-libc.fm'
source_filename = "src/tests/if-statements/with-libc/strcmp-libc.fm"

@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@.str.1 = private unnamed_addr constant [6 x i8] c"world\00", align 1
@.str.2 = private unnamed_addr constant [6 x i8] c"hello\00", align 1

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

define void @"fumo.init.for: src/tests/if-statements/with-libc/strcmp-libc.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::str1" = alloca ptr, align 8
  store ptr null, ptr %"main()::str1", align 8
  store ptr @.str, ptr %"main()::str1", align 8
  %"main()::str2" = alloca ptr, align 8
  store ptr null, ptr %"main()::str2", align 8
  store ptr @.str.1, ptr %"main()::str2", align 8
  %"main()::str3" = alloca ptr, align 8
  store ptr null, ptr %"main()::str3", align 8
  store ptr @.str.2, ptr %"main()::str3", align 8
  %"main()::var" = alloca i64, align 8
  store i64 0, ptr %"main()::var", align 4
  %1 = load ptr, ptr %"main()::str1", align 8
  %2 = call i64 @strlen(ptr %1)
  store i64 %2, ptr %"main()::var", align 4
  %3 = load ptr, ptr %"main()::str1", align 8
  %4 = load ptr, ptr %"main()::str3", align 8
  %5 = call i32 @strcmp(ptr %3, ptr %4)
  %6 = icmp eq i32 %5, 0
  br i1 %6, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %7 = load i64, ptr %"main()::var", align 4
  %8 = icmp slt i64 3, %7
  br i1 %8, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  ret i32 1

end.if2:                                          ; preds = %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  %9 = load ptr, ptr %"main()::str1", align 8
  %10 = load ptr, ptr %"main()::str2", align 8
  %11 = call i32 @strcmp(ptr %9, ptr %10)
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %begin.if3, label %end.if4

begin.if3:                                        ; preds = %end.if
  ret i32 2

end.if4:                                          ; preds = %end.if
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/with-libc/strcmp-libc.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
