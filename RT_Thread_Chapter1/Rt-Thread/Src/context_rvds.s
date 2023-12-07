;*************************************************************************
;                全局变量，从cpuport.c中导入
;*************************************************************************
;用于存储上一个线程的sp的指针
	IMPORT rt_interrupt_from_thread
;用于存储下一个将要运行的线程的sp指针
	IMPORT rt_interrupt_to_thread
;PendSV中断服务函数执行的标志
	IMPORT rt_thread_switch_interrupt_flag
		
		
;*************************************************************************
;                 常量，有关内核外设寄存器
;				  系统控制块外设SCB地址范围0XE000ED00~0XE000ED3F
;*************************************************************************
SCB_VTOR			EQU			0XE000ED08		;向量表偏移寄存器
NVIC_INT_CTRL		EQU			0XE000ED04		;中断控制状态寄存器
NVIC_SYSPRI2		EQU			0XE000ED20		;系统优先级寄存器
NVIC_PENDSV_PRI		EQU			0X00FF0000		;PendSV 优先级值（lowest）
NVIC_PENDSVSET		EQU 		0X10000000		;触发PendSV exception的值
	

;*************************************************************************
;                		代码产生指令
;*************************************************************************

	AREA |.text|, CODE, READONLY, ALIGN=2
	THUMB
	REQUIRE8
	PRESERVE8

;*************************************************************************
;          	函数原型：void rt_hw_context_switch_to(rt_uint32_t to)
;			r0 --> to
;			该函数用于开启第一次线程切换
;*************************************************************************

rt_hw_context_switch_to		PROC
	
	;导出rt_hw_context_switch_to，使其具有全局属性，可以在C文件中调用，但是必须在rthw.h文件中声明
	EXPORT rt_hw_context_switch_to
	
	;将rt_interrupt_to_thread的地址加载到r1，注意这边的等于，LDR Rd，=Label，这里的LDR[伪指令]，如果label是个标识符（比如指针），则会传入指针的地址
	;那么此时rt_interrupt_to_thread指向这个sp，然后sp再指向内存中的值
	LDR		r1, =rt_interrupt_to_thread
	;将r0的值存储到rt_interrupt_to_thread，其中r0存储的值是下一个要运行的线程的sp的地址，
	;由该函数rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp)调用时传入到r0
	STR		r0,[r1]
	
	;设置rt_interrupt_from_thread的值为0，表示启动第一次线程切换
	;将rt_interrupt_from_thread的地址加载到r1
	LDR		r1, =rt_interrupt_from_thread
	;配置r0等于0
	MOV 	r0,#0x0
	;将r0的值存储到rt_interrupt_from_thread
	STR 	r0,[r1]
	
	;设置中断标志位rt_thread_switch_interrupt_flag的值为1
	;将rt_thread_switch_interrupt_flag的地址加载到r1
	LDR		r1, =rt_thread_switch_interrupt_flag
	;配置r0等于1
	MOV 	r0, #1
	;将r0的值存储到rt_thread_switch_interrupt_flag中
	STR		r0,[r1]
	
	;设置 PendSV 异常的优先级
	LDR		r0, =NVIC_SYSPRI2		;将嵌套中断向量控制器的控制优先级的寄存器地址加载到r0
	LDR		r1, =NVIC_PENDSV_PRI	;将优先级的值加载到r1
	LDR.W	r2, [r0,#0x00]			;该命令读取NVIC_SYSPRI2的值到r2（.W是LDR的字扩展，保证加载的是一个Word字。）
	ORR		r1,r1,r2				;对r1和r2进行“或”运行算，然后将结果放到r1中
	STR		r1,	[r0]				;将r1的值（修改完毕的优先级的值）赋给NVIC_SYSPRI2
	
	;触发 PendSV 异常（产生上下文切换）
	LDR		r0, =NVIC_INT_CTRL
	LDR		r1, =NVIC_PENDSVSET		;这是一个特殊的值，用于触发 PendSV 异常
	STR		r1, [r0]
	
	;开中断。CPSIE的全拼是“Change Processor State, Interrupt Enable”，后面的参数F代表FIQ（快速中断响应），参数I代表IRQ（普通中断响应）。
	CPSIE	F
	CPSIE	I
	
	;永远无法到达的位置,与PROC成对出现,End Procedure
	ENDP
	

;/*
; *-----------------------------------------------------------------------
; * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
; * r0 --> from
; * r1 --> to
; *-----------------------------------------------------------------------
; */
rt_hw_context_switch	PROC
	EXPORT rt_hw_context_switch
	
	;设置中断标志位rt_thread_switch_interrupt_flag为1
	;加载rt_thread_switch_interrupt_flag的地址到r2
	LDR		r2, =rt_thread_switch_interrupt_flag
	;加载rt_thread_switch_interrupt_flag的值到r2
	LDR		r3, [r2]
	;r3与1比较，相等则执行BEQ命令
	CMP		r3, #1
	BEQ		_reswitch
	;设置r3的值为1
	MOV 	r3, #1
	;将rt_thread_switch_interrupt_flag的值置1
	STR		r3, [r2]
	
	;设置rt_interrupt_from_thread的值
    LDR     r2, =rt_interrupt_from_thread                 ; 加载rt_interrupt_from_thread的地址到r2
    STR     r0, [r2]                                      ; 存储r0的值到rt_interrupt_from_thread，即上一个线程栈指针sp的指针
	
_reswitch
	;设置rt_interrupt_to_thread的值
	LDR     r2, =rt_interrupt_to_thread                   ; 加载rt_interrupt_from_thread的地址到r2
    STR     r1, [r2]                                      ; 存储r1的值到rt_interrupt_from_thread，即下一个线程栈指针sp的指针
	
	;触发PendSV异常，实现上下文切换
	LDR     r0, =NVIC_INT_CTRL              
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]
	
    ; 子程序返回
	BX      LR
	
	; 子程序结束
    ENDP

	
