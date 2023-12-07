;*************************************************************************
;                ȫ�ֱ�������cpuport.c�е���
;*************************************************************************
;���ڴ洢��һ���̵߳�sp��ָ��
	IMPORT rt_interrupt_from_thread
;���ڴ洢��һ����Ҫ���е��̵߳�spָ��
	IMPORT rt_interrupt_to_thread
;PendSV�жϷ�����ִ�еı�־
	IMPORT rt_thread_switch_interrupt_flag
		
		
;*************************************************************************
;                 �������й��ں�����Ĵ���
;				  ϵͳ���ƿ�����SCB��ַ��Χ0XE000ED00~0XE000ED3F
;*************************************************************************
SCB_VTOR			EQU			0XE000ED08		;������ƫ�ƼĴ���
NVIC_INT_CTRL		EQU			0XE000ED04		;�жϿ���״̬�Ĵ���
NVIC_SYSPRI2		EQU			0XE000ED20		;ϵͳ���ȼ��Ĵ���
NVIC_PENDSV_PRI		EQU			0X00FF0000		;PendSV ���ȼ�ֵ��lowest��
NVIC_PENDSVSET		EQU 		0X10000000		;����PendSV exception��ֵ
	

;*************************************************************************
;                		�������ָ��
;*************************************************************************

	AREA |.text|, CODE, READONLY, ALIGN=2
	THUMB
	REQUIRE8
	PRESERVE8

;*************************************************************************
;          	����ԭ�ͣ�void rt_hw_context_switch_to(rt_uint32_t to)
;			r0 --> to
;			�ú������ڿ�����һ���߳��л�
;*************************************************************************

