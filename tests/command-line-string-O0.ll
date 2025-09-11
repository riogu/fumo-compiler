; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%counter = type { i32 }

define void @fumo.init() #0 {
  ret void
}

declare i32 @"counter::increment"(ptr)

define i32 @"counter::get_count"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %counter, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define ptr @"counter::get_count_ptr"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %counter, ptr %2, i32 0, i32 0
  ret ptr %3
}

define void @"counter::gaming"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %counter, ptr %2, i32 0, i32 0
  %4 = load ptr, ptr %this, align 8
  %5 = getelementptr inbounds nuw %counter, ptr %4, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = add i32 %6, 1
  store i32 %7, ptr %3, align 4
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::c" = alloca %counter, align 8
  %1 = alloca %counter, align 8
  store %counter zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %counter, ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = load %counter, ptr %1, align 4
  store %counter %3, ptr %"main()::c", align 4
  %"main()::c_ptr" = alloca ptr, align 8
  store ptr %"main()::c", ptr %"main()::c_ptr", align 8
  %4 = load ptr, ptr %"main()::c_ptr", align 8
  %5 = call i32 @"counter::increment"(ptr %4)
  %6 = alloca i32, align 4
  store i32 %5, ptr %6, align 4
  %7 = load i32, ptr %6, align 4
  %"main()::count_ptr" = alloca ptr, align 8
  %8 = load ptr, ptr %"main()::c_ptr", align 8
  %9 = call ptr @"counter::get_count_ptr"(ptr %8)
  %10 = alloca ptr, align 8
  store ptr %9, ptr %10, align 8
  %11 = load ptr, ptr %10, align 8
  store ptr %11, ptr %"main()::count_ptr", align 8
  %12 = load ptr, ptr %"main()::count_ptr", align 8
  %13 = load i32, ptr %12, align 4
  ret i32 %13
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
