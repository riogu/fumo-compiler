; ModuleID = 'src/tests/if-statements/function-calls/multiple-func-calls.fm'
source_filename = "src/tests/if-statements/function-calls/multiple-func-calls.fm"

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

define void @"fumo.init.for: src/tests/if-statements/function-calls/multiple-func-calls.fm"() #0 {
  ret void
}

define i32 @add(i32 %0, i32 %1) {
  %"add()::a" = alloca i32, align 4
  store i32 0, ptr %"add()::a", align 4
  store i32 %0, ptr %"add()::a", align 4
  %"add()::b" = alloca i32, align 4
  store i32 0, ptr %"add()::b", align 4
  store i32 %1, ptr %"add()::b", align 4
  %3 = load i32, ptr %"add()::a", align 4
  %4 = load i32, ptr %"add()::b", align 4
  %5 = add i32 %3, %4
  ret i32 %5
}

define i32 @multiply(i32 %0, i32 %1) {
  %"multiply()::a" = alloca i32, align 4
  store i32 0, ptr %"multiply()::a", align 4
  store i32 %0, ptr %"multiply()::a", align 4
  %"multiply()::b" = alloca i32, align 4
  store i32 0, ptr %"multiply()::b", align 4
  store i32 %1, ptr %"multiply()::b", align 4
  %3 = load i32, ptr %"multiply()::a", align 4
  %4 = load i32, ptr %"multiply()::b", align 4
  %5 = mul i32 %3, %4
  ret i32 %5
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 0, ptr %"main()::x", align 4
  store i32 5, ptr %"main()::x", align 4
  %"main()::y" = alloca i32, align 4
  store i32 0, ptr %"main()::y", align 4
  store i32 3, ptr %"main()::y", align 4
  %1 = load i32, ptr %"main()::x", align 4
  %2 = call i32 @multiply(i32 %1, i32 2)
  %3 = load i32, ptr %"main()::x", align 4
  %4 = load i32, ptr %"main()::y", align 4
  %5 = call i32 @add(i32 %3, i32 %4)
  %6 = icmp slt i32 %2, %5
  br i1 %6, label %begin.if, label %else

begin.if:                                         ; preds = %0
  ret i32 1

else:                                             ; preds = %0
  ret i32 0

end.if:                                           ; No predecessors!
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/function-calls/multiple-func-calls.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
