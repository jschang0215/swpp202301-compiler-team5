; Test licm pass
; Delete loop indepedent instruction in double-loop

define dso_local void @f(i32 %n, i32 %x) #0 {
; CHECK:      entry:
; CHECK-NEXT:   %n.addr = alloca i32, align 4
; CHECK-NEXT:   %x.addr = alloca i32, align 4
; CHECK-NEXT:   %s = alloca i32, align 4
; CHECK-NEXT:   %i = alloca i32, align 4
; CHECK-NEXT:   %j = alloca i32, align 4
; CHECK-NEXT:   store i32 %n, i32* %n.addr, align 4
; CHECK-NEXT:   store i32 %x, i32* %x.addr, align 4
; CHECK-NEXT:   store i32 0, i32* %i, align 4
; CHECK-NEXT:   %0 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:   %1 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:   %2 = load i32, i32* %x.addr, align 4
; CHECK-NEXT:   %i.promoted = load i32, i32* %i, align 4
; CHECK-NEXT:   %j.promoted3 = load i32, i32* %j, align 4
; CHECK-NEXT:   %s.promoted5 = load i32, i32* %s, align 1
; CHECK-NEXT:   br label %for.cond
; CHECK:      for.cond:                                         ; preds = %for.inc4, %entry
; CHECK-NEXT:   %.lcssa6 = phi i32 [ %.lcssa, %for.inc4 ], [ %s.promoted5, %entry ]
; CHECK-NEXT:   %inc1.lcssa4 = phi i32 [ %inc1.lcssa, %for.inc4 ], [ %j.promoted3, %entry ]
; CHECK-NEXT:   %inc52 = phi i32 [ %inc5, %for.inc4 ], [ %i.promoted, %entry ]
; CHECK-NEXT:   %cmp = icmp slt i32 %inc52, %0
; CHECK-NEXT:   br i1 %cmp, label %for.body, label %for.end6
; CHECK:      for.body:                                         ; preds = %for.cond
; CHECK-NEXT:   br label %for.cond1
; CHECK:      for.cond1:                                        ; preds = %for.inc, %for.body
; CHECK-NEXT:   %3 = phi i32 [ %2, %for.inc ], [ %.lcssa6, %for.body ]
; CHECK-NEXT:   %inc1 = phi i32 [ %inc, %for.inc ], [ 0, %for.body ]
; CHECK-NEXT:   %cmp2 = icmp slt i32 %inc1, %1
; CHECK-NEXT:   br i1 %cmp2, label %for.body3, label %for.end
; CHECK:      for.body3:                                        ; preds = %for.cond1
; CHECK-NEXT:   br label %for.inc
; CHECK:      for.inc:                                          ; preds = %for.body3
; CHECK-NEXT:   %inc = add nsw i32 %inc1, 1
; CHECK-NEXT:   br label %for.cond1
; CHECK:      for.end:                                          ; preds = %for.cond1
; CHECK-NEXT:   %.lcssa = phi i32 [ %3, %for.cond1 ]
; CHECK-NEXT:   %inc1.lcssa = phi i32 [ %inc1, %for.cond1 ]
; CHECK-NEXT:   br label %for.inc4
; CHECK:      for.inc4:                                         ; preds = %for.end
; CHECK-NEXT:   %inc5 = add nsw i32 %inc52, 1
; CHECK-NEXT:   br label %for.cond
; CHECK:      for.end6:                                         ; preds = %for.cond
; CHECK-NEXT:   %.lcssa6.lcssa = phi i32 [ %.lcssa6, %for.cond ]
; CHECK-NEXT:   %inc1.lcssa4.lcssa = phi i32 [ %inc1.lcssa4, %for.cond ]
; CHECK-NEXT:   %inc52.lcssa = phi i32 [ %inc52, %for.cond ]
; CHECK-NEXT:   store i32 %inc52.lcssa, i32* %i, align 4
; CHECK-NEXT:   store i32 %inc1.lcssa4.lcssa, i32* %j, align 4
; CHECK-NEXT:   store i32 %.lcssa6.lcssa, i32* %s, align 1
; CHECK-NEXT:   ret void
entry:
  %n.addr = alloca i32, align 4
  %x.addr = alloca i32, align 4
  %s = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  store i32 %x, i32* %x.addr, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc4, %entry
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %n.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end6

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %j, align 4
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc, %for.body
  %2 = load i32, i32* %j, align 4
  %3 = load i32, i32* %n.addr, align 4
  %cmp2 = icmp slt i32 %2, %3
  br i1 %cmp2, label %for.body3, label %for.end

for.body3:                                        ; preds = %for.cond1
  %4 = load i32, i32* %x.addr, align 4
  store i32 %4, i32* %s, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body3
  %5 = load i32, i32* %j, align 4
  %inc = add nsw i32 %5, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond1

for.end:                                          ; preds = %for.cond1
  br label %for.inc4

for.inc4:                                         ; preds = %for.end
  %6 = load i32, i32* %i, align 4
  %inc5 = add nsw i32 %6, 1
  store i32 %inc5, i32* %i, align 4
  br label %for.cond

for.end6:                                         ; preds = %for.cond
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