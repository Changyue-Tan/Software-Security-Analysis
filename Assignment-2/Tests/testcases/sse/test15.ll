; ModuleID = './Assignment-2/Tests/testcases/sse/test15.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test15.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @update_sum(ptr noundef %out, i32 noundef %a, i32 noundef %b) #0 {
entry:
  %add = add nsw i32 %a, %b
  store i32 %add, ptr %out, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %result1 = alloca i32, align 4
  %result2 = alloca i32, align 4
  %result3 = alloca i32, align 4
  store i32 0, ptr %result1, align 4
  store i32 0, ptr %result2, align 4
  store i32 0, ptr %result3, align 4
  call void @update_sum(ptr noundef %result1, i32 noundef 1, i32 noundef 2)
  call void @update_sum(ptr noundef %result2, i32 noundef 10, i32 noundef 5)
  call void @update_sum(ptr noundef %result3, i32 noundef 7, i32 noundef 8)
  %0 = load i32, ptr %result1, align 4
  %cmp = icmp eq i32 %0, 3
  call void @svf_assert(i1 noundef zeroext %cmp)
  %1 = load i32, ptr %result2, align 4
  %cmp1 = icmp eq i32 %1, 15
  call void @svf_assert(i1 noundef zeroext %cmp1)
  %2 = load i32, ptr %result3, align 4
  %cmp2 = icmp eq i32 %2, 15
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
