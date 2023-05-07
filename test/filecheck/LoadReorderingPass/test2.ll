; Test Load instruction reordering
; load store multiple times, move to first of block

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     [[A:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[A]], align 4
; CHECK-NEXT:     [[B:%.*]] = alloca i32, align 4
; CHECK-NEXT:     store i32 0, i32* [[B]], align 4
; CHECK-NEXT:     [[COND:%.*]] = icmp eq i32 0, 0
; CHECK-NEXT:     br i1 [[COND]], label [[BB_TRUE:%.*]], label [[BB_FALSE:%.*]]
; CHECK:       bb_true:
; CHECK-NEXT:     [[K:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[E:%.*]] = load i32, i32* [[A]], align 4
; CHECK-NEXT:     [[C:%.*]] = mul i32 4, 5
; CHECK-NEXT:     [[D:%.*]] = mul i32 [[C]], [[K]]
; CHECK-NEXT:     ret i32 0
; CHECK:       bb_false:
; CHECK-NEXT:     [[L:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     store i32 1, i32* [[B]], align 4
; CHECK-NEXT:     [[M:%.*]] = load i32, i32* [[B]], align 4
; CHECK-NEXT:     [[F:%.*]] = mul i32 [[L]], 4
; CHECK-NEXT:     ret i32 0
entry:
  %a = alloca i32, align 4
  store i32 0, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %b, align 4
  %cond = icmp eq i32 0, 0
  br i1 %cond, label %bb_true, label %bb_false
bb_true:
  %c = mul i32 4, 5
  %k = load i32, i32* %a, align 4
  %d = mul i32 %c, %k
  %e = load i32, i32* %a, align 4
  ret i32 0
bb_false:
  %l = load i32, i32* %b, align 4
  store i32 1, i32* %b, align 4
  %f = mul i32 %l, 4
  %m = load i32, i32* %b, align 4
  ret i32 0
}