; ModuleID = './Assignment-2/Tests/testcases/sse/test8.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test8.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @set_two(ptr noundef %p, ptr noundef %q, i32 noundef %v1, i32 noundef %v2) #0 {
entry:
  store i32 %v1, ptr %p, align 4
  store i32 %v2, ptr %q, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @compute(ptr noundef %p, ptr noundef %q, ptr noundef %out_sum, ptr noundef %out_prod) #0 {
entry:
  %0 = load i32, ptr %p, align 4
  %1 = load i32, ptr %q, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, ptr %out_sum, align 4
  %mul = mul nsw i32 %0, %1
  store i32 %mul, ptr %out_prod, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %s = alloca i32, align 4
  %p = alloca i32, align 4
  store i32 0, ptr %x, align 4
  store i32 0, ptr %y, align 4
  call void @set_two(ptr noundef %x, ptr noundef %y, i32 noundef 7, i32 noundef 11)
  %0 = load i32, ptr %x, align 4
  %cmp = icmp eq i32 %0, 7
  call void @svf_assert(i1 noundef zeroext %cmp)
  %1 = load i32, ptr %y, align 4
  %cmp1 = icmp eq i32 %1, 11
  call void @svf_assert(i1 noundef zeroext %cmp1)
  store i32 0, ptr %s, align 4
  store i32 0, ptr %p, align 4
  store i32 3, ptr %x, align 4
  store i32 5, ptr %y, align 4
  call void @compute(ptr noundef %x, ptr noundef %y, ptr noundef %s, ptr noundef %p)
  %2 = load i32, ptr %s, align 4
  %cmp2 = icmp eq i32 %2, 8
  call void @svf_assert(i1 noundef zeroext %cmp2)
  %3 = load i32, ptr %p, align 4
  %cmp3 = icmp eq i32 %3, 15
  call void @svf_assert(i1 noundef zeroext %cmp3)
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
