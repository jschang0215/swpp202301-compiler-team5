; sccp test
; eliminate unused block

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
  %cmp = icmp eq i32 0, 0
  br i1 %cmp, label %true, label %false

true:
  %x = load i32, i32* %a, align 4
  %y = load i32, i32* %b, align 4
  %s = add i32 %x, %y
  store i32 %s, i32* %c, align 4
  br label %end

false:
  %z = load i32, i32* %a, align 4
  store i32 %z, i32* %c, align 4
  br label %end
end:
  %r = load i32, i32* %c, align 4
  ret i32 %r
}