; Test constant add/sub to incr/decr
; Tests conversion of add, sub to incr, decr of type i1, i8, i16, i32, i64
; @f tests incr, decr of i64
; @g tests incr, decr of i32
; @h tests incr, decr of i16
; @i tests incr, decr of i8
; @k tests incr, decr of i1

define void @f(i64 %x) {
; CHECK-LABEL: @f(i64 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i64 @incr_i64(i64 %x)
; CHECK-NEXT:     %1 = call i64 @incr_i64(i64 %0)
; CHECK-NEXT:     %2 = call i64 @decr_i64(i64 %x)
; CHECK-NEXT:     ret void
entry:
  %incr1 = add i64 %x, 2
  %decr1 = sub i64 %x, 1
  ret void
}

define void @g(i32 %x) {
; CHECK-LABEL: @g(i32 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i32 @incr_i32(i32 %x)
; CHECK-NEXT:     %1 = call i32 @incr_i32(i32 %0)
; CHECK-NEXT:     %2 = call i32 @decr_i32(i32 %x)
; CHECK-NEXT:     %3 = call i32 @decr_i32(i32 %2)
; CHECK-NEXT:     %4 = call i32 @decr_i32(i32 %3)
; CHECK-NEXT:     ret void
entry:
  %incr2 = add i32 %x, 2
  %decr2 = sub i32 %x, 3
  ret void
}

define void @h(i16 %x) {
; CHECK-LABEL: @h(i16 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i16 @incr_i16(i16 %x)
; CHECK-NEXT:     %1 = call i16 @decr_i16(i16 %x)
; CHECK-NEXT:     %2 = call i16 @decr_i16(i16 %1)
; CHECK-NEXT:     %3 = call i16 @decr_i16(i16 %2)
; CHECK-NEXT:     ret void
entry:
  %incr3 = add i16 %x, 1
  %decr3 = sub i16 %x, 3
  ret void
}

define void @i(i8 %x) {
; CHECK-LABEL: @i(i8 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i8 @incr_i8(i8 %x)
; CHECK-NEXT:     %1 = call i8 @incr_i8(i8 %0)
; CHECK-NEXT:     %2 = call i8 @incr_i8(i8 %1)
; CHECK-NEXT:     %3 = call i8 @decr_i8(i8 %x)
; CHECK-NEXT:     ret void
entry:
  %incr4 = add i8 %x, 3
  %decr4 = sub i8 %x, 1
  ret void
}

define void @k(i1 %x) {
; CHECK-LABEL: @k(i1 %x)
; CHECK:       entry:
; CHECK-NEXT:     %0 = call i1 @incr_i1(i1 %x)
; CHECK-NEXT:     ret void
entry:
  %incr5 = add i1 %x, 2
  %decr5 = sub i1 %x, 1
  ret void
}

define i32 @main() {
; CHECK-LABEL: @main()
; CHECK:       entry:
; CHECK-NEXT:     call void @f(i64 4)
; CHECK-NEXT:     call void @g(i32 3)
; CHECK-NEXT:     call void @h(i16 2)
; CHECK-NEXT:     call void @i(i8 1)
; CHECK-NEXT:     call void @k(i1 false)
; CHECK-NEXT:     ret i32 0
entry:
  call void @f(i64 4)
  call void @g(i32 3)
  call void @h(i16 2)
  call void @i(i8 1)
  call void @k(i1 false)
  ret i32 0
}

declare i64 @incr_i64(i64)
declare i64 @decr_i64(i64)
declare i32 @incr_i32(i32)
declare i32 @decr_i32(i32)
declare i16 @incr_i16(i16)
declare i16 @decr_i16(i16)
declare i8 @incr_i8(i8)
declare i8 @decr_i8(i8)
declare i1 @incr_i1(i1)
declare i1 @decr_i1(i1)