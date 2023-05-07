define i32 @main() {
; CHECK:       entry:
; CHECK-NEXT:     %main.ret = call i32 @f(i32 32)
; CHECK-NEXT:     ret i32 %main.ret
entry:
  %main.ret = call i32 @f(i32 32)
  ret i32 %main.ret
}

define i32 @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)
entry:
  %n.addr = alloca i32, align 4
  %sum = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  store i32 0, i32* %sum, align 4
  store i32 1, i32* %i, align 4
  br label %for.cond

; CHECK:       for.cond:
; CHECK-NEXT:     %0 = load i32, i32* %i, align 4 
; CHECK-NEXT:     %1 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:     %cmp = icmp sgt i32 %0, %1
; CHECK-NEXT:     br i1 %cmp, label %for.end, label %for.body
for.cond: 
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %n.addr, align 4
  %cmp = icmp sle i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %2 = load i32, i32* %i, align 4
  %3 = load i32, i32* %sum, align 4
  %add = add nsw i32 %3, %2
  store i32 %add, i32* %sum, align 4
  br label %for.inc

for.inc: 
  %4 = load i32, i32* %i, align 4
  %inc = add nsw i32 %4, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end: 
  %5 = load i32, i32* %sum, align 4
  ret i32 %5
}
