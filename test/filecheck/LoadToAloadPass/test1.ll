; Test Load to Aload
; load and no use

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[K:%.*]] = call i32 @aload_i32(i32* [[A]]
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[L:%.*]] = call i32 @aload_i32(i32* [[B]]
; CHECK-NEXT:     [[X:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[Y:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[Z:%.*]] = add i32 0, 0
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %k = load i32, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %l = load i32, i32* %b, align 4
  %x = add i32 0, 0
  %y = add i32 0, 0
  %z = add i32 0, 0
  ret i32 0
}