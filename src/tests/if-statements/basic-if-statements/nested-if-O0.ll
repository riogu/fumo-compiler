; ModuleID = 'src/tests/if-statements/basic-if-statements/nested-if.fm'
source_filename = "src/tests/if-statements/basic-if-statements/nested-if.fm"

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

define void @"fumo.init.for: src/tests/if-statements/basic-if-statements/nested-if.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 0, ptr %"main()::x", align 4
  store i32 10, ptr %"main()::x", align 4
  %"main()::y" = alloca i32, align 4
  store i32 0, ptr %"main()::y", align 4
  store i32 20, ptr %"main()::y", align 4
  %1 = load i32, ptr %"main()::x", align 4
  %2 = icmp slt i32 0, %1
  br i1 %2, label %begin.if, label %else

begin.if:                                         ; preds = %0
  store i32 31231, ptr %"main()::x", align 4
  br label %end.if

else:                                             ; preds = %0
  %3 = load i32, ptr %"main()::y", align 4
  %4 = icmp slt i32 15, %3
  br i1 %4, label %begin.if1, label %else.if

begin.if1:                                        ; preds = %else
  ret i32 42

else.if:                                          ; preds = %else
  %5 = load i32, ptr %"main()::y", align 4
  %6 = icmp slt i32 3, %5
  br i1 %6, label %begin.if3, label %end.if2

begin.if3:                                        ; preds = %else.if
  %7 = load i32, ptr %"main()::y", align 4
  %8 = icmp slt i32 %7, 3
  br i1 %8, label %begin.if4, label %else.if5

begin.if4:                                        ; preds = %begin.if3
  store i32 123, ptr %"main()::x", align 4
  br label %end.if6

else.if5:                                         ; preds = %begin.if3
  %9 = load i32, ptr %"main()::x", align 4
  %10 = icmp slt i32 %9, 3
  br i1 %10, label %begin.if7, label %end.if6

begin.if7:                                        ; preds = %else.if5
  store i32 3, ptr %"main()::y", align 4
  br label %end.if6

end.if6:                                          ; preds = %begin.if7, %else.if5, %begin.if4
  br label %end.if2

end.if2:                                          ; preds = %end.if6, %else.if
  store i32 3, ptr %"main()::y", align 4
  br label %end.if

end.if:                                           ; preds = %end.if2, %begin.if
  store i32 43, ptr %"main()::x", align 4
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/basic-if-statements/nested-if.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
