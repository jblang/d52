$mod52       

	DSEG AT 20h
TI_PTR:	DS	1
Rx_Head: DS	1
Rx_Tail: DS	1
TxBuff:	DS	5
TxBuffEnd:
RxBuff:	DS	5
RxBuffEnd:

	CSEG

SerialISR:	PUSH  PSW
	PUSH  ACC
	SETB  RS0
	JBC   TI,TI_ISR
TI_ISREnd:	JBC   RI,RI_ISR
SerialISREnd:	POP   ACC
	POP   PSW
	RETI
TI_ISR:	MOV   A,TI_PTR
	JZ    TI_ISREnd
	DEC   TI_PTR
	ADD   A,#LOW(TxBuff-1)
	MOV   R0,A
	MOV   R2,P2
	CLR   A
	ADDC  A,#HIGH(TxBuff-1)
	MOV   P2,A
	MOVX  A,@R0
	MOV   P2,R2
	MOV   SBUF,A
	SJMP  TI_ISREnd
RI_ISR:	MOV   R1,RX_Head
	MOV   @R1,SBUF
	INC   R1
	CJNE  R1,#RXBuffEnd,RI_ISR_X1
	MOV   R1,#RXBuff
RI_ISR_X1:	MOV   A,R1
	CJNE  A,RX_Tail,RI_ISR_X2
	SJMP  SerialISREnd
RI_ISR_X2:	MOV   RX_Head,A
	SJMP  SerialISREnd
	end