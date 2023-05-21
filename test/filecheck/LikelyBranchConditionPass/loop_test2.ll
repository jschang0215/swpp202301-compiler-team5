define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     %main.ret = call i32 @f(i32 32)
; CHECK-NEXT:     ret i32 %main.ret
entry:
  %main.ret = call i32 @f(i32 32)
  ret i32 %main.ret
}

define i32 @f(i32 %n) {
entry:
  %n.addr = alloca i32, align 4
  %sum = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  store i32 0, i32* %sum, align 4
  store i32 1, i32* %i, align 4
  br label %for.cond

; CHECK-LABEL:    for.cond:
; CHECK-NEXT:     %0 = load i32, i32* %i, align 4 
; CHECK-NEXT:     %1 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:     %cmp = icmp sgt i32 %0, %1
; CHECK-NEXT:     br i1 %cmp, label %for.end6, label %for.body
for.cond:
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %n.addr, align 4
  %cmp = icmp sle i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end6

for.body:
  store i32 0, i32* %j, align 4
  br label %for.cond1

; CHECK-LABEL:    for.cond1:
; CHECK-NEXT:     %2 = load i32, i32* %j, align 4
; CHECK-NEXT:     %3 = load i32, i32* %i, align 4
; CHECK-NEXT:     %cmp2 = icmp sge i32 %2, %3
; CHECK-NEXT:     br i1 %cmp2, label %for.end, label %for.body3
for.cond1:
  %2 = load i32, i32* %j, align 4
  %3 = load i32, i32* %i, align 4
  %cmp2 = icmp slt i32 %2, %3
  br i1 %cmp2, label %for.body3, label %for.end

for.body3:
  %4 = load i32, i32* %j, align 4
  %5 = load i32, i32* %sum, align 4
  %add = add nsw i32 %5, %4
  store i32 %add, i32* %sum, align 4
  br label %for.inc

for.inc: 
  %6 = load i32, i32* %j, align 4
  %inc = add nsw i32 %6, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond1

for.end: 
  br label %for.inc4

for.inc4:
  %7 = load i32, i32* %i, align 4
  %inc5 = add nsw i32 %7, 1
  store i32 %inc5, i32* %i, align 4
  br label %for.cond

for.end6:
  %8 = load i32, i32* %sum, align 4
  ret i32 %8
}