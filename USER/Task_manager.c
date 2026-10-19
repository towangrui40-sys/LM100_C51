//############################################################
// FILE:  Task_manager.c
// Created on: 2022å¹?æœ?0æ—?
// Author: Jason
// summary: Task_manager
//############################################################
#include "main.h"

void Send_voltage_service(void);

#define Task_Num   4   //task +1
#define HFPeriod_COUNT     1   	//1ms
#define RTUPeriod_COUNT    50    //50ms
#define FSMPeriod_COUNT    5  	//5ms
#define TIMERPeriod_COUNT  250   //250ms

TaskTime xdata TasksPare[Task_Num];
extern code const Product_TypeDef product_data[PRODUCT_TOTAL];
void Timer_Task_Count(void)
{
	u8 data Task_Count=0;
 for(Task_Count=0;Task_Count<Task_Num;Task_Count++)    //TASK_NUM=5
 {
  if((TasksPare[Task_Count].Task_Count<TasksPare[Task_Count].Task_Period)&&(TasksPare[Task_Count].Task_Period>0))
  {
	  TasksPare[Task_Count].Task_Count++;   // è®¡æ•° äº‹ä»¶ä»»åŠ¡è®¡æ•°
  }
 }
}

void Execute_Task_List_RUN(void)
{
	u8 data	Task_Count = 0;
 for(Task_Count=0;Task_Count < Task_Num;Task_Count++)
 {
  if((TasksPare[Task_Count].Task_Count>=TasksPare[Task_Count].Task_Period)&&(TasksPare[Task_Count].Task_Period > 0))
  {
		TasksPare[Task_Count].Task_Function();  // è¿è¡Œè®¡æ•°çš„æ—¶é—´ä»»åŠ¡å‡½æ•?
	  TasksPare[Task_Count].Task_Count=0;
  }
 }
}
#define SPK_TIME_OUT_TIMES 5		   //èœ‚é¸£å™¨å“5æ¬?
#define MOTOR_PHASE_SEC 6			   //µç»úÕı×ª/Í£Ö¹/·´×ª/Í£Ö¹£¬Ã¿¶ÎÊ±¼ä£¨Ãë£©
void Task_Timer_Sec(void)  //  250msè¿è¡Œä¸€æ¬?
{
	static u16 xdata motor_step = 0;	// 0:Æô¶¯Õı×ª 1:Õı×ª 2:Í£Ö¹ 3:·´×ª 4:Í£Ö¹
	static u16 xdata motor_timer = 0;	// µ±Ç°½×¶ÎÊ£ÓàÃëÊı
	static u16 xdata tick = 0;
	tick++;	
	if(tick >= 4)    //250msè¿›ä¸€æ¬¡ï¼Œ4æ¬¡ä¸º1s
	{
		tick = 0;

		if(win_str_data.state == FSM_FREEZE && win_str_data.use_cooling_time > 0)
		{
			win_str_data.use_cooling_time --;
		}
		if(win_str_data.state == FSM_FREEZE ||  win_str_data.state == FSM_RUN)
		{
			if(win_str_data.total_time > 0)
			{
				win_str_data.total_time--;
				win_disp_total_time(win_str_data.total_time);
			}
			win_str_data.used_time++;   // è¿è¡Œ1ç§’ï¼Œç´¯è®¡è¿è¡Œæ—¶é—´åŠ?ç§?
		}
		
		/* ×¼±¸×´Ì¬£ºµç»úÑ­»·¸´Î»£¬±ÜÃâÉÏ´ÎÖĞÍ¾Í£Ö¹ºó²ĞÁô¼ÆÊ± */
		if(win_str_data.state == FSM_READY)
		{
			motor_step = 0;
			motor_timer = 0;
		}

		/* ¼ÓÒººÍ½µÎÂ½×¶Î£ºÕı×ª -> Í£Ö¹£¨¼ÓÒº½×¶Î³ÆÖØ£© -> ·´×ª -> Í£Ö¹£¨¼ÓÒº½×¶Î³ÆÖØ£© */
		if(win_str_data.state == FSM_FEED_LIQUOR || win_str_data.state == FSM_FREEZE)
		{
			/* æ¯è¿›ä¸€æ¬?ç§’ä»»åŠ¡ï¼Œé˜¶æ®µè®¡æ—¶å‡? */
			if(motor_timer > 0) motor_timer--;

			switch(motor_step)
			{
				case 0:		/* å¯åŠ¨æ­£è½¬ï¼Œå¹¶å¼€å§‹æ­£è½¬è®¡æ—?*/
					RTU_Cmd_Pending(RTU_START,MC_FWD);
					motor_timer = MOTOR_PHASE_SEC;
					motor_step = 1;
					break;

				case 1:		/* Õı×ªÊ±¼äµ½£¬Í£Ö¹µç»ú */
					if(motor_timer == 0)
					{
						RTU_Cmd_Pending(RTU_STOP,MC_STOP);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 2;
					}
					break;

				case 2:		/* åœæ­¢æ—¶é—´åˆ°ï¼šåŠ æ¶²é˜¶æ®µè¯»å–é‡é‡ï¼Œç„¶åå¯åŠ¨åè½?*/
					if(motor_timer == 0)
					{
						if(win_str_data.state == FSM_FEED_LIQUOR)
						{
							win_str_data.add_liquid = (win_str_data.measure_weight - win_str_data.save_weight)*10; //0.01ä¸?0g
							if(win_str_data.add_liquid <= 0) win_str_data.add_liquid = 0;
						}
						RTU_Cmd_Pending(RTU_START,MC_REV);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 3;
					}
					break;

				case 3:		/* ·´×ªÊ±¼äµ½£¬Í£Ö¹µç»ú */
					if(motor_timer == 0)
					{
						RTU_Cmd_Pending(RTU_STOP,MC_STOP);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 4;
					}
					break;

				case 4:		/* åœæ­¢æ—¶é—´åˆ°ï¼šåŠ æ¶²é˜¶æ®µè¯»å–é‡é‡ï¼Œç„¶åé‡æ–°å¯åŠ¨æ­£è½?*/
					if(motor_timer == 0)
					{
						if(win_str_data.state == FSM_FEED_LIQUOR)
						{
							win_str_data.add_liquid = (win_str_data.measure_weight - win_str_data.save_weight)*10; //0.01ä¸?0g
							if(win_str_data.add_liquid <= 0) win_str_data.add_liquid = 0;
						}
						RTU_Cmd_Pending(RTU_START,MC_FWD);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 1;
					}
					break;

				default:	/* å¼‚å¸¸é˜¶æ®µå€¼å¤ä½?*/
					motor_step = 0;
					motor_timer = 0;
					break;
			}
		}
		/* delay°´Ãëµİ¼õ£ºFSM_STARTÖĞdelay=40±íÊ¾µÈ´ı40Ãë */
		if(win_str_data.delay > 0) win_str_data.delay--;
	}
	
	
}
void RTU_send_Rece(void)  // 100msæ•°æ®å‘¨æœŸï¼Œå¯è‡ªè¡Œä¿®æ”¹å‘¨æœŸï¼Œæœ€å¿?ms
{
	RTU_Send_Task_Handler();
}

