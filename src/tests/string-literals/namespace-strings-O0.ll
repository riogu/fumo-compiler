; ModuleID = 'src/tests/string-literals/namespace-strings.fm'
source_filename = "src/tests/string-literals/namespace-strings.fm"

%"messages::config" = type { ptr, ptr }

@.str = private unnamed_addr constant [16 x i8] c"Error occurred!\00", align 1
@.str.1 = private unnamed_addr constant [22 x i8] c"Operation successful!\00", align 1

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

define void @"fumo.init.for: src/tests/string-literals/namespace-strings.fm"() #0 {
  ret void
}

define void @"messages::config::print_error"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"messages::config", ptr %2, i32 0, i32 0
  %4 = load ptr, ptr %3, align 8
  %5 = call i32 @puts(ptr %4)
  ret void
}

define void @"messages::config::print_success"(ptr %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds nuw %"messages::config", ptr %2, i32 0, i32 1
  %4 = load ptr, ptr %3, align 8
  %5 = call i32 @puts(ptr %4)
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::cfg" = alloca %"messages::config", align 8
  store %"messages::config" zeroinitializer, ptr %"main()::cfg", align 8
  %1 = alloca %"messages::config", align 8
  store %"messages::config" zeroinitializer, ptr %1, align 8
  %2 = getelementptr inbounds nuw %"messages::config", ptr %1, i32 0, i32 0
  store ptr @.str, ptr %2, align 8
  %3 = getelementptr inbounds nuw %"messages::config", ptr %1, i32 0, i32 1
  store ptr @.str.1, ptr %3, align 8
  %4 = load %"messages::config", ptr %1, align 8
  store %"messages::config" %4, ptr %"main()::cfg", align 8
  call void @"messages::config::print_success"(ptr %"main()::cfg")
  call void @"messages::config::print_error"(ptr %"main()::cfg")
  ret i32 0
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/string-literals/namespace-strings.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
