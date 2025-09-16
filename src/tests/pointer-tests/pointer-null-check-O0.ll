; ModuleID = 'src/tests/pointer-tests/pointer-null-check.fm'
source_filename = "src/tests/pointer-tests/pointer-null-check.fm"

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

define void @"fumo.init.for: src/tests/pointer-tests/pointer-null-check.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::ptr", align 8
  %"main()::value" = alloca i32, align 4
  store i32 0, ptr %"main()::value", align 4
  store i32 42, ptr %"main()::value", align 4
  %"main()::valid_ptr" = alloca ptr, align 8
  store ptr null, ptr %"main()::valid_ptr", align 8
  store ptr %"main()::value", ptr %"main()::valid_ptr", align 8
  %1 = load ptr, ptr %"main()::ptr", align 8
  %tobool = icmp ne ptr %1, null
  br i1 %tobool, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %2 = load ptr, ptr %"main()::valid_ptr", align 8
  %tobool1 = icmp ne ptr %2, null
  br i1 %tobool1, label %begin.if2, label %end.if3

begin.if2:                                        ; preds = %begin.if
  ret i32 1

end.if3:                                          ; preds = %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if3, %0
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/pointer-tests/pointer-null-check.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
