; ModuleID = 'src/tests/string-literals/struct_member.fm'
source_filename = "src/tests/string-literals/struct_member.fm"

%person = type { ptr, i32 }

@.str = private unnamed_addr constant [20 x i8] c"Hi, I'm %s, age %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [8 x i8] c"Unknown\00", align 1
@.str.2 = private unnamed_addr constant [4 x i8] c"Bob\00", align 1

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

define void @"fumo.init.for: src/tests/string-literals/struct_member.fm"() #0 {
  ret void
}

define void @"person::introduce"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %person, ptr %2, i32 0, i32 0
  %4 = load ptr, ptr %3, align 8
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %person, ptr %5, i32 0, i32 1
  %7 = load i32, ptr %6, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str, ptr %4, i32 %7)
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::p" = alloca %person, align 8
  store %person zeroinitializer, ptr %"main()::p", align 8
  %1 = alloca %person, align 8
  store %person zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %person, ptr %1, i32 0, i32 0
  store ptr @.str.1, ptr %2, align 8
  %3 = getelementptr inbounds nuw %person, ptr %1, i32 0, i32 1
  store i32 0, ptr %3, align 4
  %4 = getelementptr inbounds nuw %person, ptr %1, i32 0, i32 0
  store ptr @.str.2, ptr %4, align 8
  %5 = getelementptr inbounds nuw %person, ptr %1, i32 0, i32 1
  store i32 30, ptr %5, align 4
  %6 = load %person, ptr %1, align 8
  store %person %6, ptr %"main()::p", align 8
  call void @"person::introduce"(ptr %"main()::p")
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/string-literals/struct_member.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
