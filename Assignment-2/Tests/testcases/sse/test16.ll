; ModuleID = './Assignment-2/Tests/testcases/sse/test16.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test16.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @add(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %add = add nsw i32 %a, %b
  ret i32 %add
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @add_three(i32 noundef %x, i32 noundef %y, i32 noundef %z) #0 {
entry:
  %call = call i32 @add(i32 noundef %x, i32 noundef %y)
  %call1 = call i32 @add(i32 noundef %call, i32 noundef %z)
  ret i32 %call1
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @process(i32 noundef %p, i32 noundef %q, i32 noundef %r) #0 {
entry:
  %call = call i32 @add_three(i32 noundef %p, i32 noundef %q, i32 noundef %r)
  ret i32 %call
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %call = call i32 @process(i32 noundef 1, i32 noundef 2, i32 noundef 3)
  %call1 = call i32 @process(i32 noundef 4, i32 noundef 5, i32 noundef 6)
  %cmp = icmp eq i32 %call, 6
  call void @svf_assert(i1 noundef zeroext %cmp)
  %cmp2 = icmp eq i32 %call1, 15
  call void @svf_assert(i1 noundef zeroext %cmp2)
  ret i32 0
}

declare void @svf_assert(i1 noundef zeroext) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
