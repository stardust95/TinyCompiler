; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

; Function Attrs: nounwind ssp uwtable
define i32 @do_math(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %4 = load i32, i32* %2, align 4
  %5 = mul nsw i32 %4, 5
  store i32 %5, i32* %3, align 4
  %6 = load i32, i32* %3, align 4
  ret i32 %6
}

; Function Attrs: nounwind ssp uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca double, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 2, i32* %2, align 4
  store double 1.000000e+00, double* %3, align 8
  %6 = load i32, i32* %2, align 4
  %7 = sitofp i32 %6 to double
  %8 = load double, double* %3, align 8
  %9 = fcmp olt double %7, %8
  br i1 %9, label %10, label %13

; <label>:10:                                     ; preds = %0
  store i32 3, i32* %4, align 4
  %11 = load i32, i32* %4, align 4
  %12 = sitofp i32 %11 to double
  store double %12, double* %3, align 8
  br label %17

; <label>:13:                                     ; preds = %0
  %14 = load i32, i32* %2, align 4
  %15 = icmp slt i32 %14, 2
  %16 = zext i1 %15 to i32
  store i32 %16, i32* %2, align 4
  br label %17

; <label>:17:                                     ; preds = %13, %10
  store i32 0, i32* %2, align 4
  br label %18

; <label>:18:                                     ; preds = %22, %17
  %19 = load i32, i32* %2, align 4
  %20 = icmp slt i32 %19, 1
  br i1 %20, label %21, label %25

; <label>:21:                                     ; preds = %18
  store double 3.000000e+00, double* %3, align 8
  br label %22

; <label>:22:                                     ; preds = %21
  %23 = load i32, i32* %2, align 4
  %24 = add nsw i32 %23, 1
  store i32 %24, i32* %2, align 4
  br label %18

; <label>:25:                                     ; preds = %18
  br label %26

; <label>:26:                                     ; preds = %29, %25
  %27 = load i32, i32* %2, align 4
  %28 = icmp slt i32 %27, 2
  br i1 %28, label %29, label %30

; <label>:29:                                     ; preds = %26
  store double 2.000000e+00, double* %3, align 8
  br label %26

; <label>:30:                                     ; preds = %26
  %31 = call i32 @do_math(i32 1)
  store i32 %31, i32* %5, align 4
  %32 = load i32, i32* %5, align 4
  ret i32 %32
}

attributes #0 = { nounwind ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 3.9.1 (tags/RELEASE_391/final)"}
