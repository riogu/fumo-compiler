; ModuleID = 'src/tests/structs-and-postfix/recursive-node.fm'
source_filename = "src/tests/structs-and-postfix/recursive-node.fm"

%node = type { i32, ptr }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [242 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/recursive-node.fm' at line 17:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                            ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.1 = private unnamed_addr constant [254 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/recursive-node.fm' at line 17:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                                        ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [266 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/recursive-node.fm' at line 17:\0A   |     return node1.get_next()->get_next()->get_next()->value;\0A   |                                                    ^ found null pointer dereference\0A\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

declare i64 @strlen(ptr)

declare i32 @strcmp(ptr, ptr)

declare ptr @strcpy(ptr, ptr)

declare ptr @strcat(ptr, ptr)

declare ptr @malloc(i64)

declare void @free(ptr)

declare void @exit(i32)

declare void @abort()

declare ptr @memcpy(ptr, ptr, i64)

declare ptr @memset(ptr, i32, i64)

define void @"fumo.init.for: src/tests/structs-and-postfix/recursive-node.fm"() #0 {
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
  %"main()::node1" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node1", align 8
  %1 = alloca %node, align 8
  store %node zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 0, ptr %2, align 4
  %3 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 1
  store ptr null, ptr %3, align 8
  %4 = getelementptr inbounds nuw %node, ptr %1, i32 0, i32 0
  store i32 10, ptr %4, align 4
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
  %"main()::node4" = alloca %node, align 8
  store %node zeroinitializer, ptr %"main()::node4", align 8
  %16 = alloca %node, align 8
  store %node zeroinitializer, ptr %16, align 8
  %17 = getelementptr inbounds nuw %node, ptr %16, i32 0, i32 0
  store i32 0, ptr %17, align 4
  %18 = getelementptr inbounds nuw %node, ptr %16, i32 0, i32 1
  store ptr null, ptr %18, align 8
  %19 = getelementptr inbounds nuw %node, ptr %16, i32 0, i32 0
  store i32 40, ptr %19, align 4
  %20 = load %node, ptr %16, align 8
  store %node %20, ptr %"main()::node4", align 8
  call void @"node::set_next"(ptr %"main()::node1", ptr %"main()::node2")
  call void @"node::set_next"(ptr %"main()::node2", ptr %"main()::node3")
  call void @"node::set_next"(ptr %"main()::node3", ptr %"main()::node4")
  call void @"node::set_next"(ptr %"main()::node3", ptr %"main()::node4")
  %21 = call ptr @"node::get_next"(ptr %"main()::node1")
  %is_null = icmp eq ptr %21, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %0
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %0
  %22 = call ptr @"node::get_next"(ptr %21)
  %is_null3 = icmp eq ptr %22, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.1)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %23 = call ptr @"node::get_next"(ptr %22)
  %is_null6 = icmp eq ptr %23, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %24 = getelementptr inbounds nuw %node, ptr %23, i32 0, i32 0
  %25 = load i32, ptr %24, align 4
  ret i32 %25
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/structs-and-postfix/recursive-node.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
