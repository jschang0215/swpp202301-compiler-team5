define i64 @main() {
entry:
; CHECK-LABEL: @main(
; CHECK:       entry:
; CHECK-NEXT:  %0 = alloca i64
; CHECK-NEXT:  %1 = alloca i64
; CHECK-NEXT:  %2 = alloca i64
; CHECK-NEXT:  %3 = alloca i64
; CHECK-NEXT:  %4 = alloca i64
; CHECK-NEXT:  %5 = alloca i64
; CHECK-NEXT:  %6 = alloca i64
; CHECK-NEXT:  %7 = alloca i32
; CHECK-NEXT:  %8 = alloca i32
; CHECK-NEXT:  %9 = alloca i32
; CHECK-NEXT:  store i64 0, i64* %0
; CHECK-NEXT:  store i64 0, i64* %1
; CHECK-NEXT:  store i64 0, i64* %2
; CHECK-NEXT:  %x = load i64, i64* %0
; CHECK-NEXT:  store i64 0, i64* %0
; CHECK-NEXT:  store i64 0, i64* %1
; CHECK-NEXT:  store i64 0, i64* %2
; CHECK-NEXT:  store i64 0, i64* %3
; CHECK-NEXT:  store i64 0, i64* %4
; CHECK-NEXT:  %y = load i64, i64* %0
; CHECK:  call i64 @oracle(i64 5, i64 0, i64* %0, i64 0, i64* %1, i64 0, i64* %2, i64 0, i64* %3, i64 0, i64* %4, i64 0, i64* %5, i64 0, i64* %6)
; CHECK-NEXT:  %z = load i64, i64* %0
; CHECK:  call i64 @oracle(i64 1, i64 0, i64* [[p71:%.*]], i64 0, i64* [[p81:%.*]], i64 0, i64* [[p91:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  %t = load i32, i32* %7
; CHECK:  call i64 @oracle(i64 2, i64 0, i64* [[p72:%.*]], i64 0, i64* [[p82:%.*]], i64 0, i64* [[p92:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  %u = load i32, i32* %7
; CHECK:  call i64 @oracle(i64 3, i64 0, i64* [[p73:%.*]], i64 0, i64* [[p83:%.*]], i64 0, i64* [[p93:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  %v = load i32, i32* %7
; CHECK:  call i64 @oracle(i64 4, i64 0, i64* [[p74:%.*]], i64 0, i64* [[p84:%.*]], i64 0, i64* [[p94:%.*]], i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT:  ret i64 0
    %0 = alloca i64
    %1 = alloca i64
    %2 = alloca i64
    %3 = alloca i64
    %4 = alloca i64
    %5 = alloca i64
    %6 = alloca i64
    %7 = alloca i32
    %8 = alloca i32
    %9 = alloca i32
    store i64 0, i64* %0
    store i64 0, i64* %1
    store i64 0, i64* %2
    %x = load i64, i64* %0
    store i64 0, i64* %0
    store i64 0, i64* %1
    store i64 0, i64* %2
    store i64 0, i64* %3
    store i64 0, i64* %4
    %y = load i64, i64* %0
    store i64 0, i64* %0
    store i64 0, i64* %1
    store i64 0, i64* %2
    store i64 0, i64* %3
    store i64 0, i64* %4
    store i64 0, i64* %5
    store i64 0, i64* %6
    %z = load i64, i64* %0
    store i32 0, i32* %7
    store i32 0, i32* %8
    store i32 0, i32* %9
    %t = load i32, i32* %7
    store i32 0, i32* %7
    store i32 0, i32* %8
    store i32 0, i32* %9
    %u = load i32, i32* %7
    store i32 0, i32* %7
    store i32 0, i32* %8
    store i32 0, i32* %9
    %v = load i32, i32* %7
    store i32 0, i32* %7
    store i32 0, i32* %8
    store i32 0, i32* %9
    ret i64 0
}

; CHECK: @oracle(i64 [[ARG:%.*]], i64 [[v1:%.*]], i64* [[p1:%.*]], i64 [[v2:%.*]], i64* [[p2:%.*]], i64 [[v3:%.*]], i64* [[p3:%.*]], i64 [[v4:%.*]], i64* [[p4:%.*]], i64 [[v5:%.*]], i64* [[p5:%.*]], i64 [[v6:%.*]], i64* [[p6:%.*]], i64 [[v7:%.*]], i64* [[p7:%.*]]) {
; CHECK: entry:
; CHECK-NEXT:  switch i64 [[ARG]], label %end [
; CHECK-NEXT:    i64 1, label %L1
; CHECK-NEXT:    i64 2, label %L2
; CHECK-NEXT:    i64 3, label %L3
; CHECK-NEXT:    i64 4, label %L4
; CHECK-NEXT:    i64 5, label %L5
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
; CHECK-NEXT:  ret i64 0
; CHECK:       L2:
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
; CHECK:       L3:
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
; CHECK:       L4:
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
; CHECK: L5:
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
