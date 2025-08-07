; ModuleID = './Assignment-2/Tests/testcases/sse/test4.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test4.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %add = add nsw i32 20, 6
  %cmp = icmp eq i32 %add, 26
  call void @svf_assert(i1 noundef zeroext %cmp)
  %sub = sub nsw i32 20, 6
  %cmp1 = icmp eq i32 %sub, 14
  call void @svf_assert(i1 noundef zeroext %cmp1)
  %mul = mul nsw i32 20, 6
  %cmp2 = icmp eq i32 %mul, 120
  call void @svf_assert(i1 noundef zeroext %cmp2)
  %div = sdiv i32 20, 6
  %cmp3 = icmp eq i32 %div, 3
  call void @svf_assert(i1 noundef zeroext %cmp3)
  %rem = srem i32 20, 6
  %cmp4 = icmp eq i32 %rem, 2
  call void @svf_assert(i1 noundef zeroext %cmp4)
  %and = and i32 20, 6
  %cmp5 = icmp eq i32 %and, 4
  call void @svf_assert(i1 noundef zeroext %cmp5)
  %or = or i32 20, 6
  %cmp6 = icmp eq i32 %or, 22
  call void @svf_assert(i1 noundef zeroext %cmp6)
  %xor = xor i32 20, 6
  %cmp7 = icmp eq i32 %xor, 18
  call void @svf_assert(i1 noundef zeroext %cmp7)
  %shl = shl i32 20, 1
  %cmp8 = icmp eq i32 %shl, 40
  call void @svf_assert(i1 noundef zeroext %cmp8)
  %shr = ashr i32 20, 1
  %cmp9 = icmp eq i32 %shr, 10
  call void @svf_assert(i1 noundef zeroext %cmp9)
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
