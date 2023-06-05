; ModuleID = '/tmp/a.ll'
source_filename = "gcd/src/gcd.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define dso_local i64 @gcd(i64 noundef %x, i64 noundef %y) #0 {
; CHECK: entry:
entry:
  %cmp = icmp eq i64 %x, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  br label %return

if.end:                                           ; preds = %entry
  %cmp1 = icmp eq i64 %y, 0
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  br label %return

if.end3:                                          ; preds = %if.end
  %cmp4 = icmp ugt i64 %x, %y
  br i1 %cmp4, label %if.then5, label %if.else

if.then5:                                         ; preds = %if.end3
  %rem = urem i64 %x, %y
  br label %if.end7

if.else:                                          ; preds = %if.end3
  %rem6 = urem i64 %y, %x
  br label %if.end7

if.end7:                                          ; preds = %if.else, %if.then5
  %x.addr.0 = phi i64 [ %rem, %if.then5 ], [ %x, %if.else ]
  %y.addr.0 = phi i64 [ %y, %if.then5 ], [ %rem6, %if.else ]
  %call = call i64 @gcd(i64 noundef %x.addr.0, i64 noundef %y.addr.0)
  br label %return

return:                                           ; preds = %if.end7, %if.then2, %if.then
  %retval.0 = phi i64 [ %y, %if.then ], [ %x, %if.then2 ], [ %call, %if.end7 ]
  ret i64 %retval.0
}

; Function Attrs: nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %call = call i64 (...) @read()
  %call1 = call i64 (...) @read()
  %call2 = call i64 @gcd(i64 noundef %call, i64 noundef %call1)
  call void @write(i64 noundef %call2)
  ret i32 0
}

; Function Attrs: argmemonly nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

declare i64 @read(...) #2

declare void @write(i64 noundef) #2

; Function Attrs: argmemonly nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #1

attributes #0 = { nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly nocallback nofree nosync nounwind willreturn }
attributes #2 = { "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 15.0.7 (https://github.com/llvm/llvm-project.git 8dfdcc7b7bf66834a761bd8de445840ef68e4d1a)"}