; ModuleID = 'src/tests/string-literals/fumo-strings.fm'
source_filename = "src/tests/string-literals/fumo-strings.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%"fm::Fumo" = type { ptr, i32 }

@reimu = local_unnamed_addr global %"fm::Fumo" zeroinitializer
@cirno = local_unnamed_addr global %"fm::Fumo" zeroinitializer
@.str = private unnamed_addr constant [30 x i8] c"%s\E3\81\A1\E3\82\83\E3\82\93\E3\81\AF%d\E5\9B\9Esquished!\0A\00", align 1
@.str.1 = private unnamed_addr constant [24 x i8] c"petting %s %d times...\0A\00", align 1
@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [251 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/string-literals/fumo-strings.fm' at line 18:\0A   |     printf(\22petting %s %d times...\\n\22, fumo->name, times);\0A   |                                            ^ found null pointer dereference\0A\0A\00", align 1
@.str.3 = private unnamed_addr constant [6 x i8] c"Reimu\00", align 1
@.str.4 = private unnamed_addr constant [6 x i8] c"Cirno\00", align 1
@.str.7 = private unnamed_addr constant [16 x i8] c"Reimu: %d pats\0A\00", align 1
@.str.8 = private unnamed_addr constant [16 x i8] c"Cirno: %d pats\0A\00", align 1
@str = private unnamed_addr constant [19 x i8] c"fumo lang example.\00", align 1
@str.9 = private unnamed_addr constant [18 x i8] c"\0Afinal pat count:\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree
declare void @exit(i32) local_unnamed_addr #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none)
define void @"fumo.init.for: src/tests/string-literals/fumo-strings.fm"() local_unnamed_addr #2 {
  store %"fm::Fumo" { ptr @.str.4, i32 418 }, ptr @cirno, align 8
  store %"fm::Fumo" { ptr @.str.3, i32 66 }, ptr @reimu, align 8
  ret void
}

; Function Attrs: nofree nounwind
define void @"fm::Fumo::squish"(ptr nocapture %0, i32 %1) local_unnamed_addr #0 {
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %4 = load i32, ptr %3, align 4
  %5 = add i32 %4, %1
  store i32 %5, ptr %3, align 4
  %6 = load ptr, ptr %0, align 8
  %7 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, ptr %6, i32 %5)
  ret void
}

; Function Attrs: nofree
define void @"fm::pet_fumo"(ptr %0, i32 %1) local_unnamed_addr #1 {
  %is_null = icmp eq ptr %0, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %2
  tail call void @fumo.runtime_error(ptr nonnull @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %2
  %3 = load ptr, ptr %0, align 8
  %4 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.1, ptr %3, i32 %1)
  %5 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %6 = load i32, ptr %5, align 4
  %7 = add i32 %6, %1
  store i32 %7, ptr %5, align 4
  %8 = load ptr, ptr %0, align 8
  %9 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, ptr %8, i32 %7)
  ret void
}

; Function Attrs: cold nofree noreturn
define void @fumo.runtime_error(ptr %0) local_unnamed_addr #3 {
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @0, ptr %0)
  tail call void @exit(i32 1) #5
  unreachable
}

; Function Attrs: nofree nounwind
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #4 {
  store %"fm::Fumo" { ptr @.str.4, i32 418 }, ptr @cirno, align 8
  store %"fm::Fumo" { ptr @.str.3, i32 66 }, ptr @reimu, align 8
  %puts.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @str)
  %1 = load ptr, ptr @reimu, align 8
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.1, ptr %1, i32 3)
  %3 = load i32, ptr getelementptr inbounds nuw (i8, ptr @reimu, i64 8), align 8
  %4 = add i32 %3, 3
  store i32 %4, ptr getelementptr inbounds nuw (i8, ptr @reimu, i64 8), align 8
  %5 = load ptr, ptr @reimu, align 8
  %6 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, ptr %5, i32 %4)
  %7 = load ptr, ptr @cirno, align 8
  %8 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.1, ptr %7, i32 2)
  %9 = load i32, ptr getelementptr inbounds nuw (i8, ptr @cirno, i64 8), align 8
  %10 = add i32 %9, 2
  store i32 %10, ptr getelementptr inbounds nuw (i8, ptr @cirno, i64 8), align 8
  %11 = load ptr, ptr @cirno, align 8
  %12 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str, ptr %11, i32 %10)
  %puts1.i = tail call i32 @puts(ptr nonnull dereferenceable(1) @str.9)
  %13 = load i32, ptr getelementptr inbounds nuw (i8, ptr @reimu, i64 8), align 8
  %14 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.7, i32 %13)
  %15 = load i32, ptr getelementptr inbounds nuw (i8, ptr @cirno, i64 8), align 8
  %16 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.str.8, i32 %15)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
attributes #1 = { nofree }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(write, argmem: none, inaccessiblemem: none) "used" }
attributes #3 = { cold nofree noreturn }
attributes #4 = { nofree nounwind "used" }
attributes #5 = { cold }
