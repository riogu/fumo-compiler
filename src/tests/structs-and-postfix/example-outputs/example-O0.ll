; ModuleID = 'src/tests/structs-and-postfix/example-outputs/example'
source_filename = "src/tests/structs-and-postfix/example.fm"

%"foo::SomeStruct" = type { %"foo::SomeStruct::InnerStruct", i32 }
%"foo::SomeStruct::InnerStruct" = type { i32, ptr }

@x = global i32 0
@"foo::x" = global i32 0
@"foo::inner::x" = global i32 0
@global_example = global %"foo::SomeStruct" zeroinitializer
@.str = private unnamed_addr constant [15 x i8] c"string example\00", align 1
@.str.1 = private unnamed_addr constant [17 x i8] c"passing a string\00", align 1
@.str.2 = private unnamed_addr constant [15 x i8] c"string example\00", align 1
@.str.3 = private unnamed_addr constant [8 x i8] c"example\00", align 1

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

define void @fumo.init() #0 {
  %1 = alloca %"foo::SomeStruct", align 8
  store %"foo::SomeStruct" zeroinitializer, ptr %1, align 8
  %2 = alloca %"foo::SomeStruct::InnerStruct", align 8
  store %"foo::SomeStruct::InnerStruct" zeroinitializer, ptr %2, align 8
  %3 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %2, i32 0, i32 0
  store i32 123, ptr %3, align 4
  %4 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %2, i32 0, i32 1
  store ptr @.str, ptr %4, align 8
  %5 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %2, i32 0, i32 0
  store i32 69420, ptr %5, align 4
  %6 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %2, i32 0, i32 1
  store ptr @.str.1, ptr %6, align 8
  %7 = load %"foo::SomeStruct::InnerStruct", ptr %2, align 8
  %8 = getelementptr inbounds nuw %"foo::SomeStruct", ptr %1, i32 0, i32 0
  store %"foo::SomeStruct::InnerStruct" %7, ptr %8, align 8
  %9 = alloca %"foo::SomeStruct::InnerStruct", align 8
  store %"foo::SomeStruct::InnerStruct" zeroinitializer, ptr %9, align 8
  %10 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %9, i32 0, i32 0
  store i32 123, ptr %10, align 4
  %11 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %9, i32 0, i32 1
  store ptr @.str.2, ptr %11, align 8
  %12 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %9, i32 0, i32 0
  store i32 11111, ptr %12, align 4
  %13 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %9, i32 0, i32 1
  store ptr @.str.3, ptr %13, align 8
  %14 = load %"foo::SomeStruct::InnerStruct", ptr %9, align 8
  %15 = getelementptr inbounds nuw %"foo::SomeStruct", ptr %1, i32 0, i32 0
  store %"foo::SomeStruct::InnerStruct" %14, ptr %15, align 8
  %16 = getelementptr inbounds nuw %"foo::SomeStruct", ptr %1, i32 0, i32 1
  store i32 66, ptr %16, align 4
  %17 = load %"foo::SomeStruct", ptr %1, align 8
  store %"foo::SomeStruct" %17, ptr @global_example, align 8
  store i32 69420, ptr @"foo::x", align 4
  store i32 69, ptr @x, align 4
  ret void
}

define void @"foo::inner::func"() {
  store i32 222222, ptr @"foo::inner::x", align 4
  ret void
}

define void @"foo::func"(i32 %0, double %1) {
  %"foo::func()::a" = alloca i32, align 4
  store i32 0, ptr %"foo::func()::a", align 4
  store i32 %0, ptr %"foo::func()::a", align 4
  %"foo::func()::b" = alloca double, align 8
  store double 0.000000e+00, ptr %"foo::func()::b", align 8
  store double %1, ptr %"foo::func()::b", align 8
  %3 = load i32, ptr %"foo::func()::a", align 4
  %4 = load i32, ptr %"foo::func()::a", align 4
  %5 = mul i32 %3, %4
  %6 = add i32 111111, %5
  store i32 %6, ptr @"foo::x", align 4
  store i32 333333, ptr @"foo::inner::x", align 4
  ret void
}

define i32 @"foo::SomeStruct::InnerStruct::gaming_func"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"foo::SomeStruct::InnerStruct", ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  ret i32 %4
}

define internal i32 @fumo.user_main() {
  %1 = load i32, ptr getelementptr inbounds nuw (%"foo::SomeStruct", ptr @global_example, i32 0, i32 1), align 4
  ret i32 %1
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @fumo.init()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
