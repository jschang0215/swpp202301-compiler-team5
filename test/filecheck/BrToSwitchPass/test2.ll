; test for 3 switch case without else
define i32 @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)       
; CHECK-NO:      %cmp = icmp eq i32 %n, 1
; CHECK:         switch i32 %n, label %if.end [
; CHECK-NEXT:      i32 1, label %if.then
; CHECK-NEXT:      i32 2, label %if.then2
; CHECK-NEXT:    ]
entry:
  %cmp = icmp eq i32 %n, 1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %add = add nsw i32 0, 1
  br label %if.end4

; CHECK-NOT:   if.else:
if.else:                                          ; preds = %entry
  %cmp1 = icmp eq i32 %n, 2
  br i1 %cmp1, label %if.then2, label %if.end

if.then2:                                         ; preds = %if.else
  %add3 = add nsw i32 0, 3
  br label %if.end

; CHECK:       if.end: 
; CHECK-NEXT:    %a.0 = phi i32 [ %add3, %if.then2 ], [ 0, %entry ]
; CHECK-NEXT:    br label %if.end4
if.end:                                           ; preds = %if.then2, %if.else
  %a.0 = phi i32 [ %add3, %if.then2 ], [ 0, %if.else ]
  br label %if.end4

if.end4:                                          ; preds = %if.end, %if.then
  %a.1 = phi i32 [ %add, %if.then ], [ %a.0, %if.end ]
  ret i32 %a.1
}

define i32 @main() {
entry:
  %call = call i32 @f(i32 10)
  ret i32 0
}