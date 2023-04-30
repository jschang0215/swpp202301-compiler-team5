define i32 @main(i32* %p, i32* %q, i32* %r, i32* %s, i32* %t) {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  store i32 1, i32* %p
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  %x = load i32, i32* %p
; CHECK:  call i64 @oracle(i64 1, i64 1, i64* [[r:%.*]], i64 1, i64* [[s:%.*]], i64 1, i64* [[t:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  ret i32 0
    store i32 1, i32* %p
    store i32 1, i32* %q
    %x = load i32, i32* %p
    store i32 1, i32* %r
    store i32 1, i32* %s
    store i32 1, i32* %t
    ret i32 0
}

; CHECK: @oracle(i64 [[ARG:%.*]], i64 [[v1:%.*]], i64* [[p1:%.*]], i64 [[v2:%.*]], i64* [[p2:%.*]], i64 [[v3:%.*]], i64* [[p3:%.*]], i64 [[v4:%.*]], i64* [[p4:%.*]], i64 [[v5:%.*]], i64* [[p5:%.*]], i64 [[v6:%.*]], i64* [[p6:%.*]], i64 [[v7:%.*]], i64* [[p7:%.*]]) {
; CHECK: entry:
; CHECK-NEXT:  switch i64 [[ARG]], label %end [
; CHECK-NEXT:    i64 1, label %L1
; CHECK: L1:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  ret i64 0
; CHECK: end:
; CHECK-NEXT:  ret i64 0
