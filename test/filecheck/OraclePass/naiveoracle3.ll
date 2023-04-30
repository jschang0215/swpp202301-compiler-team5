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
; CHECK-NEXT:  [[v1t:%.*]] = trunc i64 [[v1]] to i32
; CHECK-NEXT:  [[p1t:%.*]] = bitcast i64* [[p1]] to i32*
; CHECK-NEXT:  store i32 [[v1t]], i32* [[p1t]]
; CHECK-NEXT:  [[v2t:%.*]] = trunc i64 [[v2]] to i32
; CHECK-NEXT:  [[p2t:%.*]] = bitcast i64* [[p2]] to i32*
; CHECK-NEXT:  store i32 [[v2t]], i32* [[p2t]]
; CHECK-NEXT:  [[v3t:%.*]] = trunc i64 [[v3]] to i32
; CHECK-NEXT:  [[p3t:%.*]] = bitcast i64* [[p3]] to i32*
; CHECK-NEXT:  store i32 [[v3t]], i32* [[p3t]]
; CHECK-NEXT:  [[v4t:%.*]] = trunc i64 [[v4]] to i32
; CHECK-NEXT:  [[p4t:%.*]] = bitcast i64* [[p4]] to i32*
; CHECK-NEXT:  store i32 [[v4t]], i32* [[p4t]]
; CHECK-NEXT:  [[v5t:%.*]] = trunc i64 [[v5]] to i32
; CHECK-NEXT:  [[p5t:%.*]] = bitcast i64* [[p5]] to i32*
; CHECK-NEXT:  store i32 [[v5t]], i32* [[p5t]]
; CHECK-NEXT:  [[v6t:%.*]] = trunc i64 [[v6]] to i32
; CHECK-NEXT:  [[p6t:%.*]] = bitcast i64* [[p6]] to i32*
; CHECK-NEXT:  store i32 [[v6t]], i32* [[p6t]]
; CHECK-NEXT:  [[v7t:%.*]] = trunc i64 [[v7]] to i32
; CHECK-NEXT:  [[p7t:%.*]] = bitcast i64* [[p7]] to i32*
; CHECK-NEXT:  store i32 [[v7t]], i32* [[p7t]]
; CHECK-NEXT:  ret i64 0
; CHECK:       end:
; CHECK-NEXT:  ret i64 0
