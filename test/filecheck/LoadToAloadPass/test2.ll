; Test Load to Aload
; check other aloads

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[K:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[USE1:%.*]] = add i32 [[K]], 0
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[L:%.*]] = call i32 @aload_i32(i32* [[B]]
; CHECK-NEXT:     [[M:%.*]] = call i32 @aload_i32(i32* [[B]]
; CHECK-NEXT:     [[N:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[O:%.*]] = call i32 @aload_i32(i32* [[B]]
; CHECK-NEXT:     [[USE2:%.*]] = add i32 [[N]], [[M]]
; CHECK-NEXT:     [[X:%.*]] = mul i32 0, 0
; CHECK-NEXT:     [[Y:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[USE3:%.*]] = add i32 [[L]], [[O]]
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %k = load i32, i32* %a, align 4
  %use1 = add i32 %k, 0
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %l = load i32, i32* %b, align 4
  %m = load i32, i32* %b, align 4
  %n = load i32, i32* %b, align 4
  %o = load i32, i32* %b, align 4
  %use2 = add i32 %n, %m
  %x = mul i32 0, 0
  %y = add i32 0, 0
  %use3 = add i32 %l, %o
  ret i32 0
}