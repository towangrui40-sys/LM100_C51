#include <stdio.h>
#include <string.h>
#include "func_handler.h"
#include "nor_flash.h"
#include "start_win.h"
#include "motor_cmd.h"
#include "main.h"

code const char Software_version[11]={"V4.00"};
code const char modle_version[11]={"--------"};
Product_TypeDef product_data[PRODUCT_TOTAL]={ 
//id, product name,		  level				    time	         temp	           ml	            voltage		
{ 0,	 "LM100",		   500,3600,100,		1,599,10,    -150,20,1,      0,1200,10,       500,3600, },
};																					//350		

STR_WIN_TypeDef xdata win_str_data;

//man page button handler API
static void win_main_btn_click_handler(u16 btn_val);
static void win_para_keypad_btn_click_handler(u16 btn_val);
static void win_pass_keypad_btn_click_handler(u16 btn_val);
static void win_factory_keypad_btn_click_handler(u16 btn_val);
static void win_fac_set_btn_click_handler(u16 btn_val);
static void win_prog_num_sel_btn_click_handler(u16 btn_val);
static void win_prog_para_btn_hold_handler(u16 btn_val);
static void win_light_set_btn_hold_handler(u16 btn_val);		
static void win_language_set_btn_hold_handler(u16 btn_val);
static void win_buzz_set_btn_hold_handler(u16 btn_val);
//function
static void win_calc_function(void);
static void win_load_setpara(void);

void win_save_prog_paras(u16 offaddr,u8 *var_addr,u16 len);
void win_load_prog_paras(u16 offaddr,u8 *var_addr,u16 len);
void win_fresh_model_icon(void);
void disp_input_icon(u16 btn_val);
void disp_input_icon_clean(void);
extern u16 const page_reg_ch[18]={						//中文界面
	{1},//MENU_PAGE
	{3},//FAC_PAGE
	{4},//INFO_PAGE_LOGO
	{5},//INFO_PAGE
	{6},//SET_PAGE
	{8},//PASSWORD_PAGE
	{9},//RESET_PAGE
	{11},//N2_MAIN
	{12},//N2_MAIN_CONFIRM
	{13},//N2_RUN
	{14},//CO2_MAIN
	{15},//CO2_MAIN_CONFIRM
	{16},//CO2_RUN
	{18},//N2_PROG
	{19},//N2_PROG_CONFIRM
	{20},//CO2_PROG
	{21},//CO2_PROG_CONFIRM
	};
	
