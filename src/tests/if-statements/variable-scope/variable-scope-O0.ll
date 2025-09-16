; ModuleID = 'src/tests/if-statements/variable-scope/variable-scope.fm'
source_filename = "src/tests/if-statements/variable-scope/variable-scope.fm"

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

define void @"fumo.init.for: src/tests/if-statements/variable-scope/variable-scope.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::outer" = alloca i32, align 4
  store i32 0, ptr %"main()::outer", align 4
  store i32 10, ptr %"main()::outer", align 4
  %1 = load i32, ptr %"main()::outer", align 4
  %2 = icmp slt i32 5, %1
  br i1 %2, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %"main()::0::inner" = alloca i32, align 4
  store i32 0, ptr %"main()::0::inner", align 4
  store i32 20, ptr %"main()::0::inner", align 4
  %3 = load i32, ptr %"main()::outer", align 4
  %4 = load i32, ptr %"main()::0::inner", align 4
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  %"main()::0::0::deeply_nested" = alloca i32, align 4
  store i32 0, ptr %"main()::0::0::deeply_nested", align 4
  store i32 30, ptr %"main()::0::0::deeply_nested", align 4
  %6 = load i32, ptr %"main()::0::0::deeply_nested", align 4
  ret i32 %6

end.if2:                                          ; preds = %begin.if
  %7 = load i32, ptr %"main()::0::inner", align 4
  ret i32 %7

end.if:                                           ; preds = %0
  %8 = load i32, ptr %"main()::outer", align 4
  ret i32 %8
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/variable-scope/variable-scope.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