rt_hw_context_switch_to		PROC
	
	;����rt_hw_context_switch_to��ʹ�����ȫ�����ԣ�������C�ļ��е��ã����Ǳ�����rthw.h�ļ�������
	EXPORT rt_hw_context_switch_to
	
	;��rt_interrupt_to_thread�ĵ�ַ���ص�r1��ע����ߵĵ��ڣ�LDR Rd��=Label�������LDR[αָ��]�����label�Ǹ���ʶ��������ָ�룩����ᴫ��ָ��ĵ�ַ
	;��ô��ʱrt_interrupt_to_threadָ�����sp��Ȼ��sp��ָ���ڴ��е�ֵ
	LDR		r1, =rt_interrupt_to_thread
	;��r0��ֵ�洢��rt_interrupt_to_thread������r0�洢��ֵ����һ��Ҫ���е��̵߳�sp�ĵ�ַ��
	;�ɸú���rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp)����ʱ���뵽r0
	STR		r0,[r1]
	
	;����rt_interrupt_from_thread��ֵΪ0����ʾ������һ���߳��л�
	;��rt_interrupt_from_thread�ĵ�ַ���ص�r1
	LDR		r1, =rt_interrupt_from_thread
	;����r0����0
	MOV 	r0,#0x0
	;��r0��ֵ�洢��rt_interrupt_from_thread
	STR 	r0,[r1]
	
	;�����жϱ�־λrt_thread_switch_interrupt_flag��ֵΪ1
	;��rt_thread_switch_interrupt_flag�ĵ�ַ���ص�r1
	LDR		r1, =rt_thread_switch_interrupt_flag
	;����r0����1
	MOV 	r0, #1
	;��r0��ֵ�洢��rt_thread_switch_interrupt_flag��
	STR		r0,[r1]
	
	;���� PendSV �쳣�����ȼ�
	LDR		r0, =NVIC_SYSPRI2		;��Ƕ���ж������������Ŀ������ȼ��ļĴ�����ַ���ص�r0
	LDR		r1, =NVIC_PENDSV_PRI	;�����ȼ���ֵ���ص�r1
	LDR.W	r2, [r0,#0x00]			;�������ȡNVIC_SYSPRI2��ֵ��r2��.W��LDR������չ����֤���ص���һ��Word�֡���
	ORR		r1,r1,r2				;��r1��r2���С��������㣬Ȼ�󽫽���ŵ�r1��
	STR		r1,	[r0]				;��r1��ֵ���޸���ϵ����ȼ���ֵ������NVIC_SYSPRI2
	
	;���� PendSV �쳣�������������л���
	LDR		r0, =NVIC_INT_CTRL
	LDR		r1, =NVIC_PENDSVSET		;����һ�������ֵ�����ڴ��� PendSV �쳣
	STR		r1, [r0]
	
	;���жϡ�CPSIE��ȫƴ�ǡ�Change Processor State, Interrupt Enable��������Ĳ���F����FIQ�������ж���Ӧ��������I����IRQ����ͨ�ж���Ӧ����
	CPSIE	F
	CPSIE	I
	
	;��Զ�޷������λ��,��PROC�ɶԳ���,End Procedure
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
	
	;�����жϱ�־λrt_thread_switch_interrupt_flagΪ1
	;����rt_thread_switch_interrupt_flag�ĵ�ַ��r2
	LDR		r2, =rt_thread_switch_interrupt_flag
	;����rt_thread_switch_interrupt_flag��ֵ��r2
	LDR		r3, [r2]
	;r3��1�Ƚϣ������ִ��BEQ����
	CMP		r3, #1
	BEQ		_reswitch
	;����r3��ֵΪ1
	MOV 	r3, #1
	;��rt_thread_switch_interrupt_flag��ֵ��1
	STR		r3, [r2]
	
	;����rt_interrupt_from_thread��ֵ
    LDR     r2, =rt_interrupt_from_thread                 ; ����rt_interrupt_from_thread�ĵ�ַ��r2
    STR     r0, [r2]                                      ; �洢r0��ֵ��rt_interrupt_from_thread������һ���߳�ջָ��sp��ָ��
	
_reswitch
	;����rt_interrupt_to_thread��ֵ
	LDR     r2, =rt_interrupt_to_thread                   ; ����rt_interrupt_from_thread�ĵ�ַ��r2
    STR     r1, [r2]                                      ; �洢r1��ֵ��rt_interrupt_from_thread������һ���߳�ջָ��sp��ָ��
	
	;����PendSV�쳣��ʵ���������л�
	LDR     r0, =NVIC_INT_CTRL              
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]
	
    ; �ӳ��򷵻�
	BX      LR
	
	; �ӳ������
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
	
	;�����жϣ�Ϊ�˱����������л������ж�
	MRS		r2, PRIMASK				;MRS:��Move to Register from Special register������PRIMASK��ֵ����ǰ�ж�״̬���ƶ���r2�Ĵ����С�
	CPSID	I						;�ر�IRQ�ж�
	
	;��ȡ�жϱ�־λ�������Ƿ�Ϊ0
	;����rt_thread_switch_interrupt_flag�ĵ�ַ��r0
	LDR		r0, =rt_thread_switch_interrupt_flag
	;����rt_thread_switch_interrupt_flag��ֵ��r1
	LDR		r1, [r0]
	;�ж�r1��ֵ�Ƿ�Ϊ0��Ϊ0����ת��pendsv_exit
	CBZ		r1, pendsv_exit			; CBZ:"Compare and Branch on Zero"����0�Ƚϣ������0����ת
	
	;r1��rt_thread_switch_interrupt_flag�ı�־λ��Ϊ0
	MOV		r1, #0x00
	;��rt_thread_switch_interrupt_flag����
	STR		r1,[r0]
	
	;�ж�rt_interrupt_from_thread��ֵ�Ƿ�Ϊ0
	;����rt_interrupt_from_thread�ĵ�ַ��r0
	LDR 	r0, =rt_interrupt_from_thread
	;����rt_interrupt_from_thread��ֵ��r1
	LDR		r1,[r0]
	;�ж�r1��ֵ�Ƿ�Ϊ0�������0�ʹ���û�����ǵ�һ���߳��л�����û��������Ҫ���棬ֱ���л������ļ��ɣ���ת��switch_to_thread
	CBZ		r1, switch_to_thread
	
