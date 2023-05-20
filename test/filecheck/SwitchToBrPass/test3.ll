define i32 @main() {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @f(i32 10)
  ret i32 0
}


define void @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)
; CHECK-NEXT:  entry:
; CHECK-NEXT:    %n.addr = alloca i32, align 4
; CHECK-NEXT:    store i32 %n, i32* %n.addr, align 4
; CHECK-NEXT:    %0 = load i32, i32* %n.addr, align 4
; CHECK-NEXT:    switch i32 %0, label %sw.default [
; CHECK-NEXT:      i32 1, label %sw.bb
; CHECK-NEXT:      i32 2, label %sw.bb1
; CHECK-NEXT:    ]
entry:
  %n.addr = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32, i32* %n.addr, align 4
  switch i32 %0, label %sw.default [
    i32 1, label %sw.bb
    i32 2, label %sw.bb1
  ]

sw.bb:
  %1 = load i32, i32* %n.addr, align 4
  %add = add nsw i32 %1, 10
  store i32 %add, i32* %n.addr, align 4
  br label %sw.epilog

sw.bb1: 
  %2 = load i32, i32* %n.addr, align 4
  %add2 = add nsw i32 %2, 20
  store i32 %add2, i32* %n.addr, align 4
  br label %sw.epilog

sw.default: 
  %3 = load i32, i32* %n.addr, align 4
  %add3 = add nsw i32 %3, 1
  store i32 %add3, i32* %n.addr, align 4
  br label %sw.epilog

sw.epilog:
  ret void
}