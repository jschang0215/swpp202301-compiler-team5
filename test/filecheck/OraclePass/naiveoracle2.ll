define i32 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %p = alloca i32
; CHECK-NEXT:  %q = alloca i32
; CHECK-NEXT:  %u = alloca i32
; CHECK-NEXT:  %r = alloca i32
; CHECK-NEXT:  %s = alloca i32
; CHECK-NEXT:  %t = alloca i32
; CHECK-NEXT:  %v = alloca i32
; CHECK:  call i64 @oracle(i64 2, i64 1, i64* [[p:%.*]], i64 1, i64* [[q:%.*]], i64 2, i64* [[u:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  %x = load i32, i32* %p
; CHECK:  call i64 @oracle(i64 1, i64 1, i64* [[r:%.*]], i64 1, i64* [[s:%.*]], i64 1, i64* [[t:%.*]], i64 3, i64* [[v:%.*]],  i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  ret i32 0
    %p = alloca i32
    %q = alloca i32
    %u = alloca i32
    %r = alloca i32
    %s = alloca i32
    %t = alloca i32
    %v = alloca i32
    store i32 1, i32* %p
    store i32 1, i32* %q
    store i32 2, i32* %u
    %x = load i32, i32* %p
    store i32 1, i32* %r
    store i32 1, i32* %s
    store i32 1, i32* %t
    store i32 3, i32* %v
    ret i32 0
}

; CHECK: @oracle(i64 [[ARG:%.*]], i64 [[v1:%.*]], i64* [[p1:%.*]], i64 [[v2:%.*]], i64* [[p2:%.*]], i64 [[v3:%.*]], i64* [[p3:%.*]], i64 [[v4:%.*]], i64* [[p4:%.*]], i64 [[v5:%.*]], i64* [[p5:%.*]], i64 [[v6:%.*]], i64* [[p6:%.*]], i64 [[v7:%.*]], i64* [[p7:%.*]]) {
; CHECK: entry:
; CHECK-NEXT:  switch i64 [[ARG]], label %end [
; CHECK-NEXT:    i64 1, label %L1
; CHECK-NEXT:    i64 2, label %L2
; CHECK: L1:
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
; CHECK-NEXT:  ret i64 0
; CHECK: L2:
; CHECK-NEXT:  [[v1t:%.*]] = trunc i64 [[v1]] to i32
; CHECK-NEXT:  [[p1t:%.*]] = bitcast i64* [[p1]] to i32*
; CHECK-NEXT:  store i32 [[v1t]], i32* [[p1t]]
; CHECK-NEXT:  [[v2t:%.*]] = trunc i64 [[v2]] to i32
; CHECK-NEXT:  [[p2t:%.*]] = bitcast i64* [[p2]] to i32*
; CHECK-NEXT:  store i32 [[v2t]], i32* [[p2t]]
; CHECK-NEXT:  [[v3t:%.*]] = trunc i64 [[v3]] to i32
; CHECK-NEXT:  [[p3t:%.*]] = bitcast i64* [[p3]] to i32*
; CHECK-NEXT:  store i32 [[v3t]], i32* [[p3t]]
; CHECK-NEXT:  ret i64 0
; CHECK: end:
; CHECK-NEXT:  ret i64 0
