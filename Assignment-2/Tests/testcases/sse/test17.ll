; ModuleID = './Assignment-2/Tests/testcases/sse/test17.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test17.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @max(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %cmp = icmp sgt i32 %a, %b
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %return

if.else:                                          ; preds = %entry
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ %a, %if.then ], [ %b, %if.else ]
  ret i32 %retval.0
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @max_of_three(i32 noundef %x, i32 noundef %y, i32 noundef %z) #0 {
entry:
  %call = call i32 @max(i32 noundef %x, i32 noundef %y)
  %cmp = icmp sgt i32 %call, %z
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %return

if.else:                                          ; preds = %entry
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ %call, %if.then ], [ %z, %if.else ]
  ret i32 %retval.0
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @process(i32 noundef %p, i32 noundef %q, i32 noundef %r) #0 {
entry:
  %call = call i32 @max_of_three(i32 noundef %p, i32 noundef %q, i32 noundef %r)
  %cmp = icmp sgt i32 %call, 10
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %sub = sub nsw i32 %call, 10
  br label %return

if.else:                                          ; preds = %entry
  %sub1 = sub nsw i32 10, %call
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ %sub, %if.then ], [ %sub1, %if.else ]
  ret i32 %retval.0
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %call = call i32 @process(i32 noundef 3, i32 noundef 7, i32 noundef 5)
  %call1 = call i32 @process(i32 noundef 12, i32 noundef 8, i32 noundef 9)
  %cmp = icmp eq i32 %call, 3
  call void @svf_assert(i1 noundef zeroext %cmp)
  %cmp2 = icmp eq i32 %call1, 2
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
