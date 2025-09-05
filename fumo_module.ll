; ModuleID = 'fumo_module.fm'
source_filename = "fumo_module.fm"

@x = external global i32

define internal void @fumo._start() {
  store i32 23, ptr @x, align 4
  ret void
}

define i32 @main() {
  call void @fumo._start()
  %1 = load i32, ptr @x, align 4
  %2 = load i32, ptr @x, align 4
  %3 = mul i32 %2, 69
  store i32 %3, i32 %1, align 4
  %"main()::y" = alloca i32, align 4
  %4 = load i32, ptr @x, align 4
  %5 = add i32 %4, 43
  %6 = sdiv i32 %5, 23
  %7 = load i32, ptr @x, align 4
  %8 = mul i32 %6, %7
  store i32 %8, ptr %"main()::y", align 4
}

