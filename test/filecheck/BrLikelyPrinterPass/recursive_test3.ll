define i32 @main() {
; CHECK:       entry:
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call i32 @f(i32 20)
  ret i32 0
}

; Collatz Conjecture
; both branch has same priority (same number of recursive call)
define i32 @f(i32 %n) {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32, i32* %n.addr, align 4
  %cmp = icmp eq i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end

if.then:
  store i32 0, i32* %retval, align 4
  br label %return
if.end:
  %1 = load i32, i32* %n.addr, align 4
  %rem = srem i32 %1, 2
  %cmp1 = icmp eq i32 %rem, 0
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:
  %2 = load i32, i32* %n.addr, align 4
  %div = sdiv i32 %2, 2
  %call = call i32 @f(i32 noundef %div)
  %add = add nsw i32 %call, 1
  store i32 %add, i32* %retval, align 4
  br label %return

if.end3:
  %3 = load i32, i32* %n.addr, align 4
  %mul = mul nsw i32 %3, 3
  %add4 = add nsw i32 %mul, 1
  %call5 = call i32 @f(i32 noundef %add4)
  %add6 = add nsw i32 %call5, 1
  store i32 %add6, i32* %retval, align 4
  br label %return

return:
  %4 = load i32, i32* %retval, align 4
  ret i32 %4
}

