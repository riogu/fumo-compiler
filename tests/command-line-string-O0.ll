; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

@y = global i32 0
@x = global i32 0

; Function Attrs: noinline
define void @fumo.init() #0 {
  store i32 123123, ptr @y, align 4
  ret void
}

define i32 @func(i32 %0) {
  ret i32 123
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 32, ptr %"main()::x", align 4
  %1 = load i32, ptr %"main()::x", align 4
  %2 = add i32 %1, 3
  store i32 %2, ptr @y, align 4
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #1 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { noinline "used" }
attributes #1 = { "used" }
