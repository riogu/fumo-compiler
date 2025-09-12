; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%node = type { i32, ptr }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [217 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'command-line-string.fm' at line 15:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                            ^ found null pointer dereference\0A\0A\00", align 1
@2 = private unnamed_addr constant [229 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'command-line-string.fm' at line 15:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                                        ^ found null pointer dereference\0A\0A\00", align 1
@3 = private unnamed_addr constant [241 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'command-line-string.fm' at line 15:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                                                    ^ found null pointer dereference\0A\0A\00", align 1

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
  store ptr null, ptr %"node::set_next()::n", align 8
  store ptr %1, ptr %"node::set_next()::n", align 8
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %node, ptr %3, i32 0, i32 1
  %5 = load ptr, ptr %"node::set_next()::n", align 8
  store ptr %5, ptr %4, align 8
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::var" = alloca i32, align 4
  store i32 0, ptr %"main()::var", align 4
  store i32 123, ptr %"main()::var", align 4
  %"main()::node" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node", align 8
  %"main()::node1" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node1", align 8
  %1 = alloca %node, align 8
  store %node zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 1
  store ptr null, ptr %3, align 8
  %4 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 5, ptr %4, align 4
  %5 = load %node, ptr %1, align 8
  store %node %5, ptr %"main()::node1", align 8
  %"main()::node2" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node2", align 8
  %6 = alloca %node, align 8
  store %node zeroinitializer, ptr %6, align 8
  %7 = getelementptr inbounds nuw %node, ptr %6, i32 0, i32 0
  store i32 0, ptr %7, align 4
  %8 = getelementptr inbounds nuw %node, ptr %6, i32 0, i32 1
  store ptr null, ptr %8, align 8
  %9 = getelementptr inbounds nuw %node, ptr %6, i32 0, i32 0
  store i32 20, ptr %9, align 4
  %10 = load %node, ptr %6, align 8
  store %node %10, ptr %"main()::node2", align 8
  %"main()::node3" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node3", align 8
  %11 = alloca %node, align 8
  store %node zeroinitializer, ptr %11, align 8
  %12 = getelementptr inbounds nuw %node, ptr %11, i32 0, i32 0
  store i32 0, ptr %12, align 4
  %13 = getelementptr inbounds nuw %node, ptr %11, i32 0, i32 1
  store ptr null, ptr %13, align 8
  %14 = getelementptr inbounds nuw %node, ptr %11, i32 0, i32 0
  store i32 30, ptr %14, align 4
  %15 = load %node, ptr %11, align 8
  store %node %15, ptr %"main()::node3", align 8
  call void @"node::set_next"(ptr %"main()::node1", ptr %"main()::node2")
  call void @"node::set_next"(ptr %"main()::node2", ptr %"main()::node3")
  %16 = call ptr @"node::get_next"(ptr %"main()::node1")
  %is_null = icmp eq ptr %16, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @1)
  unreachable

safe_deref:                                       ; preds = %0
  %17 = call ptr @"node::get_next"(ptr %16)
  %is_null3 = icmp eq ptr %17, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @2)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %18 = call ptr @"node::get_next"(ptr %17)
  %is_null6 = icmp eq ptr %18, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @3)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %19 = getelementptr inbounds nuw %node, ptr %18, i32 0, i32 0
  %20 = load i32, ptr %19, align 4
  ret i32 %20
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

declare i32 @printf(ptr, ...)

declare void @exit(i32)

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
