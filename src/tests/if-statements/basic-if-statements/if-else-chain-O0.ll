; ModuleID = 'src/tests/if-statements/basic-if-statements/if-else-chain.fm'
source_filename = "src/tests/if-statements/basic-if-statements/if-else-chain.fm"

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

define void @"fumo.init.for: src/tests/if-statements/basic-if-statements/if-else-chain.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::grade" = alloca i32, align 4
  store i32 0, ptr %"main()::grade", align 4
  store i32 85, ptr %"main()::grade", align 4
  %1 = load i32, ptr %"main()::grade", align 4
  %2 = icmp sle i32 90, %1
  br i1 %2, label %begin.if, label %else.if

begin.if:                                         ; preds = %0
  ret i32 4

else.if:                                          ; preds = %0
  %3 = load i32, ptr %"main()::grade", align 4
  %4 = icmp sle i32 80, %3
  br i1 %4, label %begin.if1, label %else.if2

begin.if1:                                        ; preds = %else.if
  ret i32 3

else.if2:                                         ; preds = %else.if
  %5 = load i32, ptr %"main()::grade", align 4
  %6 = icmp sle i32 70, %5
  br i1 %6, label %begin.if3, label %else

begin.if3:                                        ; preds = %else.if2
  ret i32 2

else:                                             ; preds = %else.if2
  ret i32 1

end.if:                                           ; No predecessors!
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/basic-if-statements/if-else-chain.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
