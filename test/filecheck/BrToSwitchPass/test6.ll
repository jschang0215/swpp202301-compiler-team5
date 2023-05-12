; test for switch case has phi node with base
define i32 @f(i32 %n) {
; CHECK:         %cmp = icmp eq i32 %n, 1
; CHECK-NEXT:    switch i32 %n, label %if.else2 [
; CHECK-NEXT:      i32 1, label %if.then
; CHECK-NEXT:      i32 2, label %if.else1
; CHECK-NEXT:    ]
entry:
  %cmp = icmp eq i32 %n, 1
  br i1 %cmp, label %if.then, label %if.else

; CHECK:       [[else:.*]]: 
; CHECK-NEXT:    br label %if.then
if.else: 
  %cmp1 = icmp eq i32 %n, 2
  br i1 %cmp1, label %if.then, label %if.else2

if.else2:
  br label %if.then

; CHECK:       if.then: 
; CHECK-NEXT:    %a = phi i32 [ 1, %entry ], [ 2, %[[else]] ], [ 3, %if.else2 ]
if.then: 
  %a = phi i32 [ 1, %entry ], [ 2, %if.else ], [ 3, %if.else2 ]
  ret i32 %a
}

define i32 @main() {
entry:
  %call = call i32 @f(i32 10)
  ret i32 0
}