/*********************çŠ¶æ€æœº******************************/
void Run_FSM(void)   //FSMçŠ¶æ€æœº 
{
	switch(win_str_data.state)
	{
		case FSM_INIT:
			win_str_data.state = FSM_READY;
			break;
		
		case FSM_READY:   //****************çŠ¶æ€æœºï¼šå‡†å¤‡çŠ¶æ€?*******************************************************	
				win_str_data.use_cooling_time = win_str_data.set_cooling_time;
				win_str_data.use_volume = win_str_data.set_volume;
				win_str_data.use_grind_temp = win_str_data.set_grind_temp;
				win_str_data.use_grind_speed = win_str_data.set_grind_speed;
				win_str_data.use_grind_time = win_str_data.set_grind_time;
		
				if(win_str_data.start == 1)    //æ¥æ”¶åˆ°Start_win  start=1å¼€å§‹ä¿¡å?
				{
					if(win_str_data.medium_flag == 1)
						win_str_data.state = FSM_START;
					else if(win_str_data.medium_flag == 2)
					{
						win_str_data.state = FSM_FREEZE;
						RTU_Cmd_Pending(RTU_SET_VLOTAGE,200);
					}
				}
				else
				{
					win_str_data.start = 0;
				}
			break;
		case FSM_START:
					RTU_Cmd_Pending(RTU_VALVE_ON,1);
					win_str_data.run_step = 0;    //0:åŠ æ¶²æ°?1:é™æ¸©  2ï¼šç ”ç£?
					win_str_data.delay = 40;
					win_str_data.state = FSM_WAIT; 
			break;
		case FSM_WAIT:  //ç­‰å¾…æ¸©åº¦åˆ?åº¦ä»¥ä¸?
					if(win_str_data.delay == 0)
					{
						win_str_data.save_weight = win_str_data.measure_weight;
						win_str_data.add_liquid = 0;
						RTU_Cmd_Pending(RTU_SET_VLOTAGE,200);
						win_str_data.state = FSM_FEED_LIQUOR; 						
					}
					if(win_str_data.start == 0) 
					{
						win_str_data.state = FSM_STOP;
					}
			break;
		case FSM_FEED_LIQUOR:  //åŠ æ¶²æ°?  //win_str_data.add_liquid mlæ•?
		
 		      if(win_str_data.add_liquid >= win_str_data.use_volume || win_str_data.disp_temperature <= (int)win_str_data.use_grind_temp)
					{
						RTU_Cmd_Pending(RTU_VALVE_OFF,0);
						RTU_Cmd_Pending(RTU_FAN_ON,1);
						win_str_data.run_step = 1;  //0:åŠ æ¶²æ°?1:é™æ¸©è¿‡ç¨‹  2ï¼šç ”ç£?
						win_str_data.state = FSM_FREEZE; 
					}
					if(win_str_data.start == 0) 
					{
						win_str_data.state = FSM_STOP;
					}
			break;
		case FSM_FREEZE:  //å†·å†»
          if(win_str_data.use_cooling_time == 0)
					{
						RTU_Cmd_Pending(RTU_START,MC_FWD); 
						win_str_data.run_step = 2;   //0:åŠ æ¶²æ°?1:é™æ¸©  2ï¼šç ”ç£?
				  	win_str_data.state = FSM_RUN;
					}
					if(win_str_data.start == 0) 
					{
						win_str_data.state = FSM_STOP;
					}
			break;
		case FSM_RUN:
				Send_voltage_service();

				if(win_str_data.total_time == 0) win_str_data.start=0;
				if(win_str_data.start == 0) win_str_data.state = FSM_STOP;
			break;
		
		case FSM_STOP:
				if(win_str_data.medium_flag == 1) win_change_page(N2_MAIN);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_MAIN);			
		
				RTU_Cmd_Pending(RTU_FAN_OFF,0);  //é£æ‰‡å…³é—­
				RTU_Cmd_Pending(RTU_VALVE_OFF,0); //æ¶²æ°®é˜€é—¨å…³é—?
				RTU_Cmd_Pending(RTU_STOP,MC_STOP);  //ç”µæœºåœæ­¢
				win_str_data.start = 0;
		
				win_str_data.used_time_l = (u16)(win_str_data.used_time & 0xffff);  //Save used time
				win_str_data.used_time_h = (u16)((win_str_data.used_time>>16) & 0xffff); 
				win_str_data.disp_time = win_str_data.used_time/3600;
				norflash_write(RUNTIME_MIN_HADDR,(u8*)&win_str_data.used_time_h,2);
	
		
				win_str_data.state = FSM_READY;
			break;
		default:
			break;
	}
}
void HFPeriod_1msTask(void)
{
	btn_click_handler();						//æŒ‰é’®ç‚¹å‡»äº‹ä»¶å¤„ç†	
	rtu_response_handler();
	win_fresh_window();     //åˆ·æ–°ç•Œé¢
}


