; test for 3 switch case with else
define i32 @f(i32 %n) {
; CHECK:       %cmp = icmp eq i32 %n, 1
; CHECK-NEXT:    switch i32 %n, label %if.else4 [
; CHECK-NEXT:      i32 1, label %if.then
; CHECK-NEXT:      i32 2, label %if.then2
; CHECK-NEXT:    ]
entry:
  %cmp = icmp eq i32 %n, 1
  br i1 %cmp, label %if.then, label %if.else

if.then: 
  %add = add nsw i32 0, 1
  br label %if.end6

; CHECK-NOT:   if.else:
if.else:
  %cmp1 = icmp eq i32 %n, 2
  br i1 %cmp1, label %if.then2, label %if.else4

; CHECK:       if.then2:
; CHECK-NEXT:    %add3 = add nsw i32 0, 3
; CHECK-NEXT:    br label %if.end
if.then2:
  %add3 = add nsw i32 0, 3
  br label %if.end

; CHECK:       if.else4:
; CHECK-NEXT:    %add5 = add nsw i32 0, 5
; CHECK-NEXT:    br label %if.end
if.else4:
  %add5 = add nsw i32 0, 5
  br label %if.end

if.end:
  %a.0 = phi i32 [ %add3, %if.then2 ], [ %add5, %if.else4 ]
  br label %if.end6

if.end6:
  %a.1 = phi i32 [ %add, %if.then ], [ %a.0, %if.end ]
  ret i32 %a.1
}

define i32 @main() {
entry:
  %call = call i32 @f(i32 10)
  ret i32 0
}