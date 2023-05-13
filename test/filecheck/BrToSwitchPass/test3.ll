; test for 4 switch case without else
define i32 @f(i32 %n) {
; CHECK-LABEL: @f(i32 %n)       
; CHECK-NO:      %cmp = icmp eq i32 %n, 1
; CHECK:         switch i32 %n, label %if.end [
; CHECK-NEXT:      i32 1, label %if.then
; CHECK-NEXT:      i32 2, label %if.then2
; CHECK-NEXT:      i32 3, label %if.then6
; CHECK-NEXT:    ]
entry:
  %cmp = icmp eq i32 %n, 1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %add = add nsw i32 0, 1
  br label %if.end9

; CHECK-NO:    if.else
if.else:                                          ; preds = %entry
  %cmp1 = icmp eq i32 %n, 2
  br i1 %cmp1, label %if.then2, label %if.else4

if.then2:                                         ; preds = %if.else
  %add3 = add nsw i32 0, 3
  br label %if.end8

; CHECK-NO:    if.else4
if.else4:                                         ; preds = %if.else
  %cmp5 = icmp eq i32 %n, 3
  br i1 %cmp5, label %if.then6, label %if.end

if.then6:                                         ; preds = %if.else4
  %add7 = add nsw i32 0, 5
  br label %if.end

; CHECK:       if.end: 
; CHECK-NEXT:    %a.0 = phi i32 [ %add7, %if.then6 ], [ 0, %entry ]
; CHECK-NEXT:    br label %if.end8
if.end:                                           ; preds = %if.then6, %if.else4
  %a.0 = phi i32 [ %add7, %if.then6 ], [ 0, %if.else4 ]
  br label %if.end8

if.end8:                                          ; preds = %if.end, %if.then2
  %a.1 = phi i32 [ %add3, %if.then2 ], [ %a.0, %if.end ]
  br label %if.end9

if.end9:                                          ; preds = %if.end8, %if.then
  %a.2 = phi i32 [ %add, %if.then ], [ %a.1, %if.end8 ]
  ret i32 %a.2
}

define i32 @main() {
entry:
  %call = call i32 @f(i32 10)
  ret i32 0
}