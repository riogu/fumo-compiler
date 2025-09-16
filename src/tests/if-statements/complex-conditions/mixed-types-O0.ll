; ModuleID = 'src/tests/if-statements/complex-conditions/mixed-types.fm'
source_filename = "src/tests/if-statements/complex-conditions/mixed-types.fm"

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

define void @"fumo.init.for: src/tests/if-statements/complex-conditions/mixed-types.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::int_val" = alloca i32, align 4
  store i32 0, ptr %"main()::int_val", align 4
  store i32 42, ptr %"main()::int_val", align 4
  %"main()::float_val" = alloca double, align 8
  store double 0.000000e+00, ptr %"main()::float_val", align 8
  store float 0x40091EB860000000, ptr %"main()::float_val", align 4
  %"main()::bool_val" = alloca i1, align 1
  store i1 false, ptr %"main()::bool_val", align 1
  store i1 true, ptr %"main()::bool_val", align 1
  %1 = load i32, ptr %"main()::int_val", align 4
  %2 = icmp slt i32 40, %1
  br i1 %2, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %3 = load double, ptr %"main()::float_val", align 8
  %4 = fcmp olt double 3.000000e+00, %3
  br i1 %4, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  %5 = load i1, ptr %"main()::bool_val", align 1
  br i1 %5, label %begin.if3, label %end.if4

begin.if3:                                        ; preds = %begin.if1
  ret i32 1

end.if4:                                          ; preds = %begin.if1
  br label %end.if2

end.if2:                                          ; preds = %end.if4, %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/complex-conditions/mixed-types.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
