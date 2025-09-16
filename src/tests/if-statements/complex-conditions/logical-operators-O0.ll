; ModuleID = 'src/tests/if-statements/complex-conditions/logical-operators.fm'
source_filename = "src/tests/if-statements/complex-conditions/logical-operators.fm"

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

define void @"fumo.init.for: src/tests/if-statements/complex-conditions/logical-operators.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::a" = alloca i32, align 4
  store i32 0, ptr %"main()::a", align 4
  store i32 5, ptr %"main()::a", align 4
  %"main()::b" = alloca i32, align 4
  store i32 0, ptr %"main()::b", align 4
  store i32 10, ptr %"main()::b", align 4
  %"main()::c" = alloca i32, align 4
  store i32 0, ptr %"main()::c", align 4
  store i32 15, ptr %"main()::c", align 4
  %1 = load i32, ptr %"main()::a", align 4
  %2 = load i32, ptr %"main()::b", align 4
  %3 = icmp slt i32 %1, %2
  br i1 %3, label %logic_and.rhs, label %logic_and.end

logic_and.rhs:                                    ; preds = %0
  %4 = load i32, ptr %"main()::b", align 4
  %5 = load i32, ptr %"main()::c", align 4
  %6 = icmp slt i32 %4, %5
  br label %logic_and.end

logic_and.end:                                    ; preds = %logic_and.rhs, %0
  %phi.result = phi i1 [ false, %0 ], [ %6, %logic_and.rhs ]
  br i1 %phi.result, label %begin.if, label %end.if

begin.if:                                         ; preds = %logic_and.end
  ret i32 1

end.if:                                           ; preds = %logic_and.end
  %7 = load i32, ptr %"main()::b", align 4
  %8 = load i32, ptr %"main()::a", align 4
  %9 = icmp slt i32 %7, %8
  br i1 %9, label %logic_or.end, label %logic_or.rhs

logic_or.rhs:                                     ; preds = %end.if
  %10 = load i32, ptr %"main()::b", align 4
  %11 = load i32, ptr %"main()::c", align 4
  %12 = icmp slt i32 %10, %11
  br label %logic_or.end

logic_or.end:                                     ; preds = %logic_or.rhs, %end.if
  %phi.result1 = phi i1 [ true, %end.if ], [ %12, %logic_or.rhs ]
  br i1 %phi.result1, label %begin.if2, label %end.if3

begin.if2:                                        ; preds = %logic_or.end
  ret i32 2

end.if3:                                          ; preds = %logic_or.end
  %13 = load i32, ptr %"main()::b", align 4
  %14 = load i32, ptr %"main()::a", align 4
  %15 = icmp slt i32 %13, %14
  %16 = xor i1 %15, true
  br i1 %16, label %begin.if4, label %end.if5

begin.if4:                                        ; preds = %end.if3
  ret i32 3

end.if5:                                          ; preds = %end.if3
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/complex-conditions/logical-operators.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
