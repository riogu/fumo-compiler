; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

@x = global i32 0
@y = global ptr null

; Function Attrs: noinline
define void @fumo.init() #0 {
  store i32 23, ptr @x, align 4
  store ptr @x, ptr @y, align 8
  ret void
}

define internal i32 @fumo.user_main() {
  ret i32 123
}

define i32 @main(i32 %argc, ptr %argv) #1 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { noinline "used" }
attributes #1 = { "used" }
