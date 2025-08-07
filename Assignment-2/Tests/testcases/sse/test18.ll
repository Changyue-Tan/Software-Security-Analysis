; ModuleID = './Assignment-2/Tests/testcases/sse/test18.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test18.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @increment(ptr noundef %ptr, i32 noundef %amount) #0 {
entry:
  %0 = load i32, ptr %ptr, align 4
  %add = add nsw i32 %0, %amount
  store i32 %add, ptr %ptr, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @double_increment(ptr noundef %ptr, i32 noundef %inc1, i32 noundef %inc2) #0 {
entry:
  call void @increment(ptr noundef %ptr, i32 noundef %inc1)
  call void @increment(ptr noundef %ptr, i32 noundef %inc2)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @process(ptr noundef %p, i32 noundef %a, i32 noundef %b) #0 {
entry:
  call void @double_increment(ptr noundef %p, i32 noundef %a, i32 noundef %b)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %value = alloca i32, align 4
  store i32 5, ptr %value, align 4
  call void @process(ptr noundef %value, i32 noundef 3, i32 noundef 4)
  %0 = load i32, ptr %value, align 4
  %cmp = icmp eq i32 %0, 12
  call void @svf_assert(i1 noundef zeroext %cmp)
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
