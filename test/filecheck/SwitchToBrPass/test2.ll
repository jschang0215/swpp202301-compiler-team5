define i32 @main() {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @f(i32 noundef 10)
  ret i32 0
}

define void @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)
; CHECK-NEXT:  entry:
; CHECK-NEXT:    %n.addr = alloca i32, align 4
; CHECK-NEXT:    store i32 %n, i32* %n.addr, align 4
; CHECK-NEXT:    %0 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:    [[cond:%.*]] = icmp eq i32 1, %0
; CHECK-NEXT:    br i1 [[cond]], label %sw.bb, label %sw.default
entry:
  %n.addr = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32, i32* %n.addr, align 4
  switch i32 %0, label %sw.default [
    i32 1, label %sw.bb
  ]

sw.bb: 
  %1 = load i32, i32* %n.addr, align 4
  %add = add nsw i32 %1, 10
  store i32 %add, i32* %n.addr, align 4
  br label %sw.default

sw.default: 
  %2 = load i32, i32* %n.addr, align 4
  %add1 = add nsw i32 %2, 1
  store i32 %add1, i32* %n.addr, align 4
  br label %sw.epilog

sw.epilog:
  ret void
}

