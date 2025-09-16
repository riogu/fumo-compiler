; ModuleID = 'src/tests/string-literals/fumo-strings.fm'
source_filename = "src/tests/string-literals/fumo-strings.fm"

%"fm::Fumo" = type { ptr, i32 }

@reimu = global %"fm::Fumo" zeroinitializer
@cirno = global %"fm::Fumo" zeroinitializer
@.str = private unnamed_addr constant [30 x i8] c"%s\E3\81\A1\E3\82\83\E3\82\93\E3\81\AF%d\E5\9B\9Esquished!\0A\00", align 1
@.str.1 = private unnamed_addr constant [24 x i8] c"petting %s %d times...\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [251 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/string-literals/fumo-strings.fm' at line 18:\0A   |     printf(\22petting %s %d times...\\n\22, fumo->name, times);\0A   |                                            ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [182 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/string-literals/fumo-strings.fm' at line 19:\0A   |     fumo->squish(times);\0A   |         ^ found null pointer dereference\0A\0A\00", align 1
@.str.3 = private unnamed_addr constant [6 x i8] c"Reimu\00", align 1
@.str.4 = private unnamed_addr constant [6 x i8] c"Cirno\00", align 1
@.str.5 = private unnamed_addr constant [20 x i8] c"fumo lang example.\0A\00", align 1
@.str.6 = private unnamed_addr constant [19 x i8] c"\0Afinal pat count:\0A\00", align 1
@.str.7 = private unnamed_addr constant [16 x i8] c"Reimu: %d pats\0A\00", align 1
@.str.8 = private unnamed_addr constant [16 x i8] c"Cirno: %d pats\0A\00", align 1

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

define void @"fumo.init.for: src/tests/string-literals/fumo-strings.fm"() #0 {
  %1 = alloca %"fm::Fumo", align 8
  %2 = alloca %"fm::Fumo", align 8
  store %"fm::Fumo" zeroinitializer, ptr %2, align 8
  %3 = getelementptr inbounds nuw %"fm::Fumo", ptr %2, i32 0, i32 0
  store ptr @.str.4, ptr %3, align 8
  %4 = getelementptr inbounds nuw %"fm::Fumo", ptr %2, i32 0, i32 1
  store i32 418, ptr %4, align 4
  %5 = load %"fm::Fumo", ptr %2, align 8
  store %"fm::Fumo" %5, ptr @cirno, align 8
  store %"fm::Fumo" zeroinitializer, ptr %1, align 8
  %6 = getelementptr inbounds nuw %"fm::Fumo", ptr %1, i32 0, i32 0
  store ptr @.str.3, ptr %6, align 8
  %7 = getelementptr inbounds nuw %"fm::Fumo", ptr %1, i32 0, i32 1
  store i32 66, ptr %7, align 4
  %8 = load %"fm::Fumo", ptr %1, align 8
  store %"fm::Fumo" %8, ptr @reimu, align 8
  ret void
}

define void @"fm::Fumo::squish"(ptr %0, i32 %1) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %"fm::Fumo::squish()::times" = alloca i32, align 4
  store i32 0, ptr %"fm::Fumo::squish()::times", align 4
  store i32 %1, ptr %"fm::Fumo::squish()::times", align 4
  %3 = load ptr, ptr %this, align 8
  %4 = getelementptr inbounds nuw %"fm::Fumo", ptr %3, i32 0, i32 1
  %5 = load ptr, ptr %this, align 8
  %6 = getelementptr inbounds nuw %"fm::Fumo", ptr %5, i32 0, i32 1
  %7 = load i32, ptr %6, align 4
  %8 = load i32, ptr %"fm::Fumo::squish()::times", align 4
  %9 = add i32 %7, %8
  store i32 %9, ptr %4, align 4
  %10 = load ptr, ptr %this, align 8
  %11 = getelementptr inbounds nuw %"fm::Fumo", ptr %10, i32 0, i32 0
  %12 = load ptr, ptr %11, align 8
  %13 = load ptr, ptr %this, align 8
  %14 = getelementptr inbounds nuw %"fm::Fumo", ptr %13, i32 0, i32 1
  %15 = load i32, ptr %14, align 4
  %16 = call i32 (ptr, ...) @printf(ptr @.str, ptr %12, i32 %15)
  ret void
}

define void @"fm::pet_fumo"(ptr %0, i32 %1) {
  %"fm::pet_fumo()::fumo" = alloca ptr, align 8
  store ptr null, ptr %"fm::pet_fumo()::fumo", align 8
  store ptr %0, ptr %"fm::pet_fumo()::fumo", align 8
  %"fm::pet_fumo()::times" = alloca i32, align 4
  store i32 0, ptr %"fm::pet_fumo()::times", align 4
  store i32 %1, ptr %"fm::pet_fumo()::times", align 4
  %3 = load ptr, ptr %"fm::pet_fumo()::fumo", align 8
  %is_null = icmp eq ptr %3, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %2
  call void @fumo.runtime_error(ptr @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %2
  %4 = getelementptr inbounds nuw %"fm::Fumo", ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %"fm::pet_fumo()::times", align 4
  %7 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr %5, i32 %6)
  %8 = load ptr, ptr %"fm::pet_fumo()::fumo", align 8
  %is_null3 = icmp eq ptr %8, null
  br i1 %is_null3, label %null_trap1, label %safe_deref2

null_trap1:                                       ; preds = %safe_deref
  call void @fumo.runtime_error(ptr @.str_error_msg.2)
  unreachable

safe_deref2:                                      ; preds = %safe_deref
  %9 = load i32, ptr %"fm::pet_fumo()::times", align 4
  call void @"fm::Fumo::squish"(ptr %8, i32 %9)
  ret void
}

define internal i32 @fumo.user_main() {
  %1 = call i32 (ptr, ...) @printf(ptr @.str.5)
  call void @"fm::pet_fumo"(ptr @reimu, i32 3)
  call void @"fm::pet_fumo"(ptr @cirno, i32 2)
  %2 = call i32 (ptr, ...) @printf(ptr @.str.6)
  %3 = load i32, ptr getelementptr inbounds nuw (%"fm::Fumo", ptr @reimu, i32 0, i32 1), align 4
  %4 = call i32 (ptr, ...) @printf(ptr @.str.7, i32 %3)
  %5 = load i32, ptr getelementptr inbounds nuw (%"fm::Fumo", ptr @cirno, i32 0, i32 1), align 4
  %6 = call i32 (ptr, ...) @printf(ptr @.str.8, i32 %5)
  ret i32 0
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/string-literals/fumo-strings.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
