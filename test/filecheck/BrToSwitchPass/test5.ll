; test for check with ne
define i32 @f(i32 %n) {
; CHECK:         %cmp = icmp ne i32 %n, 1
; CHECK-NEXT:    switch i32 %n, label %if.then2 [
; CHECK-NEXT:      i32 1, label %if.else4
; CHECK-NEXT:      i32 2, label %if.else
; CHECK-NEXT:    ]
entry:
  %cmp = icmp ne i32 %n, 1
  br i1 %cmp, label %if.then, label %if.else4

; CHECK-NO:   if.then
if.then:                                          ; preds = %entry
  %cmp1 = icmp ne i32 %n, 2
  br i1 %cmp1, label %if.then2, label %if.else

; CHECK:         if.then2
; CHECK-NEXT:      %add = add nsw i32 0, 5
; CHECK-NEXT:      br label %if.end
if.then2:                                         ; preds = %if.then
  %add = add nsw i32 0, 5
  br label %if.end

if.else:                                          ; preds = %if.then
  %add3 = add nsw i32 0, 3
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then2
  %a.0 = phi i32 [ %add, %if.then2 ], [ %add3, %if.else ]
  br label %if.end6

if.else4:                                         ; preds = %entry
  %add5 = add nsw i32 0, 1
  br label %if.end6

if.end6:                                          ; preds = %if.else4, %if.end
  %a.1 = phi i32 [ %a.0, %if.end ], [ %add5, %if.else4 ]
  ret i32 %a.1
}

define i32 @main() {
entry:
  %call = call i32 @f(i32 10)
  ret i32 0
}