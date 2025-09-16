; ModuleID = 'src/tests/if-statements/namespaces-and-structs/namespace-access.fm'
source_filename = "src/tests/if-statements/namespaces-and-structs/namespace-access.fm"

@"math::pi" = global double 0.000000e+00

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

define void @"fumo.init.for: src/tests/if-statements/namespaces-and-structs/namespace-access.fm"() #0 {
  store float 0x400921FA00000000, ptr @"math::pi", align 4
  ret void
}

define i32 @"math::abs"(i32 %0) {
  %"math::abs()::x" = alloca i32, align 4
  store i32 0, ptr %"math::abs()::x", align 4
  store i32 %0, ptr %"math::abs()::x", align 4
  %2 = load i32, ptr %"math::abs()::x", align 4
  %3 = icmp slt i32 %2, 0
  br i1 %3, label %begin.if, label %end.if

begin.if:                                         ; preds = %1
  %4 = load i32, ptr %"math::abs()::x", align 4
  %5 = sub i32 0, %4
  ret i32 %5

end.if:                                           ; preds = %1
  %6 = load i32, ptr %"math::abs()::x", align 4
  ret i32 %6
}

define internal i32 @fumo.user_main() {
  %"main()::value" = alloca i32, align 4
  store i32 0, ptr %"main()::value", align 4
  store i32 -42, ptr %"main()::value", align 4
  %1 = load i32, ptr %"main()::value", align 4
  %2 = call i32 @"math::abs"(i32 %1)
  %3 = icmp slt i32 40, %2
  br i1 %3, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %4 = load double, ptr @"math::pi", align 8
  %5 = fcmp olt double 3.000000e+00, %4
  br i1 %5, label %begin.if1, label %end.if2

begin.if1:                                        ; preds = %begin.if
  ret i32 1

end.if2:                                          ; preds = %begin.if
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/namespaces-and-structs/namespace-access.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