//点击按键处理回调函数
void btn_click_callback(u16 addr,u16 val)  //处理后清零
{
	switch(addr)
	{
		case BTN_MAIN_PAGE_ADDR:				//Main page button   0x1100 
				win_main_btn_click_handler(val);
			break;
		case BTN_PROG_NUM_SEL_ADDR:	    		//0x1110
				win_prog_num_sel_btn_click_handler(val);
			break;
		case BTN_PARA_KEYPAD_ADDR://0x1114
				win_para_keypad_btn_click_handler(val);
			break;		
		case BTN_PASS_KEYPAD_ADDR:  		//0x1202
				win_pass_keypad_btn_click_handler(val);
			break;
		case BTN_FAC_SET_ADDR:	    		//工厂界面0X1300
				win_fac_set_btn_click_handler(val);
			break;


		default:
			break;
	}
}
//保持按键处理回调函数
void btn_hold_res_callback(u16 addr,u16 val)   //按钮值改变再进入
{
	switch(addr)
	{
		case BTN_PROG_MODE_SET_ADDR://0x1110			
			win_prog_para_btn_hold_handler(val);
			break;
		case BTN_SET_LIGHT_ADDR:
			win_light_set_btn_hold_handler(val);		
			break;	
		case BTN_LANGUAGE_ADDR:
			win_language_set_btn_hold_handler(val);
			break;
		case BTN_SET_BUZZ_ADDR:
			win_buzz_set_btn_hold_handler(val);
			break;
		default:
			break;
	}
}
//保持时间按键点击回调函数
void btn_hold_click_callback(u16 addr,u16 val)
{
	STR_WIN_TypeDef	*obj = &win_str_data;
	val =0;
	if(obj->start == 1) return;
	switch(addr)
	{		
		default:
			break;
	}
}
//保持时间按键到时回调函数
void btn_hold_callback(u16 addr,u16 val)
{
	STR_WIN_TypeDef	*obj = &win_str_data;
	val =0;
	if(obj->start == 1) return;
	switch(addr)
	{
		default:
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////功能调用子函数/////////////////////////////////////////////////////////////////
static void win_calc_function(void)
{
	float xdata temp,temp1;
	STR_WIN_TypeDef	*obj = &win_str_data;	
	temp = product_data[obj->product_id].vol_max-product_data[obj->product_id].vol_min;
	temp1 = product_data[obj->product_id].para1.max-product_data[obj->product_id].para1.min;
	obj->k_factor = temp/temp1;
	
	temp = obj->k_factor*product_data[obj->product_id].para1.min;
	obj->b_factor = product_data[obj->product_id].vol_min-temp;
}

void win_change_page(u16 page)    //切换窗口页面
{
	u16 xdata temp;
	STR_WIN_TypeDef	*obj = &win_str_data;
	
	
	if(page == N2_RUN)  obj->pre_page = page;
//	if(page == SYSTEM_PAGE) obj->pre_page = page;
	if(page == BACK_PAGE) page = obj->pre_page;
	
	temp = page_reg_ch[page];  //Chinese

	//fresh page
	switch(page)
	{
		case N2_MAIN:
		case CO2_MAIN:
				win_disp_cooling_time(win_str_data.set_cooling_time);
				win_disp_volume(win_str_data.set_volume);
				win_disp_temp(win_str_data.set_grind_temp);
				win_disp_speed(win_str_data.set_grind_speed);
				win_disp_grind_time(win_str_data.set_grind_time);
		
				win_disp_prog_num(win_str_data.prog_grp);
			break;
		case N2_RUN:
				win_disp_total_time(win_str_data.total_time);
			break;
		
		
		
		default:break;
	}
	//goto page
	sys_disp_page(temp);
}

//---------------------------------------------------------------//
//name: win_load_setpara
//---------------------------------------------------------------//
static void win_load_setpara(void)
{
	//1.load startup logo
	if(win_str_data.logo >0) win_str_data.logo = 4;
	sys_write_vp(ANIMOTION_LOGO_ADDR,(u8*)&win_str_data.logo,2);	//no logo for oem

	if(win_str_data.light>100) win_str_data.light = 100;
	if(win_str_data.light<10) win_str_data.light = 10;
	sys_setdis_light(win_str_data.light);
	sys_write_vp(BTN_SET_LIGHT_ADDR,(u8*)&win_str_data.light,1);	

	if(win_str_data.alarm > 1) win_str_data.alarm = 1;
	if(win_str_data.alarm < 0) win_str_data.alarm = 0;
	sys_enable_speaker(1-win_str_data.alarm);		
	sys_write_vp(BTN_SET_BUZZ_ADDR,(u8*)&win_str_data.alarm,1);
	
	if(win_str_data.language >= 3) win_str_data.language = 3;
	if(win_str_data.language <= 0) win_str_data.language = 0;	
	sys_write_vp(BTN_LANGUAGE_ADDR,(u8*)&win_str_data.language,1);
	
	if(win_str_data.medium_flag < 1 || win_str_data.medium_flag > 2) win_str_data.medium_flag = 1;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////事件处理函数//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//处理主界面按钮点击事件	
static void win_main_btn_click_handler(u16 btn_val)
{
	#define  BTN_VAL_START	 		   0X01    //start
	#define  BTN_VAL_STOP	 		   0X02    //start	
	#define  BTN_VAL_SET               0X03    //settings into	
	#define  BTN_VAL_CONFOIRM	       0X04
	#define  BTN_VAL_CO2               0X05
	#define  BTN_VAL_N2                0X06
	#define  BTN_VAL_MAIN_PAGE         0X07
	#define  BTN_VAL_SYSTEM_PAGE       0X08
	#define  BTN_VAL_INFO_PAGE         0X09
	
	#define  BTN_VAL_COOLING_TIME_ADD	 0X0A
	#define  BTN_VAL_COOLING_TIME_DEC	 0X0B
	#define  BTN_VAL_VOLUME_ADD	         0X0C
	#define  BTN_VAL_VOLUME_DEC	    	 0X0D	
	#define  BTN_VAL_TEMP_ADD	      	 0X0E
	#define  BTN_VAL_TEMP_DEC	     	 0X0F
	#define  BTN_VAL_SPEED_ADD	   		 0X10
	#define  BTN_VAL_SPEED_DEC	   	 	 0X11
	#define  BTN_VAL_MILL_TIME_ADD		 0X12
	#define  BTN_VAL_MILL_TIME_DEC		 0X13
	
	#define  BTN_RETURN_MAIN 		0X20
	#define  BTN_RETURN_MENU 	    0X21
	#define  BTN_RETURN_SET 	    0X22
	u16 xdata temp = 0;
	switch(btn_val)
	{
		case BTN_VAL_START:
			if(win_str_data.start == 0 && win_str_data.door_state == 1)
			{
				if(win_str_data.medium_flag == 1) win_change_page(N2_MAIN_CONFIRM);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_MAIN_CONFIRM);	
			}
    	 	break;
		case BTN_VAL_STOP:
				win_str_data.start = 0;
    		 break;			

		case BTN_VAL_CONFOIRM:
				win_str_data.start = 1;
				win_str_data.total_time = win_str_data.use_cooling_time + win_str_data.use_grind_time;
					if(win_str_data.medium_flag == 1) win_change_page(N2_RUN);	
						else if(win_str_data.medium_flag == 2) win_change_page(CO2_RUN);	
   			  break;
		
		case BTN_VAL_SET:         //Page1:Setting*****************************
				Fresh_Service_Time();
				win_change_page(MENU_PAGE);
			break;		

		case BTN_VAL_CO2:
				win_change_page(CO2_MAIN);	
				win_str_data.medium_flag = 2;
				norflash_write(SYS_MEDIUM_OFFSET,(u8*)&win_str_data.medium_flag,2);
			break;
		
		case BTN_VAL_N2:
				win_change_page(N2_MAIN);	
				win_str_data.medium_flag = 1;
				norflash_write(SYS_MEDIUM_OFFSET,(u8*)&win_str_data.medium_flag,2);
			break;
		
		case BTN_VAL_MAIN_PAGE:
			if(win_str_data.medium_flag == 1)
				win_change_page(N2_MAIN);	
			else if(win_str_data.medium_flag == 2)
				win_change_page(CO2_MAIN);	
			break;
	  case BTN_VAL_SYSTEM_PAGE:
			  win_change_page(SET_PAGE);	
			break;
	  case BTN_VAL_INFO_PAGE:
				if(win_str_data.logo == 4) //no logo
					win_change_page(INFO_PAGE);	
				else
					win_change_page(INFO_PAGE_LOGO);	
			break;
		case BTN_VAL_SPEED_ADD:  //转速
			win_str_data.set_grind_speed = win_str_data.set_grind_speed + product_data[win_str_data.product_id].para1.unit;
			if(win_str_data.set_grind_speed > product_data[win_str_data.product_id].para1.max) 
				 win_str_data.set_grind_speed = product_data[win_str_data.product_id].para1.max;
			win_disp_speed(win_str_data.set_grind_speed);
			win_str_data.use_grind_speed = win_str_data.set_grind_speed;	
     break;
		case BTN_VAL_SPEED_DEC:
			win_str_data.set_grind_speed = win_str_data.set_grind_speed - product_data[win_str_data.product_id].para1.unit;
		  if(win_str_data.set_grind_speed < product_data[win_str_data.product_id].para1.min) 
				 win_str_data.set_grind_speed = product_data[win_str_data.product_id].para1.min;
			win_disp_speed(win_str_data.set_grind_speed);
			win_str_data.use_grind_speed = win_str_data.set_grind_speed;	
     break;
		case BTN_VAL_VOLUME_ADD:  //液氮干冰量
			win_str_data.set_volume = win_str_data.set_volume + product_data[win_str_data.product_id].para4.unit;
		 if(win_str_data.set_volume > product_data[win_str_data.product_id].para4.max)
		  	win_str_data.set_volume = product_data[win_str_data.product_id].para4.max;
		 win_disp_volume(win_str_data.set_volume);
		 win_str_data.use_volume = win_str_data.set_volume;	
     break;
		case BTN_VAL_VOLUME_DEC:
			win_str_data.set_volume = win_str_data.set_volume - product_data[win_str_data.product_id].para4.unit;
			if(win_str_data.set_volume < product_data[win_str_data.product_id].para4.min) 
				 win_str_data.set_volume = product_data[win_str_data.product_id].para4.min;
			win_disp_volume(win_str_data.set_volume);
			win_str_data.use_volume = win_str_data.set_volume;	
     break;
		case BTN_VAL_TEMP_ADD:  //温度
			win_str_data.set_grind_temp = win_str_data.set_grind_temp + product_data[win_str_data.product_id].para3.unit;
		 if(win_str_data.set_grind_temp > product_data[win_str_data.product_id].para3.max)
		  	win_str_data.set_grind_temp = product_data[win_str_data.product_id].para3.max;
			win_disp_temp(win_str_data.set_grind_temp);
		 win_str_data.use_grind_temp = win_str_data.set_grind_temp;	
     break;
		case BTN_VAL_TEMP_DEC:
			win_str_data.set_grind_temp = win_str_data.set_grind_temp - product_data[win_str_data.product_id].para3.unit;
			if(win_str_data.set_grind_temp < product_data[win_str_data.product_id].para3.min) 
				 win_str_data.set_grind_temp = product_data[win_str_data.product_id].para3.min;
			win_disp_temp(win_str_data.set_grind_temp);
			win_str_data.use_grind_temp = win_str_data.set_grind_temp;	
     break;
		case BTN_VAL_MILL_TIME_ADD: //研磨时间
			win_str_data.set_grind_time = win_str_data.set_grind_time + product_data[win_str_data.product_id].para2.unit;
		 if(win_str_data.set_grind_time > product_data[win_str_data.product_id].para2.max)
		  	win_str_data.set_grind_time = product_data[win_str_data.product_id].para2.max;
			win_disp_grind_time(win_str_data.set_grind_time);
		  win_str_data.use_grind_time = win_str_data.set_grind_time;	
     break;
		case BTN_VAL_MILL_TIME_DEC:
				win_str_data.set_grind_time = win_str_data.set_grind_time - product_data[win_str_data.product_id].para2.unit;
			if(win_str_data.set_grind_time < product_data[win_str_data.product_id].para2.min) 
				 win_str_data.set_grind_time = product_data[win_str_data.product_id].para2.min;
			win_disp_grind_time(win_str_data.set_grind_time);
			win_str_data.use_grind_time = win_str_data.set_grind_time;	
     break;
			
		case BTN_VAL_COOLING_TIME_ADD:  //冷冻时间
			win_str_data.set_cooling_time = win_str_data.set_cooling_time + product_data[win_str_data.product_id].para2.unit;
		 if(win_str_data.set_cooling_time > product_data[win_str_data.product_id].para2.max)
		  	win_str_data.set_cooling_time = product_data[win_str_data.product_id].para2.max;
			win_disp_cooling_time(win_str_data.set_cooling_time);
			win_str_data.use_cooling_time = win_str_data.set_cooling_time;		 
     break;
		case BTN_VAL_COOLING_TIME_DEC:
				win_str_data.set_cooling_time = win_str_data.set_cooling_time - product_data[win_str_data.product_id].para2.unit;
			if(win_str_data.set_cooling_time < product_data[win_str_data.product_id].para2.min) 
				 win_str_data.set_cooling_time = product_data[win_str_data.product_id].para2.min;
			win_disp_cooling_time(win_str_data.set_cooling_time);
			win_str_data.use_cooling_time = win_str_data.set_cooling_time;	
     break;			


		break;
		default:break;
	}
}

#define  BTN_VAL_ACC	    0X02 //升
#define  BTN_VAL_DEC	    0X01 //降
/********************************************************
函数名称：win_test_mode_btn_click_handler
函数功能：程序设置页面九组选择程序
*********************************************************/
static void win_prog_num_sel_btn_click_handler(u16 btn_val)
{
	switch(btn_val)
	{
		case BTN_VAL_ACC:
				if(win_str_data.prog_grp < 20) win_str_data.prog_grp++;	
			break;
		case BTN_VAL_DEC:
				if(win_str_data.prog_grp > 1) win_str_data.prog_grp--;	
			break;
		default:break;
	}

	disp_input_icon_clean();

	sys_write_vp(ARTNUM_MAIN_PROG_NUM_ADDR,(u8*)&win_str_data.prog_grp,1);	
	norflash_write(SYS_LAST_PROG_OFFSET,(u8 *)&win_str_data.prog_grp,2);	
	
	win_load_prog_paras(PROG_COOLING_TIME_OFFSET,(u8 *)&win_str_data.set_cooling_time,6);
	win_disp_cooling_time(win_str_data.set_cooling_time);
	win_disp_volume(win_str_data.set_volume);
	win_disp_temp(win_str_data.set_grind_temp);
	win_disp_speed(win_str_data.set_grind_speed);
	win_disp_grind_time(win_str_data.set_grind_time);
}

#define  BTN_VAL_NO	   		0X1C00
#define  BTN_VAL_YES	   	0X1400 //弹窗√
#define  BTN_VAL_RETURN		0X1000 //返回
#define  BTN_VAL_OVERSET  	0X0800 //覆盖程序
#define  BTN_VAL_TOSET	    0X0400 //主页面选择框'

#define  BTN_VAL_SEL_GRINDING_TIME		    0X05   //总时间   total time
#define  BTN_VAL_SEL_GRINDING_SPEED		    0X04   //总时间   total time
#define  BTN_VAL_SEL_GRINDING_TEMP		    0X03	 //暂停时间 pause time
#define  BTN_VAL_SEL_VOLUME		            0X02   //运行时间 rum time
#define  BTN_VAL_SEL_COOLING_TIME	        0X01   //转速/振幅
void disp_input_icon(u16 btn_val)
{
	sys_write_vp(ICON_INPUT_DISP1_ADDR,(u8 *)&win_str_data.input_disp1,5);
	if(win_str_data.language == 0)  //CN 
	{
		if(btn_val == BTN_VAL_SEL_COOLING_TIME)   win_str_data.input_disp1 = win_str_data.input_disp1;
		if(btn_val == BTN_VAL_SEL_VOLUME)         win_str_data.input_disp2 = win_str_data.input_disp2;
		if(btn_val == BTN_VAL_SEL_GRINDING_TEMP)  win_str_data.input_disp3 = win_str_data.input_disp3;
		if(btn_val == BTN_VAL_SEL_GRINDING_SPEED) win_str_data.input_disp4 = win_str_data.input_disp4;
		if(btn_val == BTN_VAL_SEL_GRINDING_TIME)  win_str_data.input_disp5 = win_str_data.input_disp5;
		sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,5);
	}
	else if(win_str_data.language == 1)  //EN
	{
		if(btn_val == BTN_VAL_SEL_COOLING_TIME) 	win_str_data.input_disp1 = win_str_data.input_disp1 + 1;
		if(btn_val == BTN_VAL_SEL_VOLUME) 		    win_str_data.input_disp2 = win_str_data.input_disp2 + 1;
		if(btn_val == BTN_VAL_SEL_GRINDING_TEMP) 	win_str_data.input_disp3 = win_str_data.input_disp3 + 1;
		if(btn_val == BTN_VAL_SEL_GRINDING_SPEED)	win_str_data.input_disp4 = win_str_data.input_disp4 + 1;
		if(btn_val == BTN_VAL_SEL_GRINDING_TIME)  win_str_data.input_disp5 = win_str_data.input_disp5 + 1;
		sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,5);
	}
	else if(win_str_data.language == 2)  //RUS
	{
		if(btn_val == BTN_VAL_SEL_COOLING_TIME) 	win_str_data.input_disp1 = win_str_data.input_disp1 + 2;
		if(btn_val == BTN_VAL_SEL_VOLUME) 		    win_str_data.input_disp2 = win_str_data.input_disp2 + 2;
		if(btn_val == BTN_VAL_SEL_GRINDING_TEMP) 	win_str_data.input_disp3 = win_str_data.input_disp3 + 2;
		if(btn_val == BTN_VAL_SEL_GRINDING_SPEED)	win_str_data.input_disp4 = win_str_data.input_disp4 + 2;
		if(btn_val == BTN_VAL_SEL_GRINDING_TIME)  win_str_data.input_disp5 = win_str_data.input_disp5 + 2;
		sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,5);
	}
	else  //JAPAN
	{
		if(btn_val == BTN_VAL_SEL_COOLING_TIME) 	win_str_data.input_disp1 = win_str_data.input_disp1 + 3;
		if(btn_val == BTN_VAL_SEL_VOLUME) 		    win_str_data.input_disp2 = win_str_data.input_disp2 + 3;
		if(btn_val == BTN_VAL_SEL_GRINDING_TEMP) 	win_str_data.input_disp3 = win_str_data.input_disp3 + 3;
		if(btn_val == BTN_VAL_SEL_GRINDING_SPEED)	win_str_data.input_disp4 = win_str_data.input_disp4 + 3;
		if(btn_val == BTN_VAL_SEL_GRINDING_TIME)  win_str_data.input_disp5 = win_str_data.input_disp5 + 3;
		sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,4);
	}
}

