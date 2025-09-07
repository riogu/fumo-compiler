; ModuleID = 'tests/command-line-string.fm'
source_filename = "command-line-string.fm"

@x = global i32 0
@y = global i32 0

; Function Attrs: noinline
define void @fumo.init() #0 {
  store i32 123123, ptr @y, align 4
  ret void
}

define void @func() {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 213, ptr %"main()::x", align 4
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
