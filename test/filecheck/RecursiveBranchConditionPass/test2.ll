define i32 @main() {
; CHECK:       entry:
; CHECK-NEXT:     %main.ret = call i32 @f(i32 32)
; CHECK-NEXT:     ret i32 %main.ret
entry:
  %main.ret = call i32 @f(i32 32)
  ret i32 %main.ret
}

; function that return sum n to 0 with recursion
; should be optimized - recursion when true basic block
define i32 @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)
; CHECK:       entry:
; CHECK-NEXT:    %retval = alloca i32, align 4
; CHECK-NEXT:    %n.addr = alloca i32, align 4
; CHECK-NEXT:    store i32 %n, i32* %n.addr, align 4
; CHECK-NEXT:    %0 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:    %cmp = icmp eq i32 %0, 0
; CHECK-NEXT:    br i1 %cmp, label %if.then, label %if.end
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32, i32* %n.addr, align 4
  %cmp = icmp ne i32 %0, 0
  br i1 %cmp, label %if.end, label %if.then

if.then:
  store i32 0, i32* %retval, align 4
  br label %return

if.end:
  %1 = load i32, i32* %n.addr, align 4
  %sub = sub nsw i32 %1, 1
  %call = call i32 @f(i32 %sub)
  %2 = load i32, i32* %n.addr, align 4
  %add = add nsw i32 %call, %2
  store i32 %add, i32* %retval, align 4
  br label %return

return:
  %3 = load i32, i32* %retval, align 4
  ret i32 %3
}