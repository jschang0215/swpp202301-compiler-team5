; test instruction reordering
; pointer dependency

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[PTR:%.*]] = alloca i32*, align 4
; CHECK-NEXT:     [[PTR2:%.*]] = alloca i32*, align 4
; CHECK-NEXT:     [[VALUE:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[VALUE]], align 4
; CHECK-NEXT:     store i32* [[VALUE]], i32** [[PTR]], align 4
; CHECK-NEXT:     [[A:%.*]] = load i32*, i32** [[PTR]], align 4
; CHECK-NEXT:     [[T1:%.*]] = add i32 2, 2
; CHECK-NEXT:     [[T2:%.*]] = add i32 3, 3
; CHECK-NEXT:     store i32* [[A]], i32** [[PTR2]], align 4
; CHECK-NEXT:     [[B:%.*]] = load i32*, i32** [[PTR2]], align 4
; CHECK-NEXT:     store i32 [[T1]], i32* [[A]], align 4
; CHECK-NEXT:     store i32 [[T2]], i32* [[VALUE]], align 4
; CHECK-NEXT:     store i32 1, i32* [[B]], align 4
; CHECK-NEXT:     [[C:%.*]] = load i32, i32* [[VALUE]], align 4
; CHECK-NEXT:     [[D:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     ret i32 0
entry:
  %ptr = alloca i32*, align 4
  %ptr2 = alloca i32*, align 4
  %value = alloca i32, align 4
  store i32 0, i32* %value, align 4
  store i32* %value, i32** %ptr, align 4
  %a = load i32*, i32** %ptr, align 4
  store i32* %a, i32** %ptr2, align 4
  %t1 = add i32 2, 2
  %t2 = add i32 3, 3
  store i32 %t1, i32* %a, align 4
  store i32 %t2, i32* %value, align 4
  %b = load i32*, i32** %ptr2, align 4
  store i32 1, i32* %b, align 4
  %c = load i32, i32* %value, align 4
  %d = load i32, i32* %a, align 4
  ret i32 0
}