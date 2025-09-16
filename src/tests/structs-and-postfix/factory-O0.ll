; ModuleID = 'src/tests/structs-and-postfix/factory.fm'
source_filename = "src/tests/structs-and-postfix/factory.fm"

%"factory::product" = type { i32 }
%"factory::factory" = type {}

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [280 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/factory.fm' at line 14:\0A   | fn factory::factory::process(p: product*) -> i32 { return p->get_id(); }\0A   |                                                            ^ found null pointer dereference\0A\0A\00", align 1

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

define void @"fumo.init.for: src/tests/structs-and-postfix/factory.fm"() #0 {
  ret void
}

define i32 @"factory::product::get_id"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"factory::product", ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define %"factory::product" @"factory::factory::create"(ptr %0, i32 %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"factory::factory::create()::id" = alloca i32, align 4
  store i32 0, ptr %"factory::factory::create()::id", align 4
  store i32 %1, ptr %"factory::factory::create()::id", align 4
  %3 = alloca %"factory::product", align 8
  store %"factory::product" zeroinitializer, ptr %3, align 4
  %4 = getelementptr inbounds nuw %"factory::product", ptr %3, i32 0, i32 0
  store i32 0, ptr %4, align 4
  %5 = load i32, ptr %"factory::factory::create()::id", align 4
  %6 = getelementptr inbounds nuw %"factory::product", ptr %3, i32 0, i32 0
  store i32 %5, ptr %6, align 4
  %7 = load %"factory::product", ptr %3, align 4
  ret %"factory::product" %7
}

define i32 @"factory::factory::process"(ptr %0, ptr %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"factory::factory::process()::p" = alloca ptr, align 8
  store ptr null, ptr %"factory::factory::process()::p", align 8
  store ptr %1, ptr %"factory::factory::process()::p", align 8
  %3 = load ptr, ptr %"factory::factory::process()::p", align 8
  %is_null = icmp eq ptr %3, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %2
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %2
  %4 = call i32 @"factory::product::get_id"(ptr %3)
  %5 = alloca i32, align 4
  store i32 %4, ptr %5, align 4
  %6 = load i32, ptr %5, align 4
  ret i32 %6
}

define internal i32 @fumo.user_main() {
  %"main()::f" = alloca %"factory::factory", align 8
  store %"factory::factory" zeroinitializer, ptr %"main()::f", align 1
  %1 = alloca %"factory::factory", align 8
  store %"factory::factory" zeroinitializer, ptr %1, align 1
  %2 = load %"factory::factory", ptr %1, align 1
  store %"factory::factory" %2, ptr %"main()::f", align 1
  %3 = call %"factory::product" @"factory::factory::create"(ptr %"main()::f", i32 42)
  %4 = alloca %"factory::product", align 8
  store %"factory::product" %3, ptr %4, align 4
  %5 = call i32 @"factory::product::get_id"(ptr %4)
  %6 = alloca i32, align 4
  store i32 %5, ptr %6, align 4
  %7 = load i32, ptr %6, align 4
  ret i32 %7
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/structs-and-postfix/factory.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
