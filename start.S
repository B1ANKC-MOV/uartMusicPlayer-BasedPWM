.global _start
.global key_isr
.global IRQ_handle
_start:
	ldr sp, =0x40000000             
	mrs r0, cpsr      
	bic r0, r0, #0x00000080     
	msr cpsr, r0  	       
	@ bl clock_init
	bl uart_init
	bl main
halt:
     b halt

key_isr:      
	sub lr, lr, #4     
	stmfd sp!, {r0-r12, lr}     
	bl key_handle    
	ldmfd sp!, {r0-r12, pc}^    

IRQ_handle:      
	sub lr, lr, #4     
	stmfd sp!, {r0-r12, lr}     
	bl irq_handler
	ldmfd sp!, {r0-r12, pc}^

