; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

define void @fumo.init() #0 {
  ret void
}

define i32 @func(ptr %0) {
  %"func()::a" = alloca ptr, align 8
  store ptr %0, ptr %"func()::a", align 8
  %2 = load ptr, ptr %"func()::a", align 8
  %3 = load i32, ptr %2, align 4
  ret i32 %3
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 69, ptr %"main()::x", align 4
  %"main()::y" = alloca i32, align 4
  %1 = call i32 @func(ptr %"main()::x")
  store i32 %1, ptr %"main()::y", align 4
  %2 = load i32, ptr %"main()::y", align 4
  ret i32 %2
}

define i32 @main(i32 %argc, ptr %argv) #0 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { "used" }
