; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%node = type { i32, ptr, ptr }

define void @fumo.init() #0 {
  ret void
}

define %node @"node::get_next"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %node, ptr %2, i32 0, i32 2
  %4 = load ptr, ptr %3, align 8
  %5 = load %node, ptr %4, align 8
  ret %node %5
}

define void @"node::set_next"(ptr %0, ptr %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"node::set_next()::n" = alloca ptr, align 8
  store ptr null, ptr %"node::set_next()::n", align 8
  store ptr %1, ptr %"node::set_next()::n", align 8
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %node, ptr %3, i32 0, i32 2
  %5 = load ptr, ptr %"node::set_next()::n", align 8
  store ptr %5, ptr %4, align 8
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::nodea" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::nodea", align 8
  %"main()::node1" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node1", align 8
  %1 = alloca %node, align 8
  store %node zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 2
  store ptr null, ptr %3, align 8
  %4 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 10, ptr %4, align 4
  %5 = load %node, ptr %1, align 8
  store %node %5, ptr %"main()::node1", align 8
  %6 = getelementptr inbounds nuw %node, ptr %"main()::node1", i32 0, i32 2
  %7 = load ptr, i32 1, align 8
  store ptr %7, ptr %6, align 8
  %"main()::node2" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node2", align 8
  %8 = alloca %node, align 8
  store %node zeroinitializer, ptr %8, align 8
  %9 = getelementptr inbounds nuw %node, ptr %8, i32 0, i32 0
  store i32 0, ptr %9, align 4
  %10 = getelementptr inbounds nuw %node, ptr %8, i32 0, i32 2
  store ptr null, ptr %10, align 8
  %11 = getelementptr inbounds nuw %node, ptr %8, i32 0, i32 0
  store i32 20, ptr %11, align 4
  %12 = load %node, ptr %8, align 8
  store %node %12, ptr %"main()::node2", align 8
  %"main()::node3" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node3", align 8
  %13 = alloca %node, align 8
  store %node zeroinitializer, ptr %13, align 8
  %14 = getelementptr inbounds nuw %node, ptr %13, i32 0, i32 0
  store i32 0, ptr %14, align 4
  %15 = getelementptr inbounds nuw %node, ptr %13, i32 0, i32 2
  store ptr null, ptr %15, align 8
  %16 = getelementptr inbounds nuw %node, ptr %13, i32 0, i32 0
  store i32 30, ptr %16, align 4
  %17 = load %node, ptr %13, align 8
  store %node %17, ptr %"main()::node3", align 8
  call void @"node::set_next"(ptr %"main()::node1", ptr %"main()::node2")
  call void @"node::set_next"(ptr %"main()::node2", ptr %"main()::node3")
  %18 = call %node @"node::get_next"(ptr %"main()::node1")
  %19 = alloca %node, align 8
  store %node %18, ptr %19, align 8
  %20 = getelementptr inbounds nuw %node, ptr %19, i32 0, i32 1
  %21 = load ptr, ptr %20, align 8
  store i32 69, ptr %21, align 4
  %22 = call %node @"node::get_next"(ptr %"main()::node1")
  %23 = alloca %node, align 8
  store %node %22, ptr %23, align 8
  %24 = getelementptr inbounds nuw %node, ptr %23, i32 0, i32 1
  %25 = load ptr, ptr %24, align 8
  %26 = load i32, ptr %25, align 4
  ret i32 %26
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
