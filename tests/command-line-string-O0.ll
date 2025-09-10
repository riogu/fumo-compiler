; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%foo = type { i32 }

define void @fumo.init() #0 {
  ret void
}

define internal i32 @fumo.user_main() {
  %"main()::var" = alloca %foo, align 8
  %1 = alloca %foo, align 8
  store %foo zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %foo, ptr %1, i32 0, i32 0
  store i32 69, ptr %2, align 4
  %3 = load %foo, ptr %1, align 4
  store %foo %3, ptr %"main()::var", align 4
  %4 = getelementptr inbounds nuw %foo, ptr %"main()::var", i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  ret i32 %5
}

define i32 @main(i32 %argc, ptr %argv) #0 {
entry:
  call void @fumo.init()
  %0 = call i32 @fumo.user_main()
  ret i32 %0
}

attributes #0 = { "used" }
