; Test licm pass
; Delete loop indepedent instruction

define dso_local void @f(i32 %n, i32 %x) #0 {
; CHECK:      entry:
; CHECK-NEXT:   %n.addr = alloca i32, align 4
; CHECK-NEXT:   %x.addr = alloca i32, align 4
; CHECK-NEXT:   %s = alloca i32, align 4
; CHECK-NEXT:   %i = alloca i32, align 4
; CHECK-NEXT:   store i32 %n, i32* %n.addr, align 4
; CHECK-NEXT:   store i32 %x, i32* %x.addr, align 4
; CHECK-NEXT:   store i32 0, i32* %i, align 4
; CHECK-NEXT:   %0 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:   %1 = load i32, i32* %x.addr, align 4
; CHECK-NEXT:   %i.promoted = load i32, i32* %i, align 4
; CHECK-NEXT:   %s.promoted = load i32, i32* %s, align 1
; CHECK-NEXT:   br label %for.cond
; CHECK:      for.cond:                                         ; preds = %for.inc, %entry
; CHECK-NEXT:   %2 = phi i32 [ %1, %for.inc ], [ %s.promoted, %entry ]
; CHECK-NEXT:   %inc1 = phi i32 [ %inc, %for.inc ], [ %i.promoted, %entry ]
; CHECK-NEXT:   %cmp = icmp slt i32 %inc1, %0
; CHECK-NEXT:   br i1 %cmp, label %for.body, label %for.end
; CHECK:      for.body:                                         ; preds = %for.cond
; CHECK-NEXT:   br label %for.inc
; CHECK:      for.inc:                                          ; preds = %for.body
; CHECK-NEXT:   %inc = add nsw i32 %inc1, 1
; CHECK-NEXT:   br label %for.cond
; CHECK:      for.end:                                          ; preds = %for.cond
; CHECK-NEXT:   %.lcssa = phi i32 [ %2, %for.cond ]
; CHECK-NEXT:   %inc1.lcssa = phi i32 [ %inc1, %for.cond ]
; CHECK-NEXT:   store i32 %inc1.lcssa, i32* %i, align 4
; CHECK-NEXT:   store i32 %.lcssa, i32* %s, align 1
; CHECK-NEXT:   ret void
entry:
  %n.addr = alloca i32, align 4
  %x.addr = alloca i32, align 4
  %s = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  store i32 %x, i32* %x.addr, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %n.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load i32, i32* %x.addr, align 4
  store i32 %2, i32* %s, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %3 = load i32, i32* %i, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

define i32 @main() {
; CHECK: entry:
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @f(i32 100, i32 5)
  ret i32 0
}