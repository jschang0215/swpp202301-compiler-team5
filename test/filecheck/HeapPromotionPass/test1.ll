; Test where all mallocs can be promoted to stack
; Output: call, call1, call2 should be found

define i32 @main() #0 {
; CHECK:      entry:
; CHECK-NEXT:    %call2_stack = alloca i8, i64 8, align 1
; CHECK-NEXT:    %call1_stack = alloca i8, i64 8, align 1
; CHECK-NEXT:    %call_stack = alloca i8, i64 8, align 1
; CHECK-NEXT:    %retval = alloca i32, align 4
; CHECK-NEXT:    %arr1 = alloca i8*, align 8
; CHECK-NEXT:    %arr2 = alloca i8*, align 8
; CHECK-NEXT:    %arr3 = alloca i8*, align 8
; CHECK-NEXT:    store i32 0, i32* %retval, align 4
; CHECK-NEXT:    store i8* %call_stack, i8** %arr1, align 8
; CHECK-NEXT:    store i8* %call1_stack, i8** %arr2, align 8
; CHECK-NEXT:    store i8* %call2_stack, i8** %arr3, align 8
; CHECK-NEXT:    %0 = load i8*, i8** %arr1, align 8
; CHECK-NEXT:    %1 = load i8*, i8** %arr2, align 8
; CHECK-NEXT:    %2 = load i8*, i8** %arr3, align 8
; CHECK-NEXT:    ret i32 0
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

  %call2 = call noalias i8* @malloc(i64 noundef 8) #4
  store i8* %call2, i8** %arr3, align 8

  %0 = load i8*, i8** %arr1, align 8
  call void @free(i8* noundef %0) #5

  %1 = load i8*, i8** %arr2, align 8
  call void @free(i8* noundef %1) #5

  %2 = load i8*, i8** %arr3, align 8
  call void @free(i8* noundef %2) #5

  ret i32 0
}

declare noalias i8* @malloc(i64 noundef)
declare void @free(i8* noundef) #3