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
  %"func()::x" = alloca i32, align 4
  store i32 69420, ptr %"func()::x", align 4
  ret void
}

define i32 @main() #1 {
  call void @fumo.init()
  ret i32 231233
}

attributes #0 = { noinline "used" }
attributes #1 = { "used" }
