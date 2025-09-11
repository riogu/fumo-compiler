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

define i32 @"calculator::triple"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = call i32 @"calculator::get_value"(ptr %2)
  %4 = mul i32 %3, 3
  ret i32 %4
}

define internal i32 @fumo.user_main() {
  %"main()::calc" = alloca %calculator, align 8
  %1 = alloca %calculator, align 8
  store %calculator zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %calculator, ptr %1, i32 0, i32 0
  store i32 34, ptr %2, align 4
  %3 = load %calculator, ptr %1, align 4
  store %calculator %3, ptr %"main()::calc", align 4
  %"main()::calc_ptr" = alloca ptr, align 8
  store ptr %"main()::calc", ptr %"main()::calc_ptr", align 8
  %"main()::result" = alloca i32, align 4
  %4 = load ptr, ptr %"main()::calc_ptr", align 8
  %5 = call i32 @"calculator::double"(ptr %4)
  %6 = alloca i32, align 4
  store i32 %5, ptr %6, align 4
  %7 = load i32, ptr %6, align 4
  store i32 %7, ptr %"main()::result", align 4
  %8 = load i32, ptr %"main()::result", align 4
  %9 = add i32 %8, 1
  ret i32 %9
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
