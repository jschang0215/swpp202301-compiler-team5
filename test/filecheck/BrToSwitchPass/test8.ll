; test for loop case, should not be optimize
define i32 @f(i32 %n) {
; CHECK:       entry:
; CHECK-NEXT:    br label %while.cond 
entry:
  br label %while.cond

; CHECK:       while.cond:
; CHECK-NEXT:    %i = phi i32 [ %i2, %while.body ], [ %n, %entry ]
; CHECK-NEXT:    %tobool = icmp eq i32 %i, 0
; CHECK-NEXT:    br i1 %tobool, label %while.end, label %while.body
while.cond:                                       ; preds = %while.body, %entry
  %i = phi i32 [ %i2, %while.body ], [ %n, %entry ]
  %tobool = icmp eq i32 %i, 0
  br i1 %tobool, label %while.end, label %while.body

; CHECK:       while.body:
; CHECK-NEXT:    %i2 = sub i32 %i, 1
; CHECK-NEXT:    br label %while.cond
while.body:                                       ; preds = %while.cond 
  %i2 = sub i32 %i, 1
  br label %while.cond
  
; CHECK:       while.end:
; CHECK-NEXT:    ret i32 0
while.end:                                        ; preds = %while.cond
  ret i32 0
}

define i32 @main() {
entry:
  %main.ret = call i32 @f(i32 32)
  ret i32 %main.ret
}