void disp_input_icon_clean(void)
{
	memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));

	sys_write_vp(BTN_PROG_MODE_SET_ADDR,(u8 *)&win_str_data.input_disp1,2);

	sys_write_vp(ICON_INPUT_DISP1_ADDR,(u8 *)&win_str_data.input_disp1,5);
	sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,5);
}
/********************************************************
函数名称：win_prog_para_btn_hold_handler
函数功能：程序设置处理函数
*********************************************************/
static void win_prog_para_btn_hold_handler(u16 btn_val)
{
	if(win_str_data.paras_sel == BTN_VAL_SEL_COOLING_TIME)
	{
		if(win_str_data.set_cooling_time > product_data[win_str_data.product_id].para2.max) win_str_data.set_cooling_time = product_data[win_str_data.product_id].para2.max;
		else if(win_str_data.set_cooling_time < product_data[win_str_data.product_id].para2.min) win_str_data.set_cooling_time =product_data[win_str_data.product_id].para2.min;
		win_disp_cooling_time(win_str_data.set_cooling_time);		
	}
	else if(win_str_data.paras_sel == BTN_VAL_SEL_VOLUME)
	{
		if(win_str_data.set_volume > product_data[win_str_data.product_id].para4.max) win_str_data.set_volume = product_data[win_str_data.product_id].para4.max;
		else if(win_str_data.set_volume < product_data[win_str_data.product_id].para4.min) win_str_data.set_volume = product_data[win_str_data.product_id].para4.min;
		win_disp_volume(win_str_data.set_volume);						
	}
	else if(win_str_data.paras_sel == BTN_VAL_SEL_GRINDING_TEMP)
	{
		if(win_str_data.set_grind_temp > product_data[win_str_data.product_id].para3.max) win_str_data.set_grind_temp = product_data[win_str_data.product_id].para3.max;
		else if(win_str_data.set_grind_temp < product_data[win_str_data.product_id].para3.min) win_str_data.set_grind_temp = product_data[win_str_data.product_id].para3.min;
		win_disp_temp(win_str_data.set_grind_temp);						
	}
	else if(win_str_data.paras_sel == BTN_VAL_SEL_GRINDING_SPEED)	
	{
		if(win_str_data.set_grind_speed > product_data[win_str_data.product_id].para1.max) win_str_data.set_grind_speed = product_data[win_str_data.product_id].para1.max;
		else if(win_str_data.set_grind_speed < product_data[win_str_data.product_id].para1.min) win_str_data.set_grind_speed = product_data[win_str_data.product_id].para1.min;
		win_disp_speed(win_str_data.set_grind_speed);									
	}
	else if(win_str_data.paras_sel == BTN_VAL_SEL_GRINDING_TIME)	
	{
		if(win_str_data.set_grind_time > product_data[win_str_data.product_id].para2.max) win_str_data.set_grind_time = product_data[win_str_data.product_id].para2.max;
		else if(win_str_data.set_grind_time < product_data[win_str_data.product_id].para2.min) win_str_data.set_grind_time = product_data[win_str_data.product_id].para2.min;
		win_disp_grind_time(win_str_data.set_grind_time);									
	}
	switch(btn_val)	
	{
		case BTN_VAL_SEL_COOLING_TIME:
		  	memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				win_str_data.input_disp1 = 0x01;
				disp_input_icon(btn_val);
				win_str_data.paras_sel = btn_val;
		
				win_str_data.time_edit_dec = 0;
				win_str_data.set_cooling_time = 0;
				win_disp_cooling_time(win_str_data.set_cooling_time);			
			break;		
		case BTN_VAL_SEL_VOLUME:
			  memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				win_str_data.input_disp2 = 0x01;
				disp_input_icon(btn_val);
				win_str_data.paras_sel = btn_val;
			break;
		case BTN_VAL_SEL_GRINDING_TEMP:
			  memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				win_str_data.input_disp3 = 0x01;
				disp_input_icon(btn_val);
				win_str_data.paras_sel = btn_val;		
			break;
		case BTN_VAL_SEL_GRINDING_SPEED:
	    	memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				win_str_data.input_disp4 = 0x01;
				disp_input_icon(btn_val);
				win_str_data.paras_sel = btn_val;
			break;
		case BTN_VAL_SEL_GRINDING_TIME:
		  	memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				win_str_data.input_disp5 = 0x01;
				disp_input_icon(btn_val);
				win_str_data.paras_sel = btn_val;
		
				win_str_data.time_edit_dec = 0;
				win_str_data.set_grind_time = 0;
				win_disp_grind_time(win_str_data.set_grind_time);				
			break;
		
		case BTN_VAL_TOSET:
				if(win_str_data.prog_grp == 0) win_str_data.prog_grp = 1;
				sys_write_vp(ARTNUM_MAIN_PROG_NUM_ADDR,(u8*)&win_str_data.prog_grp,1);
				memcpy((u8 *)&win_str_data.set_cooling_time,(u8 *)&win_str_data.use_cooling_time,10);	         //display set
				win_load_prog_paras(PROG_COOLING_TIME_OFFSET,(u8*)&win_str_data.set_cooling_time,6);  //read prog para from flash
				if(win_str_data.medium_flag == 1) win_change_page(N2_PROG);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_PROG);	
			break;
		case BTN_VAL_RETURN: //Reset the key value when returning and jump page and clean paras_sel
			    disp_input_icon_clean();

				memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				sys_write_vp(ICON_INPUT_DISP1_ADDR,(u8 *)&win_str_data.input_disp1,4);
				sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,4);

				win_str_data.paras_sel = 0;
				if(win_str_data.medium_flag == 1) win_change_page(N2_MAIN);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_MAIN);	
			break;
		case BTN_VAL_OVERSET:  //Jump to pop-up page
				disp_input_icon_clean();

				memset(&win_str_data.input_disp1, 0, 5 * sizeof(win_str_data.input_disp1));
				sys_write_vp(ICON_INPUT_DISP1_ADDR,(u8 *)&win_str_data.input_disp1,4);
				sys_write_vp(ICON_INPUT_WORD1_ADDR,(u8 *)&win_str_data.input_disp1,4);
				if(win_str_data.medium_flag == 1) win_change_page(N2_PROG_CONFIRM);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_PROG_CONFIRM);	
			break;
		case BTN_VAL_YES:  	 //pop-up page YES   Put Set -> Prog and save flash
				win_save_prog_paras(PROG_COOLING_TIME_OFFSET,(u8*)&win_str_data.set_cooling_time,6);	//write norflash
				if(win_str_data.medium_flag == 1) win_change_page(N2_PROG);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_PROG);	
			break;
		case BTN_VAL_NO:
				if(win_str_data.medium_flag == 1) win_change_page(N2_PROG);	
				else if(win_str_data.medium_flag == 2) win_change_page(CO2_PROG);	
			break;

						
		default:
			break;
	}
}