;/*
; *-----------------------------------------------------------------------
; * void PendSV_Handler(void);
; * r0 --> switch from thread stack
; * r1 --> switch to thread stack
; * psr, pc, lr, r12, r3, r2, r1, r0 are pushed into [from] stack
; *-----------------------------------------------------------------------
; */

PendSV_Handler			PROC
	EXPORT PendSV_Handler
	
	;禁用中断，为了保护上下文切换不被中断
	MRS		r2, PRIMASK				;MRS:“Move to Register from Special register”。将PRIMASK的值（当前中断状态）移动到r2寄存器中。
	CPSID	I						;关闭IRQ中断
	
	;获取中断标志位，看看是否为0
	;加载rt_thread_switch_interrupt_flag的地址到r0
	LDR		r0, =rt_thread_switch_interrupt_flag
	;加载rt_thread_switch_interrupt_flag的值到r1
	LDR		r1, [r0]
	;判断r1的值是否为0，为0则跳转到pendsv_exit
	CBZ		r1, pendsv_exit			; CBZ:"Compare and Branch on Zero"。与0比较，如果是0则跳转
	
	;r1即rt_thread_switch_interrupt_flag的标志位不为0
	MOV		r1, #0x00
	;将rt_thread_switch_interrupt_flag清零
	STR		r1,[r0]
	
	;判断rt_interrupt_from_thread的值是否为0
	;加载rt_interrupt_from_thread的地址到r0
	LDR 	r0, =rt_interrupt_from_thread
	;加载rt_interrupt_from_thread的值到r1
	LDR		r1,[r0]
	;判断r1的值是否为0，如果是0就代表没有这是第一次线程切换，并没有上文需要保存，直接切换到下文即可，跳转到switch_to_thread
	CBZ		r1, switch_to_thread
	
; ========================== 上文保存 ==============================
	;当进入PendSV_Handler的时候，上一个线程中的环境变量们
	;xPSR,PC(线程入口地址),r14,r12,r3,r2,r1,r0（线程的形参）都会自动保存到线程的栈中，剩下的r4~r11需要手动保存
	;获取线程的栈指针到r1
	MRS		r1, psp					;psp:“Process Stack Pointer”存储当前线程的栈指针
	;将r4~r11保存到r1指向的地址（每操作一次，地址将递减一次）
	STMFD	r1!,{r4- r11}			;STMFD: "Store Multiple Full Descending"每次存储完成后，地址都会减少32bit（1个字）
	;加载r0的地址指向的值到r0，即r0=rt_interrupt_from_thread。
	LDR		r0, [r0]
	;将r1的值存储到[r0]，即更新线程栈sp，这两句需要特别理解一下
	;将r1的值存储到[r0]，即r1的值存储到[rt_interrupt_from_thread]，为之后的线程操作或者恢复做准备
	STR		r1,[r0]

; ========================== 下文切换 ==============================
switch_to_thread
	;加载rt_interrupt_to_thread的值到r1
	;rt_interrupt_to_thread是一个全局变量，里面保存的是线程栈指针sp的地址，下面的指令是加载的sp的地址的地址
	LDR		r1, =rt_interrupt_to_thread
	;加载rt_interrupt_to_thread的值到r1，下面指令是加载的sp的地址
	LDR		r1, [r1]
	;下面指令是加载的sp
	LDR		r1, [r1]
	
	;将线程栈指针r1（操作前先递减）指向的内容加载到cpu寄存器r4~r11
	LDMFD	r1!,{r4- r11}
	;将线程栈指针更新到psp
	MSR		psp, r1
	
pendsv_exit
	;恢复中断
	msr		PRIMASK, r2
	
	;确保异常返回使用的栈指针是psp，即lr寄存器的位2要为1
	;通过将lr寄存器的第2位设置为1（即添加0x04），指令确保在从异常返回时，处理器使用进程栈指针（psp）而不是主栈指针（msp）。
	;ARM处理器有两种栈指针，psp（进程栈指针）和msp（主栈指针），它们分别用于不同的目的。
	;psp通常用于普通的应用程序线程，而msp用于异常处理和系统级别的功能。
	;lr（链接寄存器）通常用于存储函数返回地址。但在中断或异常处理时，lr的用途会略有不同。
	;当发生异常时，lr会存储一些额外的信息，帮助处理器决定如何从异常返回。
	ORR		lr, lr, #0x04
	;异常返回，这个时候栈中的剩下内容（xPSR，PC, r14，r12，r3，r2，r1，r0）会自动加载到CPU寄存器
	;同时，psp的值也会更新，指向线程栈的栈顶
	BX		lr
	
	;子程序Pendsv_Handler结束
	ENDP
	




	ALIGN 4	
		
	END
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		