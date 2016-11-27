;Description: 汇编源文件。当前文件仅在x64平台上编译，并不包含于x86平台，可通过设置源文件的属性实现。
.CODE // 文件开始

;函数原型: void Int_3()
;函数描述: 函数实现中断指令，没有输入/输出参数，也没有返回值
Int_3 PROC 
	int 3 ;中断指令
Int_3 ENDP 

;函数原型： uint64 asm_time()
;MSVC complains about no return value here.
;According to the help, this warning is 'harmless', and
;they even have example code which does it. Go figure.
asm_time PROC 
	rdtsc ;指令
	;ret
asm_time ENDP 

END  // 文件结束
