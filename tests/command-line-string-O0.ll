; ModuleID = 'tests/command-line-string.out'
source_filename = "command-line-string.fm"

%foo = type { i32 }

define void @fumo.init() #0 {
  ret void
}

define internal void @fumo.user_main() {
  %"main()::var" = alloca %foo, align 8
  %1 = alloca %foo, align 8
  store %foo zeroinitializer, ptr %1, align 4
  %2 = getelementptr inbounds nuw %foo, ptr %1, i32 0, i32 0
  store i32 69, ptr %2, align 4
  %3 = load %foo, ptr %1, align 4
  store %foo %3, ptr %"main()::var", align 4
  ret void
}

define i32 @main(i32 %argc, ptr %argv) #0 {
entry:
  call void @fumo.init()
  call void @fumo.user_main()
  ret i32 0
}

attributes #0 = { "used" }
