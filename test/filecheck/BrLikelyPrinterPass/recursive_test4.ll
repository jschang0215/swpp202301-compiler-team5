define i32 @main() {
; CHECK:       entry:
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %a, align 4
  call void @f(i32*  %a, i32 10, i32 0, i32 20)
  ret i32 0
}

; function that do pseudo binary search
; recursive call inside of true basic block 
define void @f(i32* %p, i32 %k, i32 %l, i32 %r) {
entry:
  %p.addr = alloca i32*, align 8
  %k.addr = alloca i32, align 4
  %l.addr = alloca i32, align 4
  %r.addr = alloca i32, align 4
  %m = alloca i32, align 4
  store i32* %p, i32** %p.addr, align 8
  store i32 %k, i32* %k.addr, align 4
  store i32 %l, i32* %l.addr, align 4
  store i32 %r, i32* %r.addr, align 4
  %0 = load i32*, i32** %p.addr, align 8
  %1 = load i32, i32* %0, align 4
  %add = add nsw i32 %1, 1
  store i32 %add, i32* %0, align 4
  %2 = load i32, i32* %l.addr, align 4
  %3 = load i32, i32* %r.addr, align 4
  %cmp = icmp eq i32 %2, %3
  br i1 %cmp, label %if.then, label %if.end

if.then:
  br label %if.end8

if.end:
  %4 = load i32, i32* %l.addr, align 4
  %5 = load i32, i32* %r.addr, align 4
  %add1 = add nsw i32 %4, %5
  %div = sdiv i32 %add1, 2
  store i32 %div, i32* %m, align 4
  %6 = load i32, i32* %k.addr, align 4
  %7 = load i32, i32* %m, align 4
  %cmp2 = icmp sle i32 %6, %7
  br i1 %cmp2, label %if.then3, label %if.end4

if.then3:
  %8 = load i32*, i32** %p.addr, align 8
  %9 = load i32, i32* %k.addr, align 4
  %10 = load i32, i32* %l.addr, align 4
  %11 = load i32, i32* %m, align 4
  call void @f(i32* noundef %8, i32 noundef %9, i32 noundef %10, i32 noundef %11)
  br label %if.end4

if.end4:
  %12 = load i32, i32* %m, align 4
  %13 = load i32, i32* %k.addr, align 4
  %cmp5 = icmp slt i32 %12, %13
  br i1 %cmp5, label %if.then6, label %if.end8

if.then6:                                         ; preds = %if.end4
  %14 = load i32*, i32** %p.addr, align 8
  %15 = load i32, i32* %k.addr, align 4
  %16 = load i32, i32* %m, align 4
  %add7 = add nsw i32 %16, 1
  %17 = load i32, i32* %r.addr, align 4
  call void @f(i32* noundef %14, i32 noundef %15, i32 noundef %add7, i32 noundef %17)
  br label %if.end8

if.end8:                                          ; preds = %if.then, %if.then6, %if.end4
  ret void
}