; ModuleID = 'src/tests/if-statements/basic-if-statements/many-basic-ifs.fm'
source_filename = "src/tests/if-statements/basic-if-statements/many-basic-ifs.fm"

@x = global i32 0
@y = global i32 0
@z = global i32 0
@result = global i32 0
@a = global i32 0
@b = global i32 0
@flag = global i1 false
@condition = global i1 false
@outer_condition = global i1 false
@inner_condition = global i1 false

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

define void @"fumo.init.for: src/tests/if-statements/basic-if-statements/many-basic-ifs.fm"() #0 {
  store i1 false, ptr @inner_condition, align 1
  store i1 true, ptr @outer_condition, align 1
  store i1 true, ptr @condition, align 1
  store i1 true, ptr @flag, align 1
  ret void
}

define internal void @fumo.user_main() {
  %1 = load i32, ptr @x, align 4
  %2 = icmp slt i32 0, %1
  br i1 %2, label %logic_and.rhs, label %logic_and.end

logic_and.rhs:                                    ; preds = %0
  %3 = load i32, ptr @y, align 4
  %4 = icmp slt i32 0, %3
  br label %logic_and.end

logic_and.end:                                    ; preds = %logic_and.rhs, %0
  %phi.result = phi i1 [ false, %0 ], [ %4, %logic_and.rhs ]
  br i1 %phi.result, label %begin.if, label %end.if

begin.if:                                         ; preds = %logic_and.end
  store i32 1, ptr @result, align 4
  br label %end.if

end.if:                                           ; preds = %begin.if, %logic_and.end
  %5 = load i32, ptr @x, align 4
  %6 = icmp slt i32 %5, 0
  br i1 %6, label %logic_or.end, label %logic_or.rhs

logic_or.rhs:                                     ; preds = %end.if
  %7 = load i32, ptr @y, align 4
  %8 = icmp slt i32 %7, 0
  br label %logic_or.end

logic_or.end:                                     ; preds = %logic_or.rhs, %end.if
  %phi.result1 = phi i1 [ true, %end.if ], [ %8, %logic_or.rhs ]
  br i1 %phi.result1, label %begin.if2, label %end.if3

begin.if2:                                        ; preds = %logic_or.end
  store i32 -1, ptr @result, align 4
  br label %end.if3

end.if3:                                          ; preds = %begin.if2, %logic_or.end
  %9 = load i1, ptr @flag, align 1
  %10 = xor i1 %9, true
  br i1 %10, label %begin.if4, label %end.if5

begin.if4:                                        ; preds = %end.if3
  store i32 0, ptr @result, align 4
  br label %end.if5

end.if5:                                          ; preds = %begin.if4, %end.if3
  %11 = load i32, ptr @x, align 4
  %12 = icmp slt i32 5, %11
  br i1 %12, label %logic_and.rhs8, label %logic_and.end9

logic_or.rhs6:                                    ; preds = %logic_and.end9
  %13 = load i1, ptr @flag, align 1
  br label %logic_or.end7

logic_or.end7:                                    ; preds = %logic_or.rhs6, %logic_and.end9
  %phi.result11 = phi i1 [ true, %logic_and.end9 ], [ %13, %logic_or.rhs6 ]
  br i1 %phi.result11, label %begin.if12, label %end.if13

logic_and.rhs8:                                   ; preds = %end.if5
  %14 = load i32, ptr @y, align 4
  %15 = icmp slt i32 %14, 10
  br label %logic_and.end9

logic_and.end9:                                   ; preds = %logic_and.rhs8, %end.if5
  %phi.result10 = phi i1 [ false, %end.if5 ], [ %15, %logic_and.rhs8 ]
  br i1 %phi.result10, label %logic_or.end7, label %logic_or.rhs6

begin.if12:                                       ; preds = %logic_or.end7
  store i32 42, ptr @result, align 4
  br label %end.if13

end.if13:                                         ; preds = %begin.if12, %logic_or.end7
  br i1 true, label %begin.if14, label %end.if15

begin.if14:                                       ; preds = %end.if13
  store i32 1, ptr @x, align 4
  store i32 2, ptr @y, align 4
  %16 = load i32, ptr @x, align 4
  %17 = load i32, ptr @y, align 4
  %18 = add i32 %16, %17
  store i32 %18, ptr @z, align 4
  br label %end.if15

end.if15:                                         ; preds = %begin.if14, %end.if13
  %19 = load i1, ptr @condition, align 1
  br i1 %19, label %begin.if16, label %else

begin.if16:                                       ; preds = %end.if15
  store i32 1, ptr @a, align 4
  store i32 2, ptr @b, align 4
  br label %end.if17

else:                                             ; preds = %end.if15
  store i32 3, ptr @a, align 4
  store i32 4, ptr @b, align 4
  br label %end.if17

end.if17:                                         ; preds = %else, %begin.if16
  %20 = load i32, ptr @x, align 4
  %21 = icmp eq i32 %20, 1
  br i1 %21, label %begin.if18, label %else.if

begin.if18:                                       ; preds = %end.if17
  store i32 1, ptr @result, align 4
  br label %end.if19

else.if:                                          ; preds = %end.if17
  %22 = load i32, ptr @x, align 4
  %23 = icmp eq i32 %22, 2
  br i1 %23, label %begin.if20, label %else.if21

begin.if20:                                       ; preds = %else.if
  store i32 2, ptr @result, align 4
  br label %end.if19

else.if21:                                        ; preds = %else.if
  %24 = load i32, ptr @x, align 4
  %25 = icmp eq i32 %24, 3
  br i1 %25, label %begin.if22, label %else23

begin.if22:                                       ; preds = %else.if21
  store i32 3, ptr @result, align 4
  br label %end.if19

else23:                                           ; preds = %else.if21
  store i32 99, ptr @result, align 4
  br label %end.if19

end.if19:                                         ; preds = %else23, %begin.if22, %begin.if20, %begin.if18
  %26 = load i1, ptr @outer_condition, align 1
  br i1 %26, label %begin.if24, label %end.if25

begin.if24:                                       ; preds = %end.if19
  %27 = load i1, ptr @inner_condition, align 1
  br i1 %27, label %begin.if26, label %else27

begin.if26:                                       ; preds = %begin.if24
  store i32 1, ptr @x, align 4
  br label %end.if28

else27:                                           ; preds = %begin.if24
  store i32 2, ptr @x, align 4
  br label %end.if28

end.if28:                                         ; preds = %else27, %begin.if26
  br label %end.if25

end.if25:                                         ; preds = %end.if28, %end.if19
  ret void
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/basic-if-statements/many-basic-ifs.fm"()
  call void @fumo.user_main()
  ret i32 0
}

attributes #0 = { "used" }
