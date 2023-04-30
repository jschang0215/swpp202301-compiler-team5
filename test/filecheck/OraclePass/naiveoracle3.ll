define i32 @main(i32* %p, i32* %q) {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK:       call i64 @oracle(i64 1, i64 1,
; CHECK:       call i64 @oracle(i64 2, i64 1,
; CHECK:       call i64 @oracle(i64 3, i64 1,
; CHECK-NEXT:  %x = load i32, i32* %p
; CHECK:       call i64 @oracle(i64 4, i64 1,
; CHECK:       call i64 @oracle(i64 5, i64 1,
; CHECK:       call i64 @oracle(i64 6, i64 1,
; CHECK:       call i64 @oracle(i64 7, i64 1,
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  store i32 1, i32* %q
; CHECK-NEXT:  ret i32 0
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    store i32 1, i32* %p
    %x = load i32, i32* %p
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    store i32 1, i32* %q
    ret i32 0
}

; CHECK: @oracle(i64 [[ARG:%.*]], i64 [[v1:%.*]], i64* [[p1:%.*]], i64 [[v2:%.*]], i64* [[p2:%.*]], i64 [[v3:%.*]], i64* [[p3:%.*]], i64 [[v4:%.*]], i64* [[p4:%.*]], i64 [[v5:%.*]], i64* [[p5:%.*]], i64 [[v6:%.*]], i64* [[p6:%.*]], i64 [[v7:%.*]], i64* [[p7:%.*]]) {
; CHECK: entry:
; CHECK-NEXT:  switch i64 [[ARG]], label %end [
; CHECK-NEXT:    i64 1, label %L1
; CHECK-NEXT:    i64 2, label %L2
; CHECK-NEXT:    i64 3, label %L3
; CHECK-NEXT:    i64 4, label %L4
; CHECK-NEXT:    i64 5, label %L5
; CHECK-NEXT:    i64 6, label %L6
; CHECK-NEXT:    i64 7, label %L7
; CHECK:       L1:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       L2:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       L3:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       L4:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       L5:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       L6:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       L7:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  store i64 [[v6]], i64* [[p6]]
; CHECK-NEXT:  store i64 [[v7]], i64* [[p7]]
; CHECK-NEXT:  ret i64 0
; CHECK:       end:
; CHECK-NEXT:  ret i64 0
