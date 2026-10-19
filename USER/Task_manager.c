//############################################################
// FILE:  Task_manager.c
// Created on: 2022年2月20日
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
	  TasksPare[Task_Count].Task_Count++;   // 计数 事件任务计数
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
		TasksPare[Task_Count].Task_Function();  // 运行计数的时间任务函数
	  TasksPare[Task_Count].Task_Count=0;
  }
 }
}
#define SPK_TIME_OUT_TIMES 5		   //蜂鸣器响5次
void Task_Timer_Sec(void)  //  250ms运行一次
{
	static u16 xdata state = 0;
	static u16 xdata flag = 0;
	static u16 xdata tick = 0;
	tick++;	
	if(tick >= 4)    //250ms进一次，4次为1s
	{
		tick = 0;

		if(win_str_data.state == FSM_FREEZE)
		{
			win_str_data.use_cooling_time --;
		}
		if(win_str_data.state == FSM_FREEZE ||  win_str_data.state == FSM_RUN)
		{
			win_str_data.total_time--;
			win_disp_total_time(win_str_data.total_time);
			win_str_data.used_time++;
		}
		
		if(win_str_data.state == FSM_READY)
			state = 0;
		if(win_str_data.state == FSM_FEED_LIQUOR || win_str_data.state == FSM_FREEZE)
		{
			flag ++;
			if(state == 0)
			{
				RTU_Cmd_Pending(RTU_START,MC_FWD); 
				state = 1;
			}
			if(state == 1)
			{
				if(flag >= 6)
				{
					flag = 0;
					RTU_Cmd_Pending(RTU_STOP,MC_STOP); 
					state = 2;
				}
			}
			if(state == 2)
			{
				if(flag >= 6)
				{
					flag = 0;
					if(win_str_data.state == FSM_FEED_LIQUOR)
					win_str_data.add_liquid = (win_str_data.measure_weight - win_str_data.save_weight)*10; //0.01为10g
					if(win_str_data.add_liquid <= 0) win_str_data.add_liquid = 0;
					state = 3;
				}
			}
			if(state == 3)
			{
				flag = 0;
				RTU_Cmd_Pending(RTU_START,MC_REV); 
				state = 4;
			}
			if(state == 4)
			{
				if(flag >= 6)
				{
					flag = 0;
					RTU_Cmd_Pending(RTU_STOP,MC_STOP); 
					state = 5;
				}
			}
			if(state == 5)
			{
				if(flag >= 6)
				{
					flag = 0;
					if(win_str_data.state == FSM_FEED_LIQUOR)
					win_str_data.add_liquid = (win_str_data.measure_weight - win_str_data.save_weight)*10; //0.01为10g
					if(win_str_data.add_liquid <= 0) win_str_data.add_liquid = 0;
					state = 0;
				}
			}	
		}
	}
	
	
	if(win_str_data.delay > 0) win_str_data.delay--;
}
void RTU_send_Rece(void)  // 100ms数据周期，可自行修改周期，最快1ms
{
	RTU_Send_Task_Handler();
}

/*********************状态机******************************/
void Run_FSM(void)   //FSM状态机 
{
	switch(win_str_data.state)
	{
		case FSM_INIT:
			win_str_data.state = FSM_READY;
			break;
		
		case FSM_READY:   //****************状态机：准备状态********************************************************	
				win_str_data.use_cooling_time = win_str_data.set_cooling_time;
				win_str_data.use_volume = win_str_data.set_volume;
				win_str_data.use_grind_temp = win_str_data.set_grind_temp;
				win_str_data.use_grind_speed = win_str_data.set_grind_speed;
				win_str_data.use_grind_time = win_str_data.set_grind_time;
		
				if(win_str_data.start == 1)    //接收到Start_win  start=1开始信号
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
					win_str_data.run_step = 0;    //0:加液氮 1:降温  2：研磨 
					win_str_data.delay = 40;
					win_str_data.state = FSM_WAIT; 
			break;
		case FSM_WAIT:  //等待温度到5度以下
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
		case FSM_FEED_LIQUOR:  //加液氮   //win_str_data.add_liquid ml数
		
 		      if(win_str_data.add_liquid >= win_str_data.use_volume || win_str_data.disp_temperature <= (int)win_str_data.use_grind_temp)
					{
						RTU_Cmd_Pending(RTU_VALVE_OFF,0);
						RTU_Cmd_Pending(RTU_FAN_ON,1);
						win_str_data.run_step = 1;  //0:加液氮 1:降温过程  2：研磨 
						win_str_data.state = FSM_FREEZE; 
					}
					if(win_str_data.start == 0) 
					{
						win_str_data.state = FSM_STOP;
					}
			break;
		case FSM_FREEZE:  //冷冻
          if(win_str_data.use_cooling_time == 0)
					{
						RTU_Cmd_Pending(RTU_START,MC_FWD); 
						win_str_data.run_step = 2;   //0:加液氮 1:降温  2：研磨 
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
		
				RTU_Cmd_Pending(RTU_FAN_OFF,0);  //风扇关闭
				RTU_Cmd_Pending(RTU_VALVE_OFF,0); //液氮阀门关闭
				RTU_Cmd_Pending(RTU_STOP,MC_STOP);  //电机停止
				win_str_data.start = 0;
		
				win_str_data.used_time_l = win_str_data.used_time & 0xffff;  //Save used time
				win_str_data.used_time_h = (win_str_data.used_time>>16) & 0xffff; 
				norflash_write(RUNTIME_MIN_HADDR,(u8*)&win_str_data.used_time_h,2);
	
		
				win_str_data.state = FSM_READY;
			break;
		default:
			break;
	}
}
void HFPeriod_1msTask(void)
{
	btn_click_handler();						//按钮点击事件处理	
	rtu_response_handler();
	win_fresh_window();     //刷新界面
}


void Task_Manage_List_Init(void)
{
	TasksPare[0].Task_Period = HFPeriod_COUNT; //PERIOD_COUNT=1    1ms
	TasksPare[0].Task_Count = 1;
	TasksPare[0].Task_Function = HFPeriod_1msTask;  

	TasksPare[1].Task_Period = RTUPeriod_COUNT; //2ms
	TasksPare[1].Task_Count = 8;
	TasksPare[1].Task_Function = RTU_send_Rece; //  通讯接收和发送;

	TasksPare[2].Task_Period = FSMPeriod_COUNT; //100ms
	TasksPare[2].Task_Count=80;
	TasksPare[2].Task_Function = Run_FSM;//  通讯接收和发送;

	TasksPare[3].Task_Period = TIMERPeriod_COUNT;//500ms
	TasksPare[3].Task_Count=300;
	TasksPare[3].Task_Function = Task_Timer_Sec;   // 500ms的LED的闪烁
}

void Send_voltage_service(void)
{
	u16 vol_temp;
	vol_temp = win_level_trans_volt(win_str_data.use_grind_speed);   //读取计算电压值
	if(str_sys.set_vol != vol_temp)	   //读取到下位机电压值不等于设置电压值
	{
		str_sys.set_vol = vol_temp;
		RTU_Cmd_Pending(RTU_SET_VLOTAGE,vol_temp);
	}
}

