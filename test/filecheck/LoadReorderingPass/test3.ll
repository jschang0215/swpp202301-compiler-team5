; Test Load instruction reordering
; keep orders in load

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[I:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[K:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[L:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[J:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[M:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[X:%.*]] = add i32 [[I]], [[J]]
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %i = load i32, i32* %a, align 4
  %j = load i32, i32* %b, align 4
  %k = load i32, i32* %a, align 4
  %x = add i32 %i, %j
  %l = load i32, i32* %a, align 4
  %m = load i32, i32* %b, align 4
  ret i32 0
}