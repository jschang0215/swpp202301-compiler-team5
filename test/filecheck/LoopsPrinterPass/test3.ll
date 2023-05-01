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
  %cmp = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  store i32 0, i32* %sum, align 4
  %0 = load i32, i32* %n.addr, align 4
  %cmp1 = icmp sgt i32 %0, 0
  %conv = zext i1 %cmp1 to i32
  store i32 %conv, i32* %cmp, align 4
  br label %while.cond

while.cond:
  %1 = load i32, i32* %cmp, align 4
  %tobool = icmp ne i32 %1, 0
  br i1 %tobool, label %while.end7, label %while.body

while.body: 
  br label %while.cond2

while.cond2:                       
  br i1 %tobool, label %while.body4, label %while.end

while.body4:
  br label %while.cond2

while.end:  
  %2 = load i32, i32* %n.addr, align 4
  %3 = load i32, i32* %sum, align 4
  %add = add nsw i32 %3, %2
  store i32 %add, i32* %sum, align 4
  %4 = load i32, i32* %n.addr, align 4
  %sub = sub nsw i32 %4, 1
  store i32 %sub, i32* %n.addr, align 4
  %5 = load i32, i32* %n.addr, align 4
  %cmp5 = icmp sgt i32 %5, 0
  %conv6 = zext i1 %cmp5 to i32
  store i32 %conv6, i32* %cmp, align 4
  br label %while.cond

while.end7:
  %6 = load i32, i32* %sum, align 4
  ret i32 %6
}