/********************************************************
时间编辑显示：从后往前输入，按 MM:SS 原样显示编辑中的数字
*********************************************************/
static void win_disp_cooling_time_edit(u16 dec)
{
	u16 m_dat = dec / 100;
	u16 s_dat = dec % 100;
	sys_write_vp(ARTNUM_MAIN_COOLING_TIME_ADDR,(u8 *)&s_dat,1);
	sys_write_vp(ARTNUM_MAIN_COOLING_TIME_HADDR,(u8 *)&m_dat,1);
}

static void win_disp_grind_time_edit(u16 dec)
{
	u16 m_dat = dec / 100;
	u16 s_dat = dec % 100;
	sys_write_vp(ARTNUM_MAIN_GRINDING_TIME_ADDR,(u8 *)&s_dat,1);
	sys_write_vp(ARTNUM_MAIN_GRINDING_TIME_HADDR,(u8 *)&m_dat,1);
}

/********************************************************
函数名称：win_para_keypad_btn_click_handler
函数功能：数字键盘处理函数
*********************************************************/
static void win_para_keypad_btn_click_handler(u16 btn_val)
{
	#define  BTN_VAL_1		0x01
	#define  BTN_VAL_2		0x02
	#define  BTN_VAL_3	    0x03
	#define  BTN_VAL_4      0x04
	#define  BTN_VAL_5      0x05
	#define  BTN_VAL_6		0x06
	#define  BTN_VAL_7		0x07
	#define  BTN_VAL_8      0x08
	#define  BTN_VAL_9		0x09
	#define  BTN_VAL_0		0x0A	
	#define  BTN_VAL_cancel	   0X0B
	#define  BTN_VAL_negative  0X0C
	s16 xdata temp;
	switch(win_str_data.paras_sel)
	{
/****************************SEL_LEVEL***********************************/		
		case BTN_VAL_SEL_COOLING_TIME:
			temp = win_str_data.time_edit_dec;
			if(btn_val<BTN_VAL_cancel) //***********数字按键************
			{
				if(btn_val == 10) btn_val =0;
				temp = (temp*10 + btn_val)%1000;
			}
			else		//*******************cancel***********************
			{
				temp = temp/10;
			}
			win_str_data.time_edit_dec = temp;
			win_str_data.set_cooling_time = (temp/100)*60 + (temp%100>59 ? 59 : temp%100);
			if(win_str_data.set_cooling_time > product_data[win_str_data.product_id].para2.max)
				win_str_data.set_cooling_time = product_data[win_str_data.product_id].para2.max;
			win_disp_cooling_time_edit(win_str_data.time_edit_dec);		
			break;
/*******************************RUN_TIME***********************************/			
		case BTN_VAL_SEL_VOLUME:
			temp = win_str_data.set_volume;
			if(btn_val<BTN_VAL_cancel) //****Number button****
			{
				if(btn_val == 10) btn_val = 0;
				temp = temp%1000;
				temp = temp*10;
				temp += btn_val;
			}
			else//***cance****
			{
				temp = temp/10;
			}	
			win_str_data.set_volume = temp;	
			win_disp_volume(win_str_data.set_volume);
			break;
/*****************************PAUSE_TIME***********************************/		
		case BTN_VAL_SEL_GRINDING_TEMP:
			temp = win_str_data.set_grind_temp;
			if(btn_val == BTN_VAL_negative)
			{
				temp = 0-temp;
			}
			else if(btn_val<BTN_VAL_cancel) //****Number button****
			{
				if(btn_val == 10) btn_val = 0;
				temp = temp%100;
				temp = temp*10;
				temp = (temp>=0)? (temp+btn_val):(temp-btn_val);
			}
			else//***cance****
			{
				temp = temp/10;
			}
			win_str_data.set_grind_temp = temp;	
		
			win_disp_temp(win_str_data.set_grind_temp);
			break;
			
/****************************TOTAL_TIME***********************************/
		case BTN_VAL_SEL_GRINDING_SPEED:
			temp = win_str_data.set_grind_speed;
			if(btn_val<BTN_VAL_cancel) //****Number button****
			{
				if(btn_val == 10) btn_val = 0;
				temp = temp%1000;
				temp = temp*10;
				temp += btn_val;
			}
			else//***cance****
			{
				temp = temp/10;
			}	
			win_str_data.set_grind_speed = temp;	
			win_disp_speed(win_str_data.set_grind_speed);
			break;
		case BTN_VAL_SEL_GRINDING_TIME:
			temp = win_str_data.time_edit_dec;
			if(btn_val<BTN_VAL_cancel) //***********数字按键************
			{
				if(btn_val == 10) btn_val =0;
				temp = (temp*10 + btn_val)%1000;
			}
			else		//*******************cancel***********************
			{
				temp = temp/10;
			}
			win_str_data.time_edit_dec = temp;
			win_str_data.set_grind_time = (temp/100)*60 + (temp%100>59 ? 59 : temp%100);
			if(win_str_data.set_grind_time > product_data[win_str_data.product_id].para2.max)
				win_str_data.set_grind_time = product_data[win_str_data.product_id].para2.max;
			win_disp_grind_time_edit(win_str_data.time_edit_dec);		
			break;
		default:
			break;
	}
}
/********************************************************
win_pass_keypad_btn_click_handler
函数功能：后台密码数字键盘处理函数
*********************************************************/
static void win_pass_keypad_btn_click_handler(u16 btn_val)
{
	u16 xdata temp;
	temp = win_str_data.passcode;
	if(btn_val == 12)   //弹窗后 还原时间
	{
		win_str_data.used_time_h = 0;
		win_str_data.used_time_l = 0;

		norflash_write(RUNTIME_MIN_HADDR,(u8 *)&win_str_data.used_time_h,2);

		win_str_data.used_time = 0;
		win_str_data.disp_time = 0;  //小时数
	
		sys_write_vp(ARTNUM_UT_ADDR,(u8*)&win_str_data.disp_time,1);	 //总运行时间清零
		win_str_data.passcode = 0;
			
		win_change_page(SET_PAGE);

	}
	if(btn_val == 13)   //X
	{
		win_change_page(SET_PAGE);
	}
	else if(btn_val < 11)
	{
		if(btn_val == 10) btn_val =0;
		temp = temp%1000;
		temp = temp*10;
		temp +=btn_val;
		if(temp>9999) temp = 9999;
		else if(temp <0) temp = 0;
		win_str_data.passcode = temp;
		sys_write_vp(ARTNUM_PASSCODE_ADDR,(u8*)&win_str_data.passcode,1);				
	}
	else
	{
		temp= temp/10;
		if(temp <0) temp=0;
		win_str_data.passcode = temp;
		sys_write_vp(ARTNUM_PASSCODE_ADDR,(u8*)&win_str_data.passcode,1);			
	}
	if(temp == 1010)            //复位密码
	{
		win_str_data.passcode = 0;
		sys_write_vp(ARTNUM_PASSCODE_ADDR,(u8*)&win_str_data.passcode,1);
		win_change_page(RESET_PAGE);
	}
	else if(temp == 5973)      //后台密码
	{
		win_change_page(FAC_PAGE);
		win_str_data.passcode = 0;
		sys_write_vp(ARTNUM_PASSCODE_ADDR,(u8*)&win_str_data.passcode,1);	
	}
}
void Fresh_Service_Time(void)
{
	win_str_data.disp_time = win_str_data.used_time/3600;  //小时数
	sys_write_vp(ARTNUM_UT_ADDR,(u8*)&win_str_data.disp_time,1);
}

