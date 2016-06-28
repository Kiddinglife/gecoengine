//
// FILE: test.asm
// Description: 汇编源文件。当前文件仅在x64平台上编译，并不包含于x86平台，可通过设置源文件的属性实现。
//
.CODE // 文件开始
// 函数原型： void Int_3()
// 函数描述：函数实现中断指令，没有输入/输出参数，也没有返回值
Int_3 PROC 
int 3 // 中断指令
ret   // 函数返回指令
Int_3 ENDP 
END  // 文件结束