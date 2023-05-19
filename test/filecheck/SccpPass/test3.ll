; sccp test
; constatant propagtion, block elimination

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 1, i32* [[A]], align 4
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 2, i32* [[B]], align 4
; CHECK-NEXT:     [[C:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[C]], align 4
; CHECK-NEXT:     br label [[TRUE:%.*]]
; CHECK:       true:
; CHECK-NEXT:     [[X:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[Y:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[S:%.*]] = add i32 [[X]], [[Y]]
; CHECK-NEXT:     store i32 [[S]], i32* [[C]], align 4
; CHECK-NEXT:     br label [[END:%.*]]
; CHECK:       end:
; CHECK-NEXT:     [[R:%.*]] = load i32, i32* [[C]], align 4
; CHECK-NEXT:     ret i32 [[R]]
entry:
  %a = alloca i32, align 4
  store i32 1, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 2, i32* %b, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %c
  %con1 = add i32 2, 3
  %con2 = add i32 %con1, 2
  %con3 = sub i32 %con2, 2
  %cmp1 = icmp eq i32 %con1, %con3
  %cmp2 = or i1 %cmp1, 0
  br i1 %cmp2, label %true, label %false
true:
  %x = load i32, i32* %a, align 4
  %y = load i32, i32* %b, align 4
  %s = add i32 %x, %y
  store i32 %s, i32* %c, align 4
  %cmp3 = and i1 1, 0
  br i1 %cmp3, label %end, label %next1
false:
  %z = load i32, i32* %a, align 4
  store i32 %z, i32* %c, align 4
  br label %end
next1:
  %x1 = add i32 1, 1
  br label %next2
next2:
  %x2 = add i32 1, 1
  br label %end
end:
  %r = load i32, i32* %c, align 4
  ret i32 %r
}