#define BTN_LOGO      	0X08
/********************************************************
函数名：win_fac_set_btn_click_handler(u16 btn_val)
函数功能：工厂界面
*********************************************************/
static void win_fac_set_btn_click_handler(u16 btn_val)
{
	switch(btn_val)//后台界面0X1300
	{
		case BTN_LOGO://logo
			win_str_data.logo = 4 - win_str_data.logo;
			if(win_str_data.logo >0) win_str_data.logo = 4;
			sys_write_vp(ANIMOTION_LOGO_ADDR,(u8*)&win_str_data.logo,2);	//no logo for oem	
			norflash_write(SYS_LOGO_OFFSET,(u8 *)&win_str_data.logo,2);
			if(win_str_data.logo == 4)	//无logo
				sys_write_vp(STRING_PRODUCT_ADDR,(u8*)modle_version,sizeof(modle_version));
			else
				sys_write_vp(STRING_PRODUCT_ADDR,(u8*)product_data[win_str_data.product_id].name,sizeof(product_data[win_str_data.product_id].name));
			break;

		default:
			break;
	}
}
static void win_light_set_btn_hold_handler(u16 btn_val)	
{
	win_str_data.light = btn_val;
	sys_setdis_light(win_str_data.light);
	norflash_write(SYS_LIGHT_OFFSET,(u8 *)&win_str_data.light,2);	
}
static void win_language_set_btn_hold_handler(u16 btn_val)
{
	win_str_data.language = btn_val;
	norflash_write(SYS_LANGUAGE_OFFSET,(u8 *)&win_str_data.language,2);		
}
static void win_buzz_set_btn_hold_handler(u16 btn_val)
{
	win_str_data.alarm = btn_val;
	sys_enable_speaker(1-win_str_data.alarm);
	norflash_write(SYS_BUZZ_OFFSET,(u8 *)&win_str_data.alarm,2);		
}
/********************************************************
函数名称：void win_save_prog_paras(u16 offaddr,u8 *var_addr,u16 len)
函数功能：存储参数
函数内容：
u16 offaddr   flash起始地址
u8 *var_addr  变量地址
u16 len       变量长度 
*********************************************************/
void win_save_prog_paras(u16 offaddr,u8 *var_addr,u16 len)
{
	u16 addr;
	STR_WIN_TypeDef	*obj = &win_str_data;	

	addr = 0x10 * win_str_data.prog_grp + offaddr;
	
	norflash_write(addr,var_addr,len);
}

