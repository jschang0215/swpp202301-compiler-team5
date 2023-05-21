; test instruction reordering
; simple reordering

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[I:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[J:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[C:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[C]], align 4
; CHECK-NEXT:     [[K:%.*]] = load i32, i32* [[C]], align 4
; CHECK-NEXT:     [[D:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[D]], align 4
; CHECK-NEXT:     [[L:%.*]] = load i32, i32* [[D]], align 4
; CHECK-NEXT:     [[X:%.*]] = mul i32 1, 1
; CHECK-NEXT:     [[Y:%.*]] = mul i32 2, 2
; CHECK-NEXT:     [[U:%.*]] = mul i32 [[I]], [[J]]
; CHECK-NEXT:     [[V:%.*]] = mul i32 [[K]], [[U]]
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %i = load i32, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %j = load i32, i32* %b, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %c, align 4
  %k = load i32, i32* %c, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %d, align 4
  %l = load i32, i32* %d, align 4
  %u = mul i32 %i, %j
  %v = mul i32 %k, %u
  %x = mul i32 1, 1
  %y = mul i32 2, 2
  ret i32 0
}