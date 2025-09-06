; ModuleID = 'tests/command-line-string.fm'
source_filename = "command-line-string.fm"

@x = global i32 0
@y = global i32 0

; Function Attrs: noinline
define void @fumo._start() #0 {
  store i32 123123, ptr @y, align 4
  ret void
}

define void @func() {
  %"func()::x" = alloca i32, align 4
  store i32 69420, ptr %"func()::x", align 4
  ret void
}

define i32 @main() {
  call void @fumo._start()
  ret i32 123
}

attributes #0 = { noinline "used" }