void Task_Manage_List_Init(void)
{
	TasksPare[0].Task_Period = HFPeriod_COUNT; //PERIOD_COUNT=1    1ms
	TasksPare[0].Task_Count = 1;
	TasksPare[0].Task_Function = HFPeriod_1msTask;  

	TasksPare[1].Task_Period = RTUPeriod_COUNT; //2ms
	TasksPare[1].Task_Count = 8;
	TasksPare[1].Task_Function = RTU_send_Rece; //  é€šè®¯æ¥æ”¶å’Œå‘é€?

	TasksPare[2].Task_Period = FSMPeriod_COUNT; //100ms
	TasksPare[2].Task_Count=80;
	TasksPare[2].Task_Function = Run_FSM;//  é€šè®¯æ¥æ”¶å’Œå‘é€?

	TasksPare[3].Task_Period = TIMERPeriod_COUNT;//500ms
	TasksPare[3].Task_Count=300;
	TasksPare[3].Task_Function = Task_Timer_Sec;   // 500msçš„LEDçš„é—ªçƒ?
}

void Send_voltage_service(void)
{
	u16 vol_temp;
	vol_temp = win_level_trans_volt(win_str_data.use_grind_speed);   //è¯»å–è®¡ç®—ç”µå‹å€?
	if(str_sys.set_vol != vol_temp)	   //è¯»å–åˆ°ä¸‹ä½æœºç”µå‹å€¼ä¸ç­‰äºè®¾ç½®ç”µå‹å€?
	{
		str_sys.set_vol = vol_temp;
		RTU_Cmd_Pending(RTU_SET_VLOTAGE,vol_temp);
	}
}

