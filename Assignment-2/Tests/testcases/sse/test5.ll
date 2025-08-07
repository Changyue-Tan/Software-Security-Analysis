; ModuleID = './Assignment-2/Tests/testcases/sse/test5.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test5.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define void @selectAndStore(ptr noundef %p, ptr noundef %q, i32 noundef %v1, i32 noundef %v2) #0 {
entry:
  %p.addr = alloca ptr, align 8
  %q.addr = alloca ptr, align 8
  %v1.addr = alloca i32, align 4
  %v2.addr = alloca i32, align 4
  store ptr %p, ptr %p.addr, align 8
  store ptr %q, ptr %q.addr, align 8
  store i32 %v1, ptr %v1.addr, align 4
  store i32 %v2, ptr %v2.addr, align 4
  %0 = load i32, ptr %v1.addr, align 4
  %1 = load i32, ptr %v2.addr, align 4
  %cmp = icmp sgt i32 %0, %1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %2 = load i32, ptr %v1.addr, align 4
  %3 = load ptr, ptr %p.addr, align 8
  store i32 %2, ptr %3, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %4 = load i32, ptr %v2.addr, align 4
  %5 = load ptr, ptr %q.addr, align 8
  store i32 %4, ptr %5, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %A = alloca i32, align 4
  %B = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 0, ptr %A, align 4
  store i32 0, ptr %B, align 4
  call void @selectAndStore(ptr noundef %A, ptr noundef %B, i32 noundef 5, i32 noundef 3)
  %0 = load i32, ptr %A, align 4
  %cmp = icmp eq i32 %0, 5
  call void @svf_assert(i1 noundef zeroext %cmp)
  %1 = load i32, ptr %B, align 4
  %cmp1 = icmp eq i32 %1, 0
  call void @svf_assert(i1 noundef zeroext %cmp1)
  ret i32 0
}

declare void @svf_assert(i1 noundef zeroext) #1

attributes #0 = { noinline nounwind optnone ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
