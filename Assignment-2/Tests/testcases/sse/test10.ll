; ModuleID = './Assignment-2/Tests/testcases/sse/test10.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test10.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %a = alloca i32, align 4
  store i32 1, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %add = add nsw i32 %0, 4
  store i32 %add, ptr %a, align 4
  %mul = mul nsw i32 2, 3
  %1 = load i32, ptr %a, align 4
  %add1 = add nsw i32 %1, %mul
  store i32 %add1, ptr %a, align 4
  %2 = load i32, ptr %a, align 4
  %add2 = add nsw i32 %2, %mul
  %sub = sub nsw i32 %add2, 1
  %3 = load i32, ptr %a, align 4
  %cmp = icmp eq i32 %3, 11
  call void @svf_assert(i1 noundef zeroext %cmp)
  %cmp3 = icmp eq i32 %mul, 6
  call void @svf_assert(i1 noundef zeroext %cmp3)
  %cmp4 = icmp eq i32 %add2, 17
  call void @svf_assert(i1 noundef zeroext %cmp4)
  %cmp5 = icmp eq i32 %sub, 16
  call void @svf_assert(i1 noundef zeroext %cmp5)
  %4 = load i32, ptr %a, align 4
  %cmp6 = icmp eq i32 %4, 11
  call void @svf_assert(i1 noundef zeroext %cmp6)
  %5 = load i32, ptr %a, align 4
  %6 = load i32, ptr %a, align 4
  %cmp7 = icmp eq i32 %5, %6
  call void @svf_assert(i1 noundef zeroext %cmp7)
  %cmp8 = icmp eq ptr %a, %a
  call void @svf_assert(i1 noundef zeroext %cmp8)
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