; ========================== ���ı��� ==============================
	;������PendSV_Handler��ʱ����һ���߳��еĻ���������
	;xPSR,PC(�߳���ڵ�ַ),r14,r12,r3,r2,r1,r0���̵߳��βΣ������Զ����浽�̵߳�ջ�У�ʣ�µ�r4~r11��Ҫ�ֶ�����
	;��ȡ�̵߳�ջָ�뵽r1
	MRS		r1, psp					;psp:��Process Stack Pointer���洢��ǰ�̵߳�ջָ��
	;��r4~r11���浽r1ָ��ĵ�ַ��ÿ����һ�Σ���ַ���ݼ�һ�Σ�
	STMFD	r1!,{r4- r11}			;STMFD: "Store Multiple Full Descending"ÿ�δ洢��ɺ󣬵�ַ�������32bit��1���֣�
	;����r0�ĵ�ַָ���ֵ��r0����r0=rt_interrupt_from_thread��
	LDR		r0, [r0]
	;��r1��ֵ�洢��[r0]���������߳�ջsp����������Ҫ�ر����һ��
	;��r1��ֵ�洢��[r0]����r1��ֵ�洢��[rt_interrupt_from_thread]��Ϊ֮����̲߳������߻ָ���׼��
	STR		r1,[r0]

; ========================== �����л� ==============================
switch_to_thread
	;����rt_interrupt_to_thread��ֵ��r1
	;rt_interrupt_to_thread��һ��ȫ�ֱ��������汣������߳�ջָ��sp�ĵ�ַ�������ָ���Ǽ��ص�sp�ĵ�ַ�ĵ�ַ
	LDR		r1, =rt_interrupt_to_thread
	;����rt_interrupt_to_thread��ֵ��r1������ָ���Ǽ��ص�sp�ĵ�ַ
	LDR		r1, [r1]
	;����ָ���Ǽ��ص�sp
	LDR		r1, [r1]
	
	;���߳�ջָ��r1������ǰ�ȵݼ���ָ������ݼ��ص�cpu�Ĵ���r4~r11
	LDMFD	r1!,{r4- r11}
	;���߳�ջָ����µ�psp
	MSR		psp, r1
	
pendsv_exit
	;�ָ��ж�
	msr		PRIMASK, r2
	
	;ȷ���쳣����ʹ�õ�ջָ����psp����lr�Ĵ�����λ2ҪΪ1
	;ͨ����lr�Ĵ����ĵ�2λ����Ϊ1�������0x04����ָ��ȷ���ڴ��쳣����ʱ��������ʹ�ý���ջָ�루psp����������ջָ�루msp����
	;ARM������������ջָ�룬psp������ջָ�룩��msp����ջָ�룩�����Ƿֱ����ڲ�ͬ��Ŀ�ġ�
	;pspͨ��������ͨ��Ӧ�ó����̣߳���msp�����쳣�����ϵͳ����Ĺ��ܡ�
	;lr�����ӼĴ�����ͨ�����ڴ洢�������ص�ַ�������жϻ��쳣����ʱ��lr����;�����в�ͬ��
	;�������쳣ʱ��lr��洢һЩ�������Ϣ������������������δ��쳣���ء�
	ORR		lr, lr, #0x04
	;�쳣���أ����ʱ��ջ�е�ʣ�����ݣ�xPSR��PC, r14��r12��r3��r2��r1��r0�����Զ����ص�CPU�Ĵ���
	;ͬʱ��psp��ֵҲ����£�ָ���߳�ջ��ջ��
	BX		lr
	
	;�ӳ���Pendsv_Handler����
	ENDP
	




	ALIGN 4	
		
	END
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		