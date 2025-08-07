; ModuleID = './Assignment-2/Tests/testcases/sse/test19.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test19.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @update_value(ptr noundef %ptr, i32 noundef %delta) #0 {
entry:
  %0 = load i32, ptr %ptr, align 4
  %cmp = icmp sgt i32 %0, 10
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %add = add nsw i32 %0, %delta
  br label %if.end

if.else:                                          ; preds = %entry
  %sub = sub nsw i32 %0, %delta
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %val.0 = phi i32 [ %add, %if.then ], [ %sub, %if.else ]
  store i32 %val.0, ptr %ptr, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @conditional_update(ptr noundef %ptr, i32 noundef %x) #0 {
entry:
  %rem = srem i32 %x, 2
  %cmp = icmp eq i32 %rem, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %if.end

if.else:                                          ; preds = %entry
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %delta.0 = phi i32 [ 5, %if.then ], [ 3, %if.else ]
  call void @update_value(ptr noundef %ptr, i32 noundef %delta.0)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @process(ptr noundef %p, i32 noundef %a, i32 noundef %b) #0 {
entry:
  call void @conditional_update(ptr noundef %p, i32 noundef %a)
  call void @conditional_update(ptr noundef %p, i32 noundef %b)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %value = alloca i32, align 4
  store i32 12, ptr %value, align 4
  call void @process(ptr noundef %value, i32 noundef 4, i32 noundef 7)
  %0 = load i32, ptr %value, align 4
  %cmp = icmp eq i32 %0, 20
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
