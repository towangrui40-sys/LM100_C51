#include "main.h"
#include "string.h"


/**************************************************
触摸地址设置在#include "func_handler.h"
显示变量地址在#include "start_win.h"

//点击按键处理回调函数
void btn_click_callback(u16 addr,u16 val)

**************************************************/
SysDataDef xdata str_sys;


int i=0;
void Init_Rtu_RegHoldingBuf(SysDataDef* dat)
{
	usRegHoldingBuf[ADDR_MOTOR_GET_FWD].dat = (u16 *)&dat->fwd;
	usRegHoldingBuf[ADDR_MOTOR_SET_RUN].dat = (u16 *)&dat->set_run;
	usRegHoldingBuf[ADDR_MOTOR_SET_VOL].dat = (u16 *)&dat->set_vol;

	usRegHoldingBuf[ADDR_DOOR].dat = (u16 *)&dat->door;
	usRegHoldingBuf[ADDR_LOCK].dat = (u16 *)&dat->lock;
	usRegHoldingBuf[ADDR_SET_DC1].dat = (u16 *)&dat->set_dc1;
	usRegHoldingBuf[ADDR_SET_DC2].dat = (u16 *)&dat->set_dc2;
	usRegHoldingBuf[ADDR_SET_DC3].dat = (u16 *)&dat->set_dc3;
	usRegHoldingBuf[ADDR_SET_AC1].dat = (u16 *)&dat->set_ac1;
	usRegHoldingBuf[ADDR_SET_AC2].dat = (u16 *)&dat->set_ac2;
	usRegHoldingBuf[ADDR_SET_AC3].dat = (u16 *)&dat->set_ac3;

	usRegHoldingBuf[ADDR_SIDE_TEMP].dat = (u16 *)&dat->side_vol;
	usRegHoldingBuf[ADDR_BOTTOM_TEMP].dat = (u16 *)&dat->bottom_vol;
	usRegHoldingBuf[ADDR_WEIGHT].dat = (u16 *)&dat->weight;

}

void main(void)
{ 
	sys_init();//系统初始化
	Init_Rtu_RegHoldingBuf(&str_sys);
	rtu_data_init();
	btn_init();        
	T5L_HMIslvInit(); //初始化通讯参数
  EA=1;
	Task_Manage_List_Init( );  // 事件任务函数初始化
	win_init();
	while(1)
	{
		Execute_Task_List_RUN();
		T5L_HMIslvSub(); //功能：在线更新触摸屏程序
	}
}



















