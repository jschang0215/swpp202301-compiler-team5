; optimization case on recursive branch condition
; should not be optimized
define i64 @gcd(i64 %x, i64 %y) {
; CHECK:       entry:
; CHECK-NEXT:    %cmp = icmp eq i64 %x, 0
; CHECK-NEXT:    br i1 %cmp, label %if.then, label %if.end
entry:
  %cmp = icmp eq i64 %x, 0
  br i1 %cmp, label %if.then, label %if.end

; CHECK:       if.then: 
; CHECK-NEXT:    br label %return
if.then:                                          ; preds = %entry
  br label %return

; CHECK:       if.end:   
; CHECK-NEXT:    %cmp1 = icmp eq i64 %y, 0
; CHECK-NEXT:    br i1 %cmp1, label %if.then2, label %if.end3
if.end:                                           ; preds = %entry
  %cmp1 = icmp eq i64 %y, 0
  br i1 %cmp1, label %if.then2, label %if.end3

; CHECK:       if.then2:   
; CHECK-NEXT:    br label %return
if.then2:                                         ; preds = %if.end
  br label %return

; CHECK:       if.end3:   
; CHECK-NEXT:    %cmp4 = icmp ugt i64 %x, %y
; CHECK-NEXT:    br i1 %cmp4, label %if.then5, label %if.else
if.end3:                                          ; preds = %if.end
  %cmp4 = icmp ugt i64 %x, %y
  br i1 %cmp4, label %if.then5, label %if.else

; CHECK:       if.then5:            
; CHECK-NEXT:    %rem = urem i64 %x, %y
; CHECK-NEXT:    br label %if.end7
if.then5:                                         ; preds = %if.end3
  %rem = urem i64 %x, %y
  br label %if.end7

; CHECK:       if.else: 
; CHECK-NEXT:    %rem6 = urem i64 %y, %x
; CHECK-NEXT:    br label %if.end7
if.else:                                          ; preds = %if.end3
  %rem6 = urem i64 %y, %x
  br label %if.end7

; CHECK:       if.end7:  
; CHECK-NEXT:    %x.addr.0 = phi i64 [ %rem, %if.then5 ], [ %x, %if.else ]
; CHECK-NEXT:    %y.addr.0 = phi i64 [ %y, %if.then5 ], [ %rem6, %if.else ]
; CHECK-NEXT:    %call = call i64 @gcd(i64 %x.addr.0, i64 %y.addr.0)
; CHECK-NEXT:    br label %return
if.end7:                                          ; preds = %if.else, %if.then5
  %x.addr.0 = phi i64 [ %rem, %if.then5 ], [ %x, %if.else ]
  %y.addr.0 = phi i64 [ %y, %if.then5 ], [ %rem6, %if.else ]
  %call = call i64 @gcd(i64 %x.addr.0, i64 %y.addr.0)
  br label %return

; CHECK:       return:    
; CHECK-NEXT:    %retval.0 = phi i64 [ %y, %if.then ], [ %x, %if.then2 ], [ %call, %if.end7 ]
; CHECK-NEXT:    ret i64 %retval.0
return:                                           ; preds = %if.end7, %if.then2, %if.then
  %retval.0 = phi i64 [ %y, %if.then ], [ %x, %if.then2 ], [ %call, %if.end7 ]
  ret i64 %retval.0
}

define i32 @main() #0 {
entry:
  %call = call i64 @gcd(i64 5, i64 10)
  ret i32 0
}
