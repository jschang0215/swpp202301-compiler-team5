; Test Aload reordering
; complex to check stack or heap

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[PTR:%.*]] = alloca i32*, align 4
; CHECK-NEXT:     [[PTR2:%.*]] = alloca i32*, align 4
; CHECK-NEXT:     [[PTR3:%.*]] = call noalias i32* @malloc(i32 8)
; CHECK-NEXT:     [[VALUE:%.*]] = call noalias i32* @malloc(i32 8)
; CHECK-NEXT:     store i32 0, i32* [[VALUE]], align 4
; CHECK-NEXT:     store i32* [[VALUE]], i32** [[PTR]], align 4
; CHECK-NEXT:     [[A:%.*]] = load i32*, i32** [[PTR]], align 4
; CHECK-NEXT:     store i32* [[A]], i32** [[PTR2]], align 4
; CHECK-NEXT:     store i32 0, i32* [[PTR3]], align 4
; CHECK-NEXT:     [[B:%.*]] = load i32*, i32** [[PTR2]], align 4
; CHECK-NEXT:     store i32 1, i32* [[B]], align 4
; CHECK-NEXT:     br label [[END:%.*]]
; CHECK:       end:
; CHECK-NEXT:     [[A1:%.*]] = call i32 @aload_i32(i32* [[VALUE]])
; CHECK-NEXT:     [[A3:%.*]] = call i32 @aload_i32(i32* [[A]])
; CHECK-NEXT:     [[A2:%.*]] = call i32 @aload_i32(i32* [[PTR3]])
; CHECK-NEXT:     [[A4:%.*]] = call i32 @aload_i32(i32* [[PTR3]])
; CHECK-NEXT:     [[C1:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C2:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C3:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C4:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C5:%.*]] = add i32 0, 0
; CHECK-NEXT:     [[C6:%.*]] = mul i32 0, 0
; CHECK-NEXT:     [[C7:%.*]] = mul i32 0, 0
; CHECK-NEXT:     [[U1:%.*]] = add i32 [[A1]], [[A4]]
; CHECK-NEXT:     [[U2:%.*]] = add i32 [[A2]], [[A3]]
; CHECK-NEXT:     call void @free(i32* [[PTR3]])
; CHECK-NEXT:     call void @free(i32* [[VALUE]])
; CHECK-NEXT:     ret i32 0
entry:
  %ptr = alloca i32*, align 4
  %ptr2 = alloca i32*, align 4
  %ptr3 = call noalias i32* @malloc(i32 8)
  %value = call noalias i32* @malloc(i32 8)
  store i32 0, i32* %value, align 4
  store i32* %value, i32** %ptr, align 4
  %a = load i32*, i32** %ptr, align 4
  store i32* %a, i32** %ptr2, align 4
  store i32 0, i32* %ptr3, align 4
  %b = load i32*, i32** %ptr2, align 4
  store i32 1, i32* %b, align 4
  br label %end
end:
  %a1 = call i32 @aload_i32(i32* %value)
  %a2 = call i32 @aload_i32(i32* %ptr3) 
  %a3 = call i32 @aload_i32(i32* %a)
  %a4 = call i32 @aload_i32(i32* %ptr3)
  %c1 = add i32 0, 0
  %c2 = add i32 0, 0
  %c3 = add i32 0, 0
  %c4 = add i32 0, 0
  %c5 = add i32 0, 0
  %c6 = mul i32 0, 0
  %c7 = mul i32 0, 0
  %u1 = add i32 %a1, %a4
  %u2 = add i32 %a2, %a3
  call void @free(i32* %ptr3)
  call void @free(i32* %value)
  ret i32 0
}

declare i32 @aload_i32(i32*)
declare noalias i32* @malloc(i32)
declare void @free(i32*)