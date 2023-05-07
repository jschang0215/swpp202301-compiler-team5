; Test Load instruction reordering
; 1 large block for just test

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[K:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[L:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[C:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[C]], align 4
; CHECK-NEXT:     [[I:%.*]] = load i32, i32* [[C]], align 4
; CHECK-NEXT:     [[D:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[D]], align 4
; CHECK-NEXT:     [[J:%.*]] = load i32, i32* [[D]], align 4
; CHECK-NEXT:     [[X:%.*]] = add i32 [[I]], [[J]]
; CHECK-NEXT:     [[Y:%.*]] = add i32 [[I]], [[K]]
; CHECK-NEXT:     [[Z:%.*]] = add i32 [[K]], [[L]]
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %c, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %d, align 4
  %i = load i32, i32* %c, align 4
  %j = load i32, i32* %d, align 4
  %x = add i32 %i, %j
  %k = load i32, i32* %a, align 4
  %y = add i32 %i, %k
  %l = load i32, i32* %b, align 4
  %z = add i32 %k, %l
  ret i32 0
}