#define CLAMP(var, pmin, pmax) do { \
    if ((var) > (pmax)) (var) = (pmax); \
    else if ((var) < (pmin)) (var) = (pmin); \
} while(0)

void win_load_prog_paras(u16 offaddr, u8 *var_addr, u16 len)
{
    Product_TypeDef *pd = &product_data[win_str_data.product_id];

    norflash_read(0x10 * win_str_data.prog_grp + offaddr, var_addr, len);

    CLAMP(win_str_data.set_cooling_time, pd->para2.min, pd->para2.max);
    CLAMP(win_str_data.set_volume,       pd->para4.min, pd->para4.max);
    CLAMP(win_str_data.set_grind_temp,   pd->para3.min, pd->para3.max);
    CLAMP(win_str_data.set_grind_speed,  pd->para1.min, pd->para1.max);
    CLAMP(win_str_data.set_grind_time,   pd->para2.min, pd->para2.max);
}


void win_disp_total_time(u16 val)
{
	u16 xdata m_dat;
	u16 xdata s_dat;
	m_dat = (val%3600)/60;
    s_dat = (val%3600)%60;
	sys_write_vp(ARTNUM_WORK_TOTAL_TIME_ADDR,(u8*)&s_dat,1);
	sys_write_vp(ARTNUM_WORK_TOTAL_TIME_HADDR,(u8*)&m_dat,1);
}

