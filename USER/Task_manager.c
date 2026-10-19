//############################################################
// FILE:  Task_manager.c
// Created on: 2022-02-20
// Author: Jason
// summary: Task_manager
//############################################################
#include "main.h"

void Send_voltage_service(void);

#define Task_Num   4  // 任务数量（实际任务数 = Task_Num - 1）
#define HFPeriod_COUNT     1  // 1ms 高频任务
#define RTUPeriod_COUNT    50  // 50ms 通信任务
#define FSMPeriod_COUNT    5  // 5ms 状态机任务
#define TIMERPeriod_COUNT  250  // 250ms 定时任务

TaskTime xdata TasksPare[Task_Num];
extern code const Product_TypeDef product_data[PRODUCT_TOTAL];
void Timer_Task_Count(void)
{
	u8 data Task_Count=0;
 for(Task_Count=0;Task_Count<Task_Num;Task_Count++)  // 遍历所有任务（TASK_NUM=5）
 {
  if((TasksPare[Task_Count].Task_Count<TasksPare[Task_Count].Task_Period)&&(TasksPare[Task_Count].Task_Period>0))
  {
	  TasksPare[Task_Count].Task_Count++;  // 任务计数加1
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
		TasksPare[Task_Count].Task_Function();  // 执行到达周期的时间任务
	  TasksPare[Task_Count].Task_Count=0;
  }
 }
}
#define SPK_TIME_OUT_TIMES 5  // 蜂鸣器响5次
#define MOTOR_PHASE_SEC 6  // 电机正转/停止/反转/停止，每段时间（秒）
void Task_Timer_Sec(void)  // 250ms调用一次
{
	static u16 xdata motor_step = 0;  // 0:启动正转 1:正转 2:停止 3:反转 4:停止
	static u16 xdata motor_timer = 0;  // 当前阶段剩余秒数
	static u16 xdata tick = 0;
	tick++;	
	if(tick >= 4)  // 250ms进一次，4次为1秒
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
			win_str_data.used_time++;  // 运行1秒，累计运行时间加1秒
		}
		
		/* 准备状态：电机循环复位，避免上次中途停止后残留计时 */
		if(win_str_data.state == FSM_READY)
		{
			motor_step = 0;
			motor_timer = 0;
		}

		/* 加液和降温阶段：正转 -> 停止（加液阶段称重） -> 反转 -> 停止（加液阶段称重） */
		if(win_str_data.state == FSM_FEED_LIQUOR || win_str_data.state == FSM_FREEZE)
		{
			/* 每进一次1秒任务，阶段计时减1 */
			if(motor_timer > 0) motor_timer--;

			switch(motor_step)
			{
				case 0:  /* 启动正转，并开始正转计时 */
					RTU_Cmd_Pending(RTU_START,MC_FWD);
					motor_timer = MOTOR_PHASE_SEC;
					motor_step = 1;
					break;

				case 1:  /* 正转时间到，停止电机 */
					if(motor_timer == 0)
					{
						RTU_Cmd_Pending(RTU_STOP,MC_STOP);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 2;
					}
					break;

				case 2:  /* 停止时间到：加液阶段读取重量，然后启动反转 */
					if(motor_timer == 0)
					{
						if(win_str_data.state == FSM_FEED_LIQUOR)
						{
							win_str_data.add_liquid = (win_str_data.measure_weight - win_str_data.save_weight)*10;  // 0.01为10g
							if(win_str_data.add_liquid <= 0) win_str_data.add_liquid = 0;
						}
						RTU_Cmd_Pending(RTU_START,MC_REV);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 3;
					}
					break;

				case 3:  /* 反转时间到，停止电机 */
					if(motor_timer == 0)
					{
						RTU_Cmd_Pending(RTU_STOP,MC_STOP);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 4;
					}
					break;

				case 4:  /* 停止时间到：加液阶段读取重量，然后重新启动正转 */
					if(motor_timer == 0)
					{
						if(win_str_data.state == FSM_FEED_LIQUOR)
						{
							win_str_data.add_liquid = (win_str_data.measure_weight - win_str_data.save_weight)*10;  // 0.01为10g
							if(win_str_data.add_liquid <= 0) win_str_data.add_liquid = 0;
						}
						RTU_Cmd_Pending(RTU_START,MC_FWD);
						motor_timer = MOTOR_PHASE_SEC;
						motor_step = 1;
					}
					break;

				default:  /* 异常阶段值复位 */
					motor_step = 0;
					motor_timer = 0;
					break;
			}
		}
		/* delay按秒递减：FSM_START中delay=20表示等待20秒 */
		if(win_str_data.delay > 0) win_str_data.delay--;
	}
	
	
}
void RTU_send_Rece(void)  // 100ms数据周期，可自行修改周期，最快1ms
{
	RTU_Send_Task_Handler();
}

