; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%node = type { i32, ptr }

define void @fumo.init() #0 {
  ret void
}

define ptr @"node::get_next"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %node, ptr %2, i32 0, i32 1
  %4 = load ptr, ptr %3, align 8
  ret ptr %4
}

define void @"node::set_next"(ptr %0, ptr %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"node::set_next()::n" = alloca ptr, align 8
  store ptr %1, ptr %"node::set_next()::n", align 8
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %node, ptr %3, i32 0, i32 1
  %5 = load ptr, ptr %"node::set_next()::n", align 8
  store ptr %5, ptr %4, align 8
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::node1" = alloca %node, align 8
  %1 = alloca %node, align 8
  store %node zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 10, ptr %2, align 4
  %3 = load %node, ptr %1, align 8
  store %node %3, ptr %"main()::node1", align 8
  %"main()::node2" = alloca %node, align 8
  %4 = alloca %node, align 8
  store %node zeroinitializer, ptr %4, align 8
  %5 = getelementptr inbounds nuw %node, ptr %4, i32 0, i32 0
  store i32 20, ptr %5, align 4
  %6 = load %node, ptr %4, align 8
  store %node %6, ptr %"main()::node2", align 8
  %"main()::node3" = alloca %node, align 8
  %7 = alloca %node, align 8
  store %node zeroinitializer, ptr %7, align 8
  %8 = getelementptr inbounds nuw %node, ptr %7, i32 0, i32 0
  store i32 30, ptr %8, align 4
  %9 = load %node, ptr %7, align 8
  store %node %9, ptr %"main()::node3", align 8
  call void @"node::set_next"(ptr %"main()::node1", ptr %"main()::node2")
  call void @"node::set_next"(ptr %"main()::node2", ptr %"main()::node3")
  %"main()::x" = alloca ptr, align 8
  %10 = call ptr @"node::get_next"(ptr %"main()::node1")
  %11 = alloca ptr, align 8
  store ptr %10, ptr %11, align 8
  %12 = load ptr, ptr %11, align 8
  store ptr %12, ptr %"main()::x", align 8
  %"main()::y" = alloca ptr, align 8
  %13 = load ptr, ptr %"main()::x", align 8
  %14 = call ptr @"node::get_next"(ptr %13)
  %15 = alloca ptr, align 8
  store ptr %14, ptr %15, align 8
  %16 = load ptr, ptr %15, align 8
  store ptr %16, ptr %"main()::y", align 8
  %17 = load ptr, ptr %"main()::y", align 8
  %18 = getelementptr inbounds nuw %node, ptr %17, i32 0, i32 0
  %19 = load i32, ptr %18, align 4
  ret i32 %19
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
