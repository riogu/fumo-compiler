; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%calculator = type { i32 }

define void @fumo.init() #0 {
  ret void
}

define i32 @"calculator::get_value"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %calculator, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define i32 @"calculator::double"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = call i32 @"calculator::get_value"(ptr %2)
  %4 = mul i32 %3, 2
  ret i32 %4
}

define internal void @fumo.user_main() {
  %"main()::calc" = alloca %calculator, align 8
  %1 = alloca %calculator, align 8
  store %calculator zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %calculator, ptr %1, i32 0, i32 0
  store i32 5, ptr %2, align 4
  %3 = load %calculator, ptr %1, align 4
  store %calculator %3, ptr %"main()::calc", align 4
  %"main()::result" = alloca i32, align 4
  %4 = call i32 @"calculator::double"(ptr %"main()::calc")
  %5 = alloca i32, align 4
  store i32 %4, ptr %5, align 4
  %6 = load i32, ptr %5, align 4
  store i32 %6, ptr %"main()::result", align 4
  ret void
}

define i32 @main(i32 %argc, ptr %argv) #0 {
entry:
  call void @fumo.init()
  call void @fumo.user_main()
  ret i32 0
}

attributes #0 = { "used" }
