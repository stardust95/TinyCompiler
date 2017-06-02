; ModuleID = 'testmain.cpp'
source_filename = "testmain.cpp"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

; Function Attrs: nounwind ssp uwtable
define i32 @_Z4funcPi(i32*) #0 {
  %2 = alloca i32*, align 8
  %3 = load i32*, i32** %2, align 8
  %4 = getelementptr inbounds i32, i32* %3, i64 2
  %5 = load i32, i32* %4, align 4
  ret i32 %5
}

; Function Attrs: norecurse nounwind ssp uwtable
define i32 @main() #1 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca [3 x i32], align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 1, i32* %2, align 4
  store i32 2, i32* %3, align 4
  %6 = getelementptr inbounds [3 x i32], [3 x i32]* %4, i64 0, i64 0
  %7 = load i32, i32* %2, align 4
  store i32 %7, i32* %6, align 4
  %8 = getelementptr inbounds i32, i32* %6, i64 1
  %9 = load i32, i32* %3, align 4
  store i32 %9, i32* %8, align 4
  %10 = getelementptr inbounds i32, i32* %8, i64 1
  %11 = getelementptr inbounds i32, i32* %6, i64 3
  br label %12

; <label>:12:                                     ; preds = %12, %0
  %13 = phi i32* [ %10, %0 ], [ %14, %12 ]
  store i32 0, i32* %13, align 4
  %14 = getelementptr inbounds i32, i32* %13, i64 1
  %15 = icmp eq i32* %14, %11
  br i1 %15, label %16, label %12

; <label>:16:                                     ; preds = %12
  %17 = getelementptr inbounds [3 x i32], [3 x i32]* %4, i64 0, i64 2
  %18 = load i32, i32* %17, align 4
  store i32 %18, i32* %5, align 4
  %19 = getelementptr inbounds [3 x i32], [3 x i32]* %4, i32 0, i32 0
  %20 = call i32 @_Z4funcPi(i32* %19)
  %21 = load i32, i32* %1, align 4
  ret i32 %21
}

attributes #0 = { nounwind ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { norecurse nounwind ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 3.9.1 (tags/RELEASE_391/final)"}
