; Test various cases

define void @f() {
  ret void
}

define i32 @main() #0 {
; CHECK:      entry:
; CHECK-NEXT:   %retval = alloca i32, align 4
; CHECK-NEXT:   %arr1 = alloca i8*, align 8
; CHECK-NEXT:   %arr2 = alloca i8*, align 8
; CHECK-NEXT:   %arr3 = alloca i8*, align 8
; CHECK-NEXT:   store i32 0, i32* %retval, align 4
; CHECK-NEXT:   call void @f()
; CHECK-NEXT:   %call1 = call noalias i8* @malloc(i64 noundef 8)
; CHECK-NEXT:   store i8* %call1, i8** %arr2, align 8
; CHECK-NEXT:   %0 = load i8*, i8** %arr2, align 8
; CHECK-NEXT:   call void @free(i8* noundef %0)
; CHECK-NEXT:   %call = call noalias i8* @malloc(i64 noundef 8)
; CHECK-NEXT:   store i8* %call, i8** %arr1, align 8
; CHECK-NEXT:   %1 = load i8*, i8** %arr1, align 8
; CHECK-NEXT:   call void @free(i8* noundef %1)
; CHECK-NEXT:   %2 = load i8*, i8** %arr1, align 8
; CHECK-NEXT:   %tobool = icmp ne i8* %2, null
; CHECK-NEXT:   br i1 %tobool, label %if.then, label %if.end
; CHECK:      if.then:                                          ; preds = %entry
; CHECK-NEXT:   call void @f()
; CHECK-NEXT:   %call2 = call noalias i8* @malloc(i64 noundef 8)
; CHECK-NEXT:   store i8* %call2, i8** %arr3, align 8
; CHECK-NEXT:   %3 = load i8*, i8** %arr3, align 8
; CHECK-NEXT:   call void @free(i8* noundef %3)
; CHECK-NEXT:   br label %if.end
; CHECK:      if.end:                                           ; preds = %if.then, %entry
; CHECK-NEXT:   ret i32 0
entry:
  %retval = alloca i32, align 4
  %arr1 = alloca i8*, align 8
  %arr2 = alloca i8*, align 8
  %arr3 = alloca i8*, align 8
  store i32 0, i32* %retval, align 4
  %call = call noalias i8* @malloc(i64 noundef 8) #4
  store i8* %call, i8** %arr1, align 8
  %call1 = call noalias i8* @malloc(i64 noundef 8) #4
  store i8* %call1, i8** %arr2, align 8
  call void @f()
  %0 = load i8*, i8** %arr2, align 8
  call void @free(i8* noundef %0) #5
  %1 = load i8*, i8** %arr1, align 8
  call void @free(i8* noundef %1) #5
  %2 = load i8*, i8** %arr1, align 8
  %tobool = icmp ne i8* %2, null
  br i1 %tobool, label %if.then, label %if.end

if.then:                                         
  %call2 = call noalias i8* @malloc(i64 noundef 8) #4
  store i8* %call2, i8** %arr3, align 8
  call void @f()
  %3 = load i8*, i8** %arr3, align 8
  call void @free(i8* noundef %3) #5
  br label %if.end

if.end:                                           
  ret i32 0                 
}

declare noalias i8* @malloc(i64 noundef)
declare void @free(i8* noundef)