/********************* 状态机 ******************************/
void Run_FSM(void)  // FSM状态机
{
	switch(win_str_data.state)
	{
		case FSM_INIT:
			win_str_data.state = FSM_READY;
			break;
		
		case FSM_READY:  //**************** 状态机：准备状态 *******************************************************
				win_str_data.use_cooling_time = win_str_data.set_cooling_time;
				win_str_data.use_volume = win_str_data.set_volume;
				win_str_data.use_grind_temp = win_str_data.set_grind_temp;
				win_str_data.use_grind_speed = win_str_data.set_grind_speed;
				win_str_data.use_grind_time = win_str_data.set_grind_time;
		
				if(win_str_data.start == 1)  // 接收到Start_win，start=1开始信号
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
				win_str_data.run_step = 0;  // 0:加液氮 1:降温 2:研磨
				win_str_data.delay = 20;
				win_str_data.state = FSM_WAIT; 
			break;
		case FSM_WAIT:  //等到倒计时到达
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
		case FSM_FEED_LIQUOR:  // 加液氮，win_str_data.add_liquid为ml数
		
 		      if(win_str_data.add_liquid >= win_str_data.use_volume || win_str_data.disp_temperature <= (int)win_str_data.use_grind_temp)
				{
					RTU_Cmd_Pending(RTU_VALVE_OFF,0);
					RTU_Cmd_Pending(RTU_FAN_ON,1);
					win_str_data.run_step = 1;  // 0:加液氮 1:降温过程 2:研磨
					win_str_data.state = FSM_FREEZE; 
				}
				if(win_str_data.start == 0) 
				{
					win_str_data.state = FSM_STOP;
				}
			break;
		case FSM_FREEZE:  // 冷冻
        	  if(win_str_data.use_cooling_time == 0)
				{
					RTU_Cmd_Pending(RTU_START,MC_FWD); 
					win_str_data.run_step = 2;  // 0:加液氮 1:降温 2:研磨
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
		
				RTU_Cmd_Pending(RTU_FAN_OFF,0);  // 风扇关闭
				RTU_Cmd_Pending(RTU_VALVE_OFF,0);  // 液氮阀门关闭
				RTU_Cmd_Pending(RTU_STOP,MC_STOP);  // 电机停止
				win_str_data.start = 0;
		
				win_str_data.used_time_l = (u16)(win_str_data.used_time & 0xffff);  // 保存运行时间低16位
				win_str_data.used_time_h = (u16)((win_str_data.used_time>>16) & 0xffff);  // 保存运行时间高16位
				win_str_data.disp_time = win_str_data.used_time/3600;  // 运行时间换算成小时数
				norflash_write(RUNTIME_MIN_HADDR,(u8*)&win_str_data.used_time_h,2);  // 保存高16位和低16位两个字
	
		
				win_str_data.state = FSM_READY;
			break;
		default:
			break;
	}
}
void HFPeriod_1msTask(void)
{
	btn_click_handler();  // 按钮点击事件处理
	rtu_response_handler();
	win_fresh_window();  // 刷新界面
}


void Task_Manage_List_Init(void)
{
	TasksPare[0].Task_Period = HFPeriod_COUNT;  // 1ms 高频任务
	TasksPare[0].Task_Count = 1;
	TasksPare[0].Task_Function = HFPeriod_1msTask;  

	TasksPare[1].Task_Period = RTUPeriod_COUNT;  // 50ms 通信任务
	TasksPare[1].Task_Count = 8;
	TasksPare[1].Task_Function = RTU_send_Rece;  // 通信接收和发送

	TasksPare[2].Task_Period = FSMPeriod_COUNT;  // 5ms 状态机任务
	TasksPare[2].Task_Count=80;
	TasksPare[2].Task_Function = Run_FSM;  // 状态机处理

	TasksPare[3].Task_Period = TIMERPeriod_COUNT;  // 250ms 定时任务
	TasksPare[3].Task_Count=300;
	TasksPare[3].Task_Function = Task_Timer_Sec;  // 定时器任务（1秒计时、电机循环）
}

void Send_voltage_service(void)
{
	u16 vol_temp;
	vol_temp = win_level_trans_volt(win_str_data.use_grind_speed);  // 读取计算电压值
	if(str_sys.set_vol != vol_temp)  // 读取到的下位机电压值不等于设置电压值
	{
		str_sys.set_vol = vol_temp;
		RTU_Cmd_Pending(RTU_SET_VLOTAGE,vol_temp);
	}
}

