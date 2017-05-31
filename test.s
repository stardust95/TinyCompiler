; ModuleID = 'main'
source_filename = "main"

define internal i64 @do_math(i64 %a) {
entry:
  %0 = alloca i64
  store i64 %a, i64* %0
  %1 = alloca i64
  %2 = load i64, i64* %0
  %multmp = mul i64 %2, 5
  store i64 %multmp, i64* %1
  %3 = load i64, i64* %1
  ret i64 %3
}

define internal i64 @main() {
entry:
  %0 = alloca i64
  store i64 2, i64* %0
  %1 = alloca double
  store double 1.000000e+00, double* %1
  %2 = load i64, i64* %0
  %3 = load double, double* %1
  %ftmp = uitofp i64 %2 to double
  %cmpftmp = fcmp ult double %ftmp, %3
  %4 = icmp ne i1 %cmpftmp, false
  br i1 %4, label %then, label %else

then:                                             ; preds = %entry
  %5 = alloca i64
  store i64 3, i64* %5
  %6 = load i64, i64* %5
  %7 = uitofp i64 %6 to double
  store double %7, double* %1
  br label %ifcont

else:                                             ; preds = %entry
  %8 = load i64, i64* %0
  %cmptmp = icmp ult i64 %8, 2
  %9 = sext i1 %cmptmp to i64
  store i64 %9, i64* %0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %10 = load i64, i64* %0
  %cmptmp1 = icmp ult i64 %10, 1
  %11 = icmp ne i1 %cmptmp1, false
  store i64 0, i64* %0
  br i1 %11, label %forloop, label %forcont

forloop:                                          ; preds = %forloop, %ifcont
  store double 3.000000e+00, double* %1
  %12 = load i64, i64* %0
  %addtmp = add i64 %12, 1
  store i64 %addtmp, i64* %0
  %13 = load i64, i64* %0
  %cmptmp2 = icmp ult i64 %13, 1
  %14 = icmp ne i1 %cmptmp2, false
  br i1 %14, label %forloop, label %forcont

forcont:                                          ; preds = %forloop, %ifcont
  %15 = load i64, i64* %0
  %cmptmp3 = icmp ult i64 %15, 2
  %16 = icmp ne i1 %cmptmp3, false
  br i1 %16, label %forloop4, label %forcont6

forloop4:                                         ; preds = %forloop4, %forcont
  store double 2.000000e+00, double* %1
  %17 = load i64, i64* %0
  %cmptmp5 = icmp ult i64 %17, 2
  %18 = icmp ne i1 %cmptmp5, false
  br i1 %18, label %forloop4, label %forcont6

forcont6:                                         ; preds = %forloop4, %forcont
  %19 = alloca i64
  %calltmp = call i64 @do_math(i64 1)
  store i64 %calltmp, i64* %19
  %20 = load i64, i64* %19
  ret i64 %20
}
