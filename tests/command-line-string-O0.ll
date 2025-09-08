; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

@x = global i32 0

; Function Attrs: noinline
define void @fumo.init() #0 {
  store i32 5002, ptr @x, align 4
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::ptr" = alloca ptr, align 8
  store ptr @x, ptr %"main()::ptr", align 8
  %"main()::ptr2" = alloca ptr, align 8
  store ptr %"main()::ptr", ptr %"main()::ptr2", align 8
  %1 = load ptr, ptr %"main()::ptr2", align 8
  %2 = load i32, ptr %1, align 4
  ret i32 %2
}

define i32 @main(i32 %argc, ptr %argv) #1 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { noinline "used" }
attributes #1 = { "used" }
