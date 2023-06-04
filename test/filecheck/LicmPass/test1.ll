; Test licm pass
; Replace %a with constant value 5 in loop

define i32 @main() {
; CHECK:      entry:
; CHECK-NEXT:   %retval = alloca i32, align 4
; CHECK-NEXT:   %n = alloca i32, align 4
; CHECK-NEXT:   %sum = alloca i32, align 4
; CHECK-NEXT:   %a = alloca i32, align 4
; CHECK-NEXT:   %i = alloca i32, align 4
; CHECK-NEXT:   store i32 0, i32* %retval, align 4
; CHECK-NEXT:   store i32 100, i32* %n, align 4
; CHECK-NEXT:   store i32 0, i32* %sum, align 4
; CHECK-NEXT:   store i32 5, i32* %a, align 4
; CHECK-NEXT:   store i32 0, i32* %i, align 4
; CHECK-NEXT:   %0 = load i32, i32* %n, align 4
; CHECK-NEXT:   %1 = load i32, i32* %a, align 4
; CHECK-NEXT:   %i.promoted = load i32, i32* %i, align 4
; CHECK-NEXT:   %sum.promoted = load i32, i32* %sum, align 4
; CHECK-NEXT:   br label %for.cond
; CHECK:      for.cond:                                         ; preds = %for.inc, %entry
; CHECK-NEXT:   %add2 = phi i32 [ %add, %for.inc ], [ %sum.promoted, %entry ]
; CHECK-NEXT:   %inc1 = phi i32 [ %inc, %for.inc ], [ %i.promoted, %entry ]
; CHECK-NEXT:   %cmp = icmp slt i32 %inc1, %0
; CHECK-NEXT:   br i1 %cmp, label %for.body, label %for.end
; CHECK:      for.body:                                         ; preds = %for.cond
; CHECK-NEXT:   %add = add nsw i32 %add2, %1
; CHECK-NEXT:   br label %for.inc
; CHECK:      for.inc:                                          ; preds = %for.body
; CHECK-NEXT:   %inc = add nsw i32 %inc1, 1
; CHECK-NEXT:   br label %for.cond
; CHECK:      for.end:                                          ; preds = %for.cond
; CHECK-NEXT:   %add2.lcssa = phi i32 [ %add2, %for.cond ]
; CHECK-NEXT:   %inc1.lcssa = phi i32 [ %inc1, %for.cond ]
; CHECK-NEXT:   store i32 %inc1.lcssa, i32* %i, align 4
; CHECK-NEXT:   store i32 %add2.lcssa, i32* %sum, align 4
; CHECK-NEXT:   ret i32 0

entry:
  %retval = alloca i32, align 4
  %n = alloca i32, align 4
  %sum = alloca i32, align 4
  %a = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 100, i32* %n, align 4
  store i32 0, i32* %sum, align 4
  store i32 5, i32* %a, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %n, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load i32, i32* %a, align 4
  %3 = load i32, i32* %sum, align 4
  %add = add nsw i32 %3, %2
  store i32 %add, i32* %sum, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %4 = load i32, i32* %i, align 4
  %inc = add nsw i32 %4, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret i32 0
}