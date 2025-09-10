; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

define void @fumo.init() #0 {
  ret void
}

; fn main() -> i32 {
;     let x = 69;
;     let z = &x;
;     let y = &z;
;     **y = 140;
;     let b = **y;
;     return b;
; }
define internal i32 @fumo.user_main() {
  %"main()::x" = alloca i32, align 4
  store i32 69, ptr %"main()::x", align 4
  %"main()::z" = alloca ptr, align 8
  store ptr %"main()::x", ptr %"main()::z", align 8
  %"main()::y" = alloca ptr, align 8
  store ptr %"main()::z", ptr %"main()::y", align 8

  %1 = load ptr, ptr %"main()::y", align 8
  %2 = load ptr, ptr %1, align 8
  store i32 140, ptr %2, align 4

  %"main()::b" = alloca i32, align 4

  %3 = load ptr, ptr %"main()::y", align 8
  %4 = load ptr, ptr %3, align 8
  %5 = load i32, ptr %4, align 4
  store i32 %5, ptr %"main()::b", align 4


  %6 = load i32, ptr %"main()::b", align 4
  ret i32 %6
}

define i32 @main(i32 %argc, ptr %argv) #0 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { "used" }
