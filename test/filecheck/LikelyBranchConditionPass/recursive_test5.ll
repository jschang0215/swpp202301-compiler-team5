define i32 @main() {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @f(i32 10)
  ret i32 0
}


; function that call decreasing n
; should be optimized with select - tobool is used more than once
define void @f(i32 %n) {
; CHECK-LABEL:    entry
; CHECK:            %tobool = icmp ne i32 %1, 0
; CHECK-NEXT:       [[cond:%.*]] = select i1 %tobool, i1 false, i1 true 
; CHECK-NEXT:       br i1 [[cond]], label %if.end, label %if.then
entry:
  %n.addr = alloca i32, align 4
  %k = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32, i32* %n.addr, align 4
  %cmp = icmp ne i32 %0, 0
  %conv = zext i1 %cmp to i32
  store i32 %conv, i32* %k, align 4
  %1 = load i32, i32* %k, align 4
  %tobool = icmp ne i32 %1, 0
  br i1 %tobool, label %if.then, label %if.end

if.then:
  %2 = load i32, i32* %n.addr, align 4
  %sub = sub nsw i32 %2, 1
  call void @f(i32 %sub)
  br label %if.end

if.end:
  %lnot = xor i1 %tobool, true
  %lnot.ext = zext i1 %lnot to i32
  store i32 %lnot.ext, i32* %k, align 4
  ret void
}