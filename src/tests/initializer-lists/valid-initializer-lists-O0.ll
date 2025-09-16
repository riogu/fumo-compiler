; ModuleID = 'src/tests/initializer-lists/valid-initializer-lists.fm'
source_filename = "src/tests/initializer-lists/valid-initializer-lists.fm"

%SomeStruct = type { i32, i32 }
%OuterStruct = type { %InnerStruct, i32 }
%InnerStruct = type { i32, i32 }
%EmptyStruct = type {}
%SingleStruct = type { i32 }
%StringStruct = type { ptr, ptr }
%ExprStruct = type { i32, i32 }
%VarStruct = type { i32, i32 }
%TrailingStruct = type { i32, i32, i32 }
%NestedStruct = type { %InnerStruct, i32 }
%ComplexStruct = type { %SimpleStruct, %MiddleStruct, i32 }
%SimpleStruct = type { i32, i32 }
%MiddleStruct = type { %InnerStruct, i32 }
%FuncStruct = type { i32, i32 }
%"foo::BarStruct" = type { i32, i32 }
%"foo::bar::DeepStruct" = type { %"foo::bar::InnerStruct", i32 }
%"foo::bar::InnerStruct" = type { i32, i32 }

@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@.str.1 = private unnamed_addr constant [6 x i8] c"world\00", align 1

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

define void @"fumo.init.for: src/tests/initializer-lists/valid-initializer-lists.fm"() #0 {
  ret void
}

define i32 @get_value() {
  ret i32 42
}

define i32 @another_func() {
  ret i32 24
}

