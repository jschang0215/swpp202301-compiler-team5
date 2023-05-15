define i64 @main() {
; CHECK-LABEL: @main()
entry:
; CHECK: entry:
; CHECK-NEXT: %p = alloca i64
; CHECK-NEXT: call i64 @oracle(i64 3, i64 0, i64* %p, i64 1, i64* %p, i64 2, i64* %p, i64 3, i64* %p, i64 4, i64* %p, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT: %q = alloca i64
; CHECK-NEXT: store i64 0, i64* %q
; CHECK-NEXT: br label %for.cond
    %p = alloca i64
    store i64 0, i64* %p
    store i64 1, i64* %p
    store i64 2, i64* %p
    store i64 3, i64* %p
    store i64 4, i64* %p
    %q = alloca i64
    store i64 0, i64* %q
    br label %for.cond

for.cond:
; CHECK: for.cond:
; CHECK-NEXT: %i = load i64, i64* %q
; CHECK-NEXT: %cmp = icmp slt i64 %i, 10
; CHECK-NEXT: br i1 %cmp, label %for.body, label %for.end
    %i = load i64, i64* %q
    %cmp = icmp slt i64 %i, 10
    br i1 %cmp, label %for.body, label %for.end

for.body:
; CHECK: for.body:
; CHECK-NEXT: call i64 @oracle(i64 2, i64 0, i64* %p, i64 1, i64* %p, i64 2, i64* %p, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT: %x = load i64, i64* %p
; CHECK-NEXT: call i64 @oracle(i64 1, i64 0, i64* %p, i64 1, i64* %p, i64 2, i64* %p, i64 3, i64* %p, i64 0, i64* null, i64 0, i64* null, i64 0, i64* null)
; CHECK-NEXT: br label %for.inc
    store i64 0, i64* %p
    store i64 1, i64* %p
    store i64 2, i64* %p
    %x = load i64, i64* %p
    store i64 0, i64* %p
    store i64 1, i64* %p
    store i64 2, i64* %p
    store i64 3, i64* %p
    br label %for.inc

for.inc:
; CHECK: for.inc:
; CHECK-NEXT: %inc = add i64 %i, 1
; CHECK-NEXT: store i64 %inc, i64* %q
; CHECK-NEXT: br label %for.cond
    %inc = add i64 %i, 1
    store i64 %inc, i64* %q
    br label %for.cond

for.end:
; CHECK: for.end:
; CHECK-NEXT: ret i64 0
    ret i64 0
}

; CHECK: @oracle(i64 [[ARG:%.*]], i64 [[v1:%.*]], i64* [[p1:%.*]], i64 [[v2:%.*]], i64* [[p2:%.*]], i64 [[v3:%.*]], i64* [[p3:%.*]], i64 [[v4:%.*]], i64* [[p4:%.*]], i64 [[v5:%.*]], i64* [[p5:%.*]], i64 [[v6:%.*]], i64* [[p6:%.*]], i64 [[v7:%.*]], i64* [[p7:%.*]]) {
; CHECK: entry:
; CHECK-NEXT:  switch i64 [[ARG]], label %end [
; CHECK-NEXT:    i64 1, label %L1
; CHECK-NEXT:    i64 2, label %L2
; CHECK-NEXT:    i64 3, label %L3
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
; CHECK: L3:
; CHECK-NEXT:  store i64 [[v1]], i64* [[p1]]
; CHECK-NEXT:  store i64 [[v2]], i64* [[p2]]
; CHECK-NEXT:  store i64 [[v3]], i64* [[p3]]
; CHECK-NEXT:  store i64 [[v4]], i64* [[p4]]
; CHECK-NEXT:  store i64 [[v5]], i64* [[p5]]
; CHECK-NEXT:  ret i64 0
; CHECK: end:
; CHECK-NEXT:  ret i64 0
