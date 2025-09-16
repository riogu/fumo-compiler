; ModuleID = 'src/tests/if-statements/namespaces-and-structs/struct-member-access.fm'
source_filename = "src/tests/if-statements/namespaces-and-structs/struct-member-access.fm"

%Point = type { i32, i32 }

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

define void @"fumo.init.for: src/tests/if-statements/namespaces-and-structs/struct-member-access.fm"() #0 {
  ret void
}

define i1 @"Point::is_origin"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %Point, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %logic_and.rhs, label %logic_and.end

logic_and.rhs:                                    ; preds = %1
  %6 = load ptr, ptr %this, align 8
  %7 = getelementptr inbounds nuw %Point, ptr %6, i32 0, i32 1
  %8 = load i32, ptr %7, align 4
  %9 = icmp eq i32 %8, 0
  br label %logic_and.end

logic_and.end:                                    ; preds = %logic_and.rhs, %1
  %phi.result = phi i1 [ false, %1 ], [ %9, %logic_and.rhs ]
  ret i1 %phi.result
}

define i32 @"Point::distance_from_origin"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %Point, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %Point, ptr %5, i32 0, i32 0
  %7 = load i32, ptr %6, align 4
  %8 = mul i32 %4, %7
  %9 = load ptr, ptr %this, align 8
  %10 = getelementptr inbounds nuw %Point, ptr %9, i32 0, i32 1
  %11 = load i32, ptr %10, align 4
  %12 = load ptr, ptr %this, align 8
  %13 = getelementptr inbounds nuw %Point, ptr %12, i32 0, i32 1
  %14 = load i32, ptr %13, align 4
  %15 = mul i32 %11, %14
  %16 = add i32 %8, %15
  ret i32 %16
}

define internal i32 @fumo.user_main() {
  %"main()::p" = alloca %Point, align 8
  store %Point zeroinitializer, ptr %"main()::p", align 4
  %1 = alloca %Point, align 8
  store %Point zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %Point, ptr %1, i32 0, i32 0
  store i32 3, ptr %2, align 4
  %3 = getelementptr inbounds nuw %Point, ptr %1, i32 0, i32 1
  store i32 4, ptr %3, align 4
  %4 = load %Point, ptr %1, align 4
  store %Point %4, ptr %"main()::p", align 4
  %5 = call i1 @"Point::is_origin"(ptr %"main()::p")
  %6 = alloca i1, align 1
  store i1 %5, ptr %6, align 1
  %7 = load i1, ptr %6, align 1
  %8 = xor i1 %7, true
  br i1 %8, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %9 = call i32 @"Point::distance_from_origin"(ptr %"main()::p")
  %10 = alloca i32, align 4
  store i32 %9, ptr %10, align 4
  %11 = load i32, ptr %10, align 4
  %12 = icmp slt i32 20, %11
  br i1 %12, label %begin.if1, label %else

begin.if1:                                        ; preds = %begin.if
  ret i32 1

else:                                             ; preds = %begin.if
  ret i32 2

end.if2:                                          ; No predecessors!
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/namespaces-and-structs/struct-member-access.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
