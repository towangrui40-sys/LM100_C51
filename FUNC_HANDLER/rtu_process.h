#ifndef __RTU_PROCESS_H_
#define __RTU_PROCESS_H_


//TRU CMD    //RTU״̬
typedef enum
{
	RTU_IDlE = 0,        
	RTU_START,        
	RTU_STOP,
	RTU_SET_VLOTAGE,	
	RTU_FAN_ON,
	RTU_FAN_OFF,
	RTU_VALVE_ON,
	RTU_VALVE_OFF,
	
}RTU_StateType;

void RTU_Send_Task_Handler(void);

void RTU_Cmd_Pending(u8 cmd,u16 dat);
#endif
