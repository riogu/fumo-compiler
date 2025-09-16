; ModuleID = 'src/tests/if-statements/with-libc/libc-math.fm'
source_filename = "src/tests/if-statements/with-libc/libc-math.fm"

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

define void @"fumo.init.for: src/tests/if-statements/with-libc/libc-math.fm"() #0 {
  ret void
}

declare i32 @abs(i32)

declare double @fabs(double)

define internal i32 @fumo.user_main() {
  %"main()::negative" = alloca i32, align 4
  store i32 0, ptr %"main()::negative", align 4
  store i32 -42, ptr %"main()::negative", align 4
  %"main()::float_val" = alloca double, align 8
  store double 0.000000e+00, ptr %"main()::float_val", align 8
  store float 0xC0091EB860000000, ptr %"main()::float_val", align 4
  %1 = load i32, ptr %"main()::negative", align 4
  %2 = call i32 @abs(i32 %1)
  %3 = icmp slt i32 40, %2
  br i1 %3, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %4 = load double, ptr %"main()::float_val", align 8
  %5 = call double @fabs(double %4)
  %6 = fcmp olt double 3.000000e+00, %5
  br i1 %6, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  ret i32 1

end.if2:                                          ; preds = %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/with-libc/libc-math.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
