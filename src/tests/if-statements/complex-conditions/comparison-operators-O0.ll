; ModuleID = 'src/tests/if-statements/complex-conditions/comparison-operators.fm'
source_filename = "src/tests/if-statements/complex-conditions/comparison-operators.fm"

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

define void @"fumo.init.for: src/tests/if-statements/complex-conditions/comparison-operators.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 0, ptr %"main()::x", align 4
  store i32 42, ptr %"main()::x", align 4
  %"main()::y" = alloca i32, align 4
  store i32 0, ptr %"main()::y", align 4
  store i32 42, ptr %"main()::y", align 4
  %"main()::z" = alloca i32, align 4
  store i32 0, ptr %"main()::z", align 4
  store i32 100, ptr %"main()::z", align 4
  %1 = load i32, ptr %"main()::x", align 4
  %2 = load i32, ptr %"main()::y", align 4
  %3 = icmp eq i32 %1, %2
  br i1 %3, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %4 = load i32, ptr %"main()::x", align 4
  %5 = load i32, ptr %"main()::z", align 4
  %6 = icmp ne i32 %4, %5
  br i1 %6, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  %7 = load i32, ptr %"main()::x", align 4
  %8 = load i32, ptr %"main()::z", align 4
  %9 = icmp slt i32 %7, %8
  br i1 %9, label %begin.if3, label %end.if4

begin.if3:                                        ; preds = %begin.if1
  %10 = load i32, ptr %"main()::x", align 4
  %11 = icmp sle i32 42, %10
  br i1 %11, label %begin.if5, label %end.if6

begin.if5:                                        ; preds = %begin.if3
  %12 = load i32, ptr %"main()::y", align 4
  %13 = icmp sle i32 %12, 50
  br i1 %13, label %begin.if7, label %end.if8

begin.if7:                                        ; preds = %begin.if5
  ret i32 1

end.if8:                                          ; preds = %begin.if5
  br label %end.if6

end.if6:                                          ; preds = %end.if8, %begin.if3
  br label %end.if4

end.if4:                                          ; preds = %end.if6, %begin.if1
  br label %end.if2

end.if2:                                          ; preds = %end.if4, %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/complex-conditions/comparison-operators.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