define internal i32 @fumo.user_main() {
  %"main()::s1" = alloca %SomeStruct, align 8
  store %SomeStruct zeroinitializer, ptr %"main()::s1", align 4
  %1 = alloca %SomeStruct, align 8
  store %SomeStruct zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %SomeStruct, ptr %1, i32 0, i32 0
  store i32 123, ptr %2, align 4
  %3 = getelementptr inbounds nuw %SomeStruct, ptr %1, i32 0, i32 1
  store i32 456, ptr %3, align 4
  %4 = load %SomeStruct, ptr %1, align 4
  store %SomeStruct %4, ptr %"main()::s1", align 4
  %"main()::s2" = alloca %OuterStruct, align 8
  store %OuterStruct zeroinitializer, ptr %"main()::s2", align 4
  %5 = alloca %OuterStruct, align 8
  store %OuterStruct zeroinitializer, ptr %5, align 4
  %6 = alloca %InnerStruct, align 8
  store %InnerStruct zeroinitializer, ptr %6, align 4
  %7 = getelementptr inbounds nuw %InnerStruct, ptr %6, i32 0, i32 0
  store i32 1, ptr %7, align 4
  %8 = getelementptr inbounds nuw %InnerStruct, ptr %6, i32 0, i32 1
  store i32 2, ptr %8, align 4
  %9 = load %InnerStruct, ptr %6, align 4
  %10 = getelementptr inbounds nuw %OuterStruct, ptr %5, i32 0, i32 0
  store %InnerStruct %9, ptr %10, align 4
  %11 = getelementptr inbounds nuw %OuterStruct, ptr %5, i32 0, i32 1
  store i32 3, ptr %11, align 4
  %12 = load %OuterStruct, ptr %5, align 4
  store %OuterStruct %12, ptr %"main()::s2", align 4
  %"main()::s3" = alloca %EmptyStruct, align 8
  store %EmptyStruct zeroinitializer, ptr %"main()::s3", align 1
  %13 = alloca %EmptyStruct, align 8
  store %EmptyStruct zeroinitializer, ptr %13, align 1
  %14 = load %EmptyStruct, ptr %13, align 1
  store %EmptyStruct %14, ptr %"main()::s3", align 1
  %"main()::s4" = alloca %SingleStruct, align 8
  store %SingleStruct zeroinitializer, ptr %"main()::s4", align 4
  %15 = alloca %SingleStruct, align 8
  store %SingleStruct zeroinitializer, ptr %15, align 4
  %16 = getelementptr inbounds nuw %SingleStruct, ptr %15, i32 0, i32 0
  store i32 42, ptr %16, align 4
  %17 = load %SingleStruct, ptr %15, align 4
  store %SingleStruct %17, ptr %"main()::s4", align 4
  %"main()::s5" = alloca %StringStruct, align 8
  store %StringStruct zeroinitializer, ptr %"main()::s5", align 8
  %18 = alloca %StringStruct, align 8
  store %StringStruct zeroinitializer, ptr %18, align 8
  %19 = getelementptr inbounds nuw %StringStruct, ptr %18, i32 0, i32 0
  store ptr @.str, ptr %19, align 8
  %20 = getelementptr inbounds nuw %StringStruct, ptr %18, i32 0, i32 1
  store ptr @.str.1, ptr %20, align 8
  %21 = load %StringStruct, ptr %18, align 8
  store %StringStruct %21, ptr %"main()::s5", align 8
  %"main()::s6" = alloca %ExprStruct, align 8
  store %ExprStruct zeroinitializer, ptr %"main()::s6", align 4
  %22 = alloca %ExprStruct, align 8
  store %ExprStruct zeroinitializer, ptr %22, align 4
  %23 = getelementptr inbounds nuw %ExprStruct, ptr %22, i32 0, i32 0
  store i32 3, ptr %23, align 4
  %24 = getelementptr inbounds nuw %ExprStruct, ptr %22, i32 0, i32 1
  store i32 12, ptr %24, align 4
  %25 = load %ExprStruct, ptr %22, align 4
  store %ExprStruct %25, ptr %"main()::s6", align 4
  %"main()::x" = alloca i32, align 4
  store i32 0, ptr %"main()::x", align 4
  store i32 10, ptr %"main()::x", align 4
  %"main()::y" = alloca i32, align 4
  store i32 0, ptr %"main()::y", align 4
  store i32 20, ptr %"main()::y", align 4
  %"main()::s7" = alloca %VarStruct, align 8
  store %VarStruct zeroinitializer, ptr %"main()::s7", align 4
  %26 = alloca %VarStruct, align 8
  store %VarStruct zeroinitializer, ptr %26, align 4
  %27 = load i32, ptr %"main()::x", align 4
  %28 = getelementptr inbounds nuw %VarStruct, ptr %26, i32 0, i32 0
  store i32 %27, ptr %28, align 4
  %29 = load i32, ptr %"main()::y", align 4
  %30 = getelementptr inbounds nuw %VarStruct, ptr %26, i32 0, i32 1
  store i32 %29, ptr %30, align 4
  %31 = load %VarStruct, ptr %26, align 4
  store %VarStruct %31, ptr %"main()::s7", align 4
  %"main()::s8" = alloca %TrailingStruct, align 8
  store %TrailingStruct zeroinitializer, ptr %"main()::s8", align 4
  %32 = alloca %TrailingStruct, align 8
  store %TrailingStruct zeroinitializer, ptr %32, align 4
  %33 = getelementptr inbounds nuw %TrailingStruct, ptr %32, i32 0, i32 0
  store i32 1, ptr %33, align 4
  %34 = getelementptr inbounds nuw %TrailingStruct, ptr %32, i32 0, i32 1
  store i32 2, ptr %34, align 4
  %35 = getelementptr inbounds nuw %TrailingStruct, ptr %32, i32 0, i32 2
  store i32 3, ptr %35, align 4
  %36 = load %TrailingStruct, ptr %32, align 4
  store %TrailingStruct %36, ptr %"main()::s8", align 4
  %"main()::s9" = alloca %NestedStruct, align 8
  store %NestedStruct zeroinitializer, ptr %"main()::s9", align 4
  %37 = alloca %NestedStruct, align 8
  store %NestedStruct zeroinitializer, ptr %37, align 4
  %38 = alloca %InnerStruct, align 8
  store %InnerStruct zeroinitializer, ptr %38, align 4
  %39 = getelementptr inbounds nuw %InnerStruct, ptr %38, i32 0, i32 0
  store i32 1, ptr %39, align 4
  %40 = getelementptr inbounds nuw %InnerStruct, ptr %38, i32 0, i32 1
  store i32 2, ptr %40, align 4
  %41 = load %InnerStruct, ptr %38, align 4
  %42 = getelementptr inbounds nuw %NestedStruct, ptr %37, i32 0, i32 0
  store %InnerStruct %41, ptr %42, align 4
  %43 = getelementptr inbounds nuw %NestedStruct, ptr %37, i32 0, i32 1
  store i32 3, ptr %43, align 4
  %44 = load %NestedStruct, ptr %37, align 4
  store %NestedStruct %44, ptr %"main()::s9", align 4
  %"main()::s10" = alloca %ComplexStruct, align 8
  store %ComplexStruct zeroinitializer, ptr %"main()::s10", align 4
  %45 = alloca %ComplexStruct, align 8
  store %ComplexStruct zeroinitializer, ptr %45, align 4
  %46 = alloca %SimpleStruct, align 8
  store %SimpleStruct zeroinitializer, ptr %46, align 4
  %47 = getelementptr inbounds nuw %SimpleStruct, ptr %46, i32 0, i32 0
  store i32 1, ptr %47, align 4
  %48 = getelementptr inbounds nuw %SimpleStruct, ptr %46, i32 0, i32 1
  store i32 2, ptr %48, align 4
  %49 = load %SimpleStruct, ptr %46, align 4
  %50 = getelementptr inbounds nuw %ComplexStruct, ptr %45, i32 0, i32 0
  store %SimpleStruct %49, ptr %50, align 4
  %51 = alloca %MiddleStruct, align 8
  store %MiddleStruct zeroinitializer, ptr %51, align 4
  %52 = alloca %InnerStruct, align 8
  store %InnerStruct zeroinitializer, ptr %52, align 4
  %53 = getelementptr inbounds nuw %InnerStruct, ptr %52, i32 0, i32 0
  store i32 3, ptr %53, align 4
  %54 = getelementptr inbounds nuw %InnerStruct, ptr %52, i32 0, i32 1
  store i32 4, ptr %54, align 4
  %55 = load %InnerStruct, ptr %52, align 4
  %56 = getelementptr inbounds nuw %MiddleStruct, ptr %51, i32 0, i32 0
  store %InnerStruct %55, ptr %56, align 4
  %57 = getelementptr inbounds nuw %MiddleStruct, ptr %51, i32 0, i32 1
  store i32 5, ptr %57, align 4
  %58 = load %MiddleStruct, ptr %51, align 4
  %59 = getelementptr inbounds nuw %ComplexStruct, ptr %45, i32 0, i32 1
  store %MiddleStruct %58, ptr %59, align 4
  %60 = getelementptr inbounds nuw %ComplexStruct, ptr %45, i32 0, i32 2
  store i32 6, ptr %60, align 4
  %61 = load %ComplexStruct, ptr %45, align 4
  store %ComplexStruct %61, ptr %"main()::s10", align 4
  %"main()::s11" = alloca %FuncStruct, align 8
  store %FuncStruct zeroinitializer, ptr %"main()::s11", align 4
  %62 = alloca %FuncStruct, align 8
  store %FuncStruct zeroinitializer, ptr %62, align 4
  %63 = call i32 @get_value()
  %64 = getelementptr inbounds nuw %FuncStruct, ptr %62, i32 0, i32 0
  store i32 %63, ptr %64, align 4
  %65 = call i32 @another_func()
  %66 = getelementptr inbounds nuw %FuncStruct, ptr %62, i32 0, i32 1
  store i32 %65, ptr %66, align 4
  %67 = load %FuncStruct, ptr %62, align 4
  store %FuncStruct %67, ptr %"main()::s11", align 4
  %"main()::s12" = alloca %"foo::BarStruct", align 8
  store %"foo::BarStruct" zeroinitializer, ptr %"main()::s12", align 4
  %68 = alloca %"foo::BarStruct", align 8
  store %"foo::BarStruct" zeroinitializer, ptr %68, align 4
  %69 = getelementptr inbounds nuw %"foo::BarStruct", ptr %68, i32 0, i32 0
  store i32 1, ptr %69, align 4
  %70 = getelementptr inbounds nuw %"foo::BarStruct", ptr %68, i32 0, i32 1
  store i32 2, ptr %70, align 4
  %71 = load %"foo::BarStruct", ptr %68, align 4
  store %"foo::BarStruct" %71, ptr %"main()::s12", align 4
  %"main()::s13" = alloca %"foo::bar::DeepStruct", align 8
  store %"foo::bar::DeepStruct" zeroinitializer, ptr %"main()::s13", align 4
  %72 = alloca %"foo::bar::DeepStruct", align 8
  store %"foo::bar::DeepStruct" zeroinitializer, ptr %72, align 4
  %73 = alloca %"foo::bar::InnerStruct", align 8
  store %"foo::bar::InnerStruct" zeroinitializer, ptr %73, align 4
  %74 = getelementptr inbounds nuw %"foo::bar::InnerStruct", ptr %73, i32 0, i32 0
  store i32 1, ptr %74, align 4
  %75 = getelementptr inbounds nuw %"foo::bar::InnerStruct", ptr %73, i32 0, i32 1
  store i32 2, ptr %75, align 4
  %76 = load %"foo::bar::InnerStruct", ptr %73, align 4
  %77 = getelementptr inbounds nuw %"foo::bar::DeepStruct", ptr %72, i32 0, i32 0
  store %"foo::bar::InnerStruct" %76, ptr %77, align 4
  %78 = getelementptr inbounds nuw %"foo::bar::DeepStruct", ptr %72, i32 0, i32 1
  store i32 3, ptr %78, align 4
  %79 = load %"foo::bar::DeepStruct", ptr %72, align 4
  store %"foo::bar::DeepStruct" %79, ptr %"main()::s13", align 4
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/initializer-lists/valid-initializer-lists.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
