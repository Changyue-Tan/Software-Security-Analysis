; ModuleID = './Assignment-3/Tests/buf//test6.ll'
source_filename = "./Assignment-3/Tests/buf//test6.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [7 x i8] c"abcdef\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %buffer = alloca [10 x i8], align 1
  store i32 0, ptr %retval, align 4
  call void @llvm.memset.p0.i64(ptr align 1 %buffer, i8 0, i64 10, i1 false)
  %arraydecay = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 0
  call void @mem_insert(ptr noundef %arraydecay, ptr noundef @.str, i32 noundef 3, i32 noundef 5)
  %arrayidx = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 5
  %0 = load i8, ptr %arrayidx, align 1
  %conv = sext i8 %0 to i32
  %cmp = icmp eq i32 %conv, 97
  call void @svf_assert(i1 noundef zeroext %cmp)
  %arrayidx2 = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 6
  %1 = load i8, ptr %arrayidx2, align 1
  %conv3 = sext i8 %1 to i32
  %cmp4 = icmp eq i32 %conv3, 98
  call void @svf_assert(i1 noundef zeroext %cmp4)
  %arrayidx6 = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 7
  %2 = load i8, ptr %arrayidx6, align 1
  %conv7 = sext i8 %2 to i32
  %cmp8 = icmp eq i32 %conv7, 99
  call void @svf_assert(i1 noundef zeroext %cmp8)
  %arrayidx10 = getelementptr inbounds [10 x i8], ptr %buffer, i64 0, i64 8
  %3 = load i8, ptr %arrayidx10, align 1
  %conv11 = sext i8 %3 to i32
  %cmp12 = icmp ne i32 %conv11, 100
  call void @svf_assert(i1 noundef zeroext %cmp12)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #1

declare void @mem_insert(ptr noundef, ptr noundef, i32 noundef, i32 noundef) #2

declare void @svf_assert(i1 noundef zeroext) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 16.0.4 (https://github.com/bjjwwang/LLVM-compile a0f79b14ca0360151ea799a9ad1d5932235bbee1)"}