void win_disp_speed(u16 val)
{
	sys_write_vp(ARTNUM_MAIN_SPEED_ADDR,(u8*)&val,1);
}
void win_disp_volume(u16 val)
{
	sys_write_vp(ARTNUM_MAIN_VOLUME_ADDR,(u8*)&val,1);
}
void win_disp_temp(u16 val)
{
	sys_write_vp(ARTNUM_MAIN_TEMP_ADDR,(u8*)&val,1);
}

void win_disp_grind_time(u16 val)
{
	u16 xdata m_dat;
	u16 xdata s_dat;
	m_dat = (val%3600)/60;
  s_dat = (val%3600)%60;
	sys_write_vp(ARTNUM_MAIN_GRINDING_TIME_ADDR,(u8*)&s_dat,1);
	sys_write_vp(ARTNUM_MAIN_GRINDING_TIME_HADDR,(u8*)&m_dat,1);
}
void win_disp_cooling_time(u16 val)
{
	u16 xdata m_dat;
	u16 xdata s_dat;
	m_dat = (val%3600)/60;
  s_dat = (val%3600)%60;
	sys_write_vp(ARTNUM_MAIN_COOLING_TIME_ADDR,(u8*)&s_dat,1);
	sys_write_vp(ARTNUM_MAIN_COOLING_TIME_HADDR,(u8*)&m_dat,1);
}
void win_disp_real_temper(u16 val1,u16 val2)
{
	
	win_str_data.side_temp = (val1*0.1212) - 200;

	win_str_data.disp_temperature = win_str_data.side_temp;
		
	
	if(win_str_data.disp_temperature < -196) 
	{
		win_str_data.disp_temperature = 0;
	}
	else
	{
		if((int)win_str_data.disp_temperature <= 0)
  		if(win_str_data.disp_temperature <= (int)win_str_data.use_grind_temp)
		{
			win_str_data.disp_temperature = win_str_data.use_grind_temp;
		}
	}
	

	sys_write_vp(ARTNUM_FAC_BOTTOM_TEMP_ADDR,(u8*)&win_str_data.bottom_temp,1);
	sys_write_vp(ARTNUM_FAC_SIDE_TEMP_ADDR,(u8*)&win_str_data.side_temp,1);

	sys_write_vp(ARTNUM_MAIN_DISP_TEMP_ADDR,(u8*)&win_str_data.disp_temperature,1);
}
void win_disp_weight(u16 val)
{
	sys_write_vp(ARTNUM_MAIN_WEIGHT_ADDR,(u8*)&val,1);
}
void win_disp_prog_num(u16 val)
{
	sys_write_vp(ARTNUM_MAIN_PROG_NUM_ADDR,(u8*)&val,1);
}




