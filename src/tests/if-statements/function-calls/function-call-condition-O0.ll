; ModuleID = 'src/tests/if-statements/function-calls/function-call-condition.fm'
source_filename = "src/tests/if-statements/function-calls/function-call-condition.fm"

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

define void @"fumo.init.for: src/tests/if-statements/function-calls/function-call-condition.fm"() #0 {
  ret void
}

define i1 @is_positive(i32 %0) {
  %"is_positive()::x" = alloca i32, align 4
  store i32 0, ptr %"is_positive()::x", align 4
  store i32 %0, ptr %"is_positive()::x", align 4
  %2 = load i32, ptr %"is_positive()::x", align 4
  %3 = icmp slt i32 0, %2
  ret i1 %3
}

define i64 @get_value() {
  ret i64 42
}

define internal i32 @fumo.user_main() {
  %1 = call i64 @get_value()
  %2 = trunc i64 %1 to i32
  %3 = call i1 @is_positive(i32 %2)
  br i1 %3, label %begin.if, label %else

begin.if:                                         ; preds = %0
  ret i32 1

else:                                             ; preds = %0
  ret i32 0

end.if:                                           ; No predecessors!
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/function-calls/function-call-condition.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
