; ModuleID = 'src/tests/structs-and-postfix/factory-outputs/factory'
source_filename = "src/tests/structs-and-postfix/factory.fm"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%"factory::product" = type { i32 }

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str_error_msg = private unnamed_addr constant [280 x i8] c"-> \1B[38;2;235;67;54m[runtime error]\1B[0m in file 'src/tests/structs-and-postfix/factory.fm' at line 14:\0A   | fn factory::factory::process(p: product*) -> i32 { return p->get_id(); }\0A   |                                                            ^ found null pointer dereference\0A\0A\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree
declare void @exit(i32) local_unnamed_addr #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define void @fumo.init() local_unnamed_addr #2 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define i32 @"factory::product::get_id"(ptr nocapture readonly %0) local_unnamed_addr #3 {
  %2 = load i32, ptr %0, align 4
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define %"factory::product" @"factory::factory::create"(ptr nocapture readnone %0, i32 %1) local_unnamed_addr #4 {
  %.fca.0.insert = insertvalue %"factory::product" poison, i32 %1, 0
  ret %"factory::product" %.fca.0.insert
}

; Function Attrs: nofree
define i32 @"factory::factory::process"(ptr nocapture readnone %0, ptr readonly %1) local_unnamed_addr #1 {
  %is_null = icmp eq ptr %1, null
  br i1 %is_null, label %null_trap, label %safe_deref

null_trap:                                        ; preds = %2
  tail call void @fumo.runtime_error(ptr nonnull @.str_error_msg)
  unreachable

safe_deref:                                       ; preds = %2
  %3 = load i32, ptr %1, align 4
  ret i32 %3
}

; Function Attrs: cold nofree noreturn
define void @fumo.runtime_error(ptr %0) local_unnamed_addr #5 {
  %2 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @0, ptr %0)
  tail call void @exit(i32 1) #6
  unreachable
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define noundef i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #2 {
  ret i32 42
}

attributes #0 = { nofree nounwind }
attributes #1 = { nofree }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "used" }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #4 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #5 = { cold nofree noreturn }
attributes #6 = { cold }