//****************************窗口数据初始化****************************
void win_init(void)
{
	u32 xdata temp = 0;
//init workbence para
	memset(&win_str_data,0,sizeof(win_str_data));
//read from flash///////////////////////////////////////////
	norflash_read(SYS_LOGO_OFFSET,(u8 *)&win_str_data.logo,8);
	
	win_str_data.product_id = 0;

//2.load system parametes settings
	win_load_setpara();	

	//擦除后的Flash两个字都是0xFFFF，按0处理
	if(win_str_data.used_time_h == 0xFFFF && win_str_data.used_time_l == 0xFFFF)
	{
		win_str_data.used_time_h = 0;
		win_str_data.used_time_l = 0;
	}
	win_str_data.used_time = ((u32)win_str_data.used_time_h << 16) | win_str_data.used_time_l;
	win_str_data.disp_time = win_str_data.used_time/3600;  //小时数//7.机型匹配	
	sys_write_vp(ARTNUM_UT_ADDR,(u8*)&win_str_data.disp_time,1);
	
	if(win_str_data.logo == 4) //no logo
		sys_write_vp(STRING_PRODUCT_ADDR,(u8*)modle_version,sizeof(modle_version));
	else
		sys_write_vp(STRING_PRODUCT_ADDR,(u8*)product_data[win_str_data.product_id].name,sizeof(product_data[win_str_data.product_id].name));
	sys_write_vp(STRING_VERSION_ADDR,(u8*)Software_version,sizeof(Software_version));
	
	if(win_str_data.prog_grp > 20 || win_str_data.prog_grp < 1) win_str_data.prog_grp = 1;
	win_load_prog_paras(PROG_COOLING_TIME_OFFSET,(u8 *)&win_str_data.set_cooling_time,6);
	memcpy((u8 *)&win_str_data.use_cooling_time,(u8 *)&win_str_data.set_cooling_time,10);	         //display set
	
//10. calc level vs voltage function
	win_calc_function();	
	sys_delay_about_ms(4000);  
//11. jump work page	

	if(win_str_data.medium_flag == 1) win_change_page(N2_MAIN);	
	else if(win_str_data.medium_flag == 2) win_change_page(CO2_MAIN);	
}
/******************************************************************************************
函数：void win_fresh_window(SysDataDef *sys)
函数功能：刷新窗口图标																														
*******************************************************************************************/
void win_fresh_window()
{
	if(str_sys.door == 0)
	{
		win_str_data.door_state = 1;
	}
	else
	{
		win_str_data.door_state = 0;
		win_str_data.start = 0;
	}
	
	sys_write_vp(ICON_DOOR_ADDR,(u8*)&win_str_data.door_state,1);
	
	win_disp_real_temper(str_sys.side_vol,str_sys.bottom_vol);  //主页面显示温度
	win_str_data.measure_weight = str_sys.weight;
	win_disp_weight(win_str_data.measure_weight);  //主页面显示测量重量
	sys_write_vp(ARTNUM_WORK_ADD_LIQUID_ADDR,(u8*)&win_str_data.add_liquid,1);
	sys_write_vp(ICON_RUN_STEP_ADDR,(u8*)&win_str_data.run_step,1);
}

/******************************************************************************************
函数：u16 win_level_trans_volt(u16 level)
函数功能：计算输出电压值																																		
*******************************************************************************************/
u16 win_level_trans_volt(u16 level)       //计算电压值
{
	float temp;
	temp = level;
	
	temp = (float)(win_str_data.k_factor * temp) + win_str_data.b_factor;	
	
	if(temp >6000)
	temp =6000;
	
	return (u16)temp;
}
