; ModuleID = 'src/tests/if-statements/edge-cases/empty-blocks.fm'
source_filename = "src/tests/if-statements/edge-cases/empty-blocks.fm"

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

define void @"fumo.init.for: src/tests/if-statements/edge-cases/empty-blocks.fm"() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 0, ptr %"main()::x", align 4
  store i32 42, ptr %"main()::x", align 4
  %1 = load i32, ptr %"main()::x", align 4
  %2 = icmp slt i32 0, %1
  br i1 %2, label %begin.if, label %else

begin.if:                                         ; preds = %0
  br label %end.if

else:                                             ; preds = %0
  ret i32 1

end.if:                                           ; preds = %begin.if
  %3 = load i32, ptr %"main()::x", align 4
  %4 = icmp slt i32 %3, 0
  br i1 %4, label %begin.if1, label %else2

begin.if1:                                        ; preds = %end.if
  ret i32 2

else2:                                            ; preds = %end.if
  br label %end.if3

end.if3:                                          ; preds = %else2
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/edge-cases/empty-blocks.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
