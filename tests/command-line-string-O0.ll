; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

@x = global i32 0

; Function Attrs: noinline
define void @fumo.init() #0 {
  store i32 32, ptr @x, align 4
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::y" = alloca i32, align 4
  %1 = load ptr, ptr @x, align 8
  %2 = add ptr %1, i32 1
  %3 = load ptr, ptr %2, align 8
  %4 = load ptr, ptr %3, align 8
  %5 = load i32, ptr %4, align 4
  store i32 %5, ptr %"main()::y", align 4
  %6 = load i32, ptr %"main()::y", align 4
  ret i32 %6
}

define i32 @main(i32 %argc, ptr %argv) #1 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { noinline "used" }
attributes #1 = { "used" }
