; ModuleID = 'testmain.cpp'
source_filename = "testmain.cpp"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

; Function Attrs: norecurse nounwind ssp uwtable
define i32 @main() #0 {
  %1 = alloca [9 x i32], align 16
  %2 = getelementptr inbounds [9 x i32], [9 x i32]* %1, i64 0, i64 8
  %3 = load i32, i32* %2, align 16
  %4 = getelementptr inbounds [9 x i32], [9 x i32]* %1, i64 0, i64 7
  store i32 %3, i32* %4, align 4
  ret i32 0
}

attributes #0 = { norecurse nounwind ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 3.9.1 (tags/RELEASE_391/final)"}
