; ModuleID = 'src/tests/if-statements/complex-conditions/fumo_if_test.fm'
source_filename = "src/tests/if-statements/complex-conditions/fumo_if_test.fm"

%"fm::Fumo" = type { ptr, i32 }

@reimu = global %"fm::Fumo" zeroinitializer
@cirno = global %"fm::Fumo" zeroinitializer
@.str = private unnamed_addr constant [30 x i8] c"%s\E3\81\A1\E3\82\83\E3\82\93\E3\81\AF%d\E5\9B\9Esquished!\0A\00", align 1
@.str.1 = private unnamed_addr constant [24 x i8] c"petting %s %d times...\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [268 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/fumo_if_test.fm' at line 17:\0A   |     printf(\22petting %s %d times...\\n\22, fumo->name, times);\0A   |                                            ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.2 = private unnamed_addr constant [199 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/fumo_if_test.fm' at line 18:\0A   |     fumo->squish(times);\0A   |         ^ found null pointer dereference\0A\0A\00", align 1
@.str_error_msg.3 = private unnamed_addr constant [207 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/fumo_if_test.fm' at line 20:\0A   |     if fumo->squished > 100 {\0A   |            ^ found null pointer dereference\0A\0A\00", align 1
@.str.4 = private unnamed_addr constant [30 x i8] c"%s is getting very squished!\0A\00", align 1
@.str_error_msg.5 = private unnamed_addr constant [281 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/if-statements/complex-conditions/fumo_if_test.fm' at line 21:\0A   |         printf(\22%s is getting very squished!\\n\22, fumo->name);\0A   |                                                      ^ found null pointer dereference\0A\0A\00", align 1
@.str.6 = private unnamed_addr constant [6 x i8] c"Reimu\00", align 1
@.str.7 = private unnamed_addr constant [6 x i8] c"Cirno\00", align 1
@.str.8 = private unnamed_addr constant [20 x i8] c"fumo lang example.\0A\00", align 1
@.str.9 = private unnamed_addr constant [34 x i8] c"\0Atotal pats across all fumos: %d\0A\00", align 1
@.str.10 = private unnamed_addr constant [23 x i8] c"that's a lot of pats!\0A\00", align 1
@.str.11 = private unnamed_addr constant [16 x i8] c"need more pats\0A\00", align 1
@.str.12 = private unnamed_addr constant [19 x i8] c"\0Afinal pat count:\0A\00", align 1
@.str.13 = private unnamed_addr constant [16 x i8] c"Reimu: %d pats\0A\00", align 1
@.str.14 = private unnamed_addr constant [16 x i8] c"Cirno: %d pats\0A\00", align 1

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

define void @"fumo.init.for: src/tests/if-statements/complex-conditions/fumo_if_test.fm"() #0 {
  %1 = alloca %"fm::Fumo", align 8
  %2 = alloca %"fm::Fumo", align 8
  store %"fm::Fumo" zeroinitializer, ptr %2, align 8
  %3 = getelementptr inbounds nuw %"fm::Fumo", ptr %2, i32 0, i32 0
  store ptr @.str.7, ptr %3, align 8
  %4 = getelementptr inbounds nuw %"fm::Fumo", ptr %2, i32 0, i32 1
  store i32 418, ptr %4, align 4
  %5 = load %"fm::Fumo", ptr %2, align 8
  store %"fm::Fumo" %5, ptr @cirno, align 8
  store %"fm::Fumo" zeroinitializer, ptr %1, align 8
  %6 = getelementptr inbounds nuw %"fm::Fumo", ptr %1, i32 0, i32 0
  store ptr @.str.6, ptr %6, align 8
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
  %10 = load ptr, ptr %"fm::pet_fumo()::fumo", align 8
  %is_null6 = icmp eq ptr %10, null
  br i1 %is_null6, label %null_trap4, label %safe_deref5

null_trap4:                                       ; preds = %safe_deref2
  call void @fumo.runtime_error(ptr @.str_error_msg.3)
  unreachable

safe_deref5:                                      ; preds = %safe_deref2
  %11 = getelementptr inbounds nuw %"fm::Fumo", ptr %10, i32 0, i32 1
  %12 = load i32, ptr %11, align 4
  %13 = icmp slt i32 100, %12
  br i1 %13, label %begin.if, label %end.if

begin.if:                                         ; preds = %safe_deref5
  %14 = load ptr, ptr %"fm::pet_fumo()::fumo", align 8
  %is_null9 = icmp eq ptr %14, null
  br i1 %is_null9, label %null_trap7, label %safe_deref8

null_trap7:                                       ; preds = %begin.if
  call void @fumo.runtime_error(ptr @.str_error_msg.5)
  unreachable

safe_deref8:                                      ; preds = %begin.if
  %15 = getelementptr inbounds nuw %"fm::Fumo", ptr %14, i32 0, i32 0
  %16 = load ptr, ptr %15, align 8
  %17 = call i32 (ptr, ...) @printf(ptr @.str.4, ptr %16)
  br label %end.if

end.if:                                           ; preds = %safe_deref8, %safe_deref5
  ret void
}

define internal i32 @fumo.user_main() {
  %1 = call i32 (ptr, ...) @printf(ptr @.str.8)
  call void @"fm::pet_fumo"(ptr @reimu, i32 3)
  call void @"fm::pet_fumo"(ptr @cirno, i32 2)
  %"main()::0::total_pats" = alloca i32, align 4
  store i32 0, ptr %"main()::0::total_pats", align 4
  %2 = load i32, ptr getelementptr inbounds nuw (%"fm::Fumo", ptr @reimu, i32 0, i32 1), align 4
  %3 = load i32, ptr getelementptr inbounds nuw (%"fm::Fumo", ptr @cirno, i32 0, i32 1), align 4
  %4 = add i32 %2, %3
  store i32 %4, ptr %"main()::0::total_pats", align 4
  %tobool = icmp ne i32 %4, 0
  br i1 %tobool, label %begin.if, label %end.if

begin.if:                                         ; preds = %0
  %5 = load i32, ptr %"main()::0::total_pats", align 4
  %6 = call i32 (ptr, ...) @printf(ptr @.str.9, i32 %5)
  %7 = load i32, ptr %"main()::0::total_pats", align 4
  %8 = icmp slt i32 500, %7
  br i1 %8, label %begin.if1, label %else

begin.if1:                                        ; preds = %begin.if
  %9 = call i32 (ptr, ...) @printf(ptr @.str.10)
  br label %end.if2

else:                                             ; preds = %begin.if
  %10 = call i32 (ptr, ...) @printf(ptr @.str.11)
  br label %end.if2

end.if2:                                          ; preds = %else, %begin.if1
  br label %end.if

end.if:                                           ; preds = %end.if2, %0
  %11 = call i32 (ptr, ...) @printf(ptr @.str.12)
  %12 = load i32, ptr getelementptr inbounds nuw (%"fm::Fumo", ptr @reimu, i32 0, i32 1), align 4
  %13 = call i32 (ptr, ...) @printf(ptr @.str.13, i32 %12)
  %14 = load i32, ptr getelementptr inbounds nuw (%"fm::Fumo", ptr @cirno, i32 0, i32 1), align 4
  %15 = call i32 (ptr, ...) @printf(ptr @.str.14, i32 %14)
  ret i32 0
}

define void @fumo.runtime_error(ptr %0) {
  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %0)
  call void @exit(i32 1)
  unreachable
}

define i32 @main(i32 %argc, ptr %argv) #0 {
  call void @"fumo.init.for: src/tests/if-statements/complex-conditions/fumo_if_test.fm"()
  %1 = call i32 @fumo.user_main()
  ret i32 %1
}

attributes #0 = { "used" }
