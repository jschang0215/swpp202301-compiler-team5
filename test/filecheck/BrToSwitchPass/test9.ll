; test for non loop case, should be optimize
define i32 @f(i32 %n) {
; CHECK:       entry:
; CHECK-NEXT:    br label %if.cond
entry:
  br label %if.cond


; CHECK:       if.cond:
; CHECK-NO:      %tobool = icmp eq i32 %n, 0
; CHECK-NEXT:      switch i32 %n, label %if.body [
; CHECK-NEXT:      i32 0, label %if.end
; CHECK-NEXT:    ]
if.cond:                                          ; preds = %entry
  %tobool = icmp eq i32 %n, 0
  br i1 %tobool, label %if.end, label %if.body

; CHECK:       if.body: 
; CHECK-NEXT:    br label %if.end
if.body:                                          ; preds = %if.cond
  br label %if.end

; CHECK:       if.end: 
; CHECK-NEXT:    ret i32 0
if.end:                                           ; preds = %if.cond, %if.body
  ret i32 0
}

define i32 @main() {
entry:
  %main.ret = call i32 @f(i32 32)
  ret i32 %main.ret
}