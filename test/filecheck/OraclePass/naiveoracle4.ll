; this test checks if oracle optimization in multiple functions works correctly

define i64 @f(i64 %x, i64 %y, i64 %z) {
entry:
; CHECK-LABEL: @f(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i64
; CHECK-NEXT:  %q = alloca i64
; CHECK-NEXT:  %r = alloca i64
; CHECK:  call i64 @oracle(i64 2, i64 [[x:%.*]], i64* [[p:%.*]], i64 [[y:%.*]], i64* [[q:%.*]], i64 [[z:%.*]], i64* [[r:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  %x1 = load i64, i64* [[p]]
; CHECK-NEXT:  %y1 = load i64, i64* [[q]]
; CHECK-NEXT:  %z1 = load i64, i64* [[r]]
; CHECK-NEXT:  %sum = add i64 %x1, %y1
; CHECK-NEXT:  %sum2 = add i64 %sum, %z1
; CHECK-NEXT:  ret i64 %sum2
    %p = alloca i64
    %q = alloca i64
    %r = alloca i64
    store i64 %x, i64* %p
    store i64 %y, i64* %q
    store i64 %z, i64* %r
    %x1 = load i64, i64* %p
    %y1 = load i64, i64* %q
    %z1 = load i64, i64* %r
    %sum = add i64 %x1, %y1
    %sum2 = add i64 %sum, %z1
    ret i64 %sum2
}

define i64 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i64
; CHECK-NEXT:  %q = alloca i64
; CHECK-NEXT:  %r = alloca i64
; CHECK-NEXT:  %s = alloca i64
; CHECK-NEXT:  %sum = call i64 @f(i64 1, i64 2, i64 3)
; CHECK:  call i64 @oracle(i64 1, i64 1, i64* %p, i64 2, i64* %q, i64 3, i64* %r, i64 %sum, i64* %s, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  ret i64 0
    %p = alloca i64
    %q = alloca i64
    %r = alloca i64
    %s = alloca i64
    %sum = call i64 @f(i64 1, i64 2, i64 3)
    store i64 1, i64* %p
    store i64 2, i64* %q
    store i64 3, i64* %r
    store i64 %sum, i64* %s
    ret i64 0
}

; CHECK: @oracle(i64 [[ARG:%.*]], i64 [[v1:%.*]], i64* [[p1:%.*]], i64 [[v2:%.*]], i64* [[p2:%.*]], i64 [[v3:%.*]], i64* [[p3:%.*]], i64 [[v4:%.*]], i64* [[p4:%.*]], i64 [[v5:%.*]], i64* [[p5:%.*]], i64 [[v6:%.*]], i64* [[p6:%.*]], i64 [[v7:%.*]], i64* [[p7:%.*]]) {
; CHECK: entry:
; CHECK-NEXT:  switch i64 [[ARG]], label %end [
; CHECK-NEXT:    i64 1, label %L1
; CHECK-NEXT:    i64 2, label %L2
; CHECK: L1:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  ret i64 0
; CHECK: L2:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  ret i64 0
; CHECK: end:
; CHECK-NEXT:  ret i64 0
