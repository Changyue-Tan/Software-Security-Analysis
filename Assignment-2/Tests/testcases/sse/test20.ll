; ModuleID = './Assignment-2/Tests/testcases/sse/test20.ll'
source_filename = "./Assignment-2/Tests/testcases/sse/test20.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

%struct.Data = type { i32, i32, i32 }

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @copy_if_greater(ptr noundef %out, i32 noundef %x, i32 noundef %y) #0 {
entry:
  %cmp = icmp sgt i32 %x, %y
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store i32 %x, ptr %out, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  store i32 %y, ptr %out, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @update_data(ptr noundef %d, i32 noundef %x, i32 noundef %y) #0 {
entry:
  %temp = alloca i32, align 4
  call void @copy_if_greater(ptr noundef %temp, i32 noundef %x, i32 noundef %y)
  %0 = load i32, ptr %temp, align 4
  %a = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 0
  store i32 %0, ptr %a, align 4
  %1 = load i32, ptr %temp, align 4
  %rem = srem i32 %1, 2
  %cmp = icmp eq i32 %rem, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %2 = load i32, ptr %temp, align 4
  %mul = mul nsw i32 %2, 2
  %b = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 1
  store i32 %mul, ptr %b, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %3 = load i32, ptr %temp, align 4
  %add = add nsw i32 %3, 1
  %b1 = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 1
  store i32 %add, ptr %b1, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %a2 = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 0
  %4 = load i32, ptr %a2, align 4
  %b3 = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 1
  %5 = load i32, ptr %b3, align 4
  %add4 = add nsw i32 %4, %5
  %c = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 2
  store i32 %add4, ptr %c, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @process_data(ptr noundef %data, i32 noundef %x, i32 noundef %y, i32 noundef %z) #0 {
entry:
  call void @update_data(ptr noundef %data, i32 noundef %x, i32 noundef %y)
  call void @update_data(ptr noundef %data, i32 noundef %y, i32 noundef %z)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
entry:
  %d = alloca %struct.Data, align 4
  call void @llvm.memset.p0.i64(ptr align 4 %d, i8 0, i64 12, i1 false)
  call void @process_data(ptr noundef %d, i32 noundef 3, i32 noundef 5, i32 noundef 2)
  %a = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 0
  %0 = load i32, ptr %a, align 4
  %cmp = icmp eq i32 %0, 5
  call void @svf_assert(i1 noundef zeroext %cmp)
  %b = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 1
  %1 = load i32, ptr %b, align 4
  %cmp1 = icmp eq i32 %1, 6
  call void @svf_assert(i1 noundef zeroext %cmp1)
  %c = getelementptr inbounds %struct.Data, ptr %d, i32 0, i32 2
  %2 = load i32, ptr %c, align 4
  %cmp2 = icmp eq i32 %2, 11
  call void @svf_assert(i1 noundef zeroext %cmp2)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #1

declare void @svf_assert(i1 noundef zeroext) #2

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 16.0.6"}
