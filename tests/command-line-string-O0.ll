; ModuleID = 'tests/command-line-string.fm'
source_filename = "command-line-string.fm"

@x = global i32 0
@y = global i32 0

define internal void @fumo._start() {
  store i32 123123, ptr @y, align 4
  ret void
}

define i32 @main() {
  call void @fumo._start()
  %"main()::x" = alloca i32, align 4
  store i32 32, ptr %"main()::x", align 4
  %1 = load i32, ptr %"main()::x", align 4
  %2 = add i32 %1, 3
  store i32 %2, ptr @y, align 4
  ret i32 0
}
