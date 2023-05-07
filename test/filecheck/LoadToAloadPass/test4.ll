; Test Load to Aload
; check function calls

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[K:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[C:%.*]] = mul i32 1, 2
; CHECK-NEXT:     [[D:%.*]] = call i32 @incr_i32(i32 [[C]])
; CHECK-NEXT:     [[USE1:%.*]] = add i32 [[K]], 0
; CHECK-NEXT:     ret i32 0
;
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %k = load i32, i32* %a, align 4
  %c = mul i32 1, 2
  %d = call i32 @incr_i32(i32 %c)
  %use1 = add i32 %k, 0
  ret i32 0
}

declare i32 @incr_i32(i32)