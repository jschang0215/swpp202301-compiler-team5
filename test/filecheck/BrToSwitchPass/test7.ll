; test for default case has phi node with base
define i32 @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)       
; CHECK-NO:      %cmp = icmp eq i32 %n, 1
; CHECK:         switch i32 %n, label %[[def_bird:.*]] [
; CHECK-NEXT:      i32 1, label %if.then
; CHECK-NEXT:      i32 2, label %[[else_brid:.*]]
; CHECK-NEXT:      i32 3, label %if.then2
; CHECK-NEXT:    ]
entry:
  %cmp = icmp eq i32 %n, 1
  br i1 %cmp, label %if.then, label %if.else

; CHECK:        [[else_brid]]:
; CHEKC-NEXT:     br label %if.then


; CHECK:        [[def_bird]]:
; CHEKC-NEXT:     br label %if.then

; CHECK-NO:     if.else:
if.else: 
  %cmp1 = icmp eq i32 %n, 2
  br i1 %cmp1, label %if.then, label %if.else2

; CHECK-NO:     if.else2:
if.else2:
  %cmp2 = icmp eq i32 %n, 3
  br i1 %cmp2, label %if.then2, label %if.then

; CHECK:        if.then2:
; CHECK-NEXT:     br label %if.then
if.then2:
  br label %if.then

; CHECK:       if.then: 
; CHECK-NEXT:    %a = phi i32 [ 1, %entry ], [ 2, %[[else_brid]] ], [ 3, %[[def_bird]] ], [ 3, %if.then2 ]
if.then: 
  %a = phi i32 [ 1, %entry ], [ 2, %if.else ], [ 3, %if.else2 ], [ 3, %if.then2 ]
  ret i32 %a
}

define i32 @main() {
entry:
  %call = call i32 @f(i32 10)
  ret i32 0
}