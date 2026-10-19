#ifndef __START_WIN_H__
#define __START_WIN_H__
#include "sys.h"

#define PRODUCT_TOTAL 1      //机型数量
/*---------------------define arae-------------------------*/
/********************************************************
地址OX2000开头 显示变量
地址OX2000开头 显示变量
系统变量接口（0x0000-0x0FFF 变量存储器空间）
0x1000-0xFFFF 变量存储空间用户可以任意使用。

**********************LOGO*****************************/
#define ANIMOTION_LOGO_ADDR		    0x2100  

//WORK PAGE VAR
/*******************主页面显示*************************/
//0x2000~0x201F
#define ARTNUM_MAIN_COOLING_TIME_ADDR		    	  0x2000  //降温时间
#define ARTNUM_MAIN_COOLING_TIME_HADDR		    	0x2001  //降温时间
#define ARTNUM_MAIN_VOLUME_ADDR		    	        0x2002  //液氮量
#define ARTNUM_MAIN_TEMP_ADDR		    	          0x2003  //设置温度
#define ARTNUM_MAIN_SPEED_ADDR		    	        0x2004  //研磨速度
#define ARTNUM_MAIN_GRINDING_TIME_ADDR		      0x2005  //研磨时间
#define ARTNUM_MAIN_GRINDING_TIME_HADDR		    	0x2006  //研磨时间
#define ARTNUM_MAIN_DISP_TEMP_ADDR		          0x2007  //显示温度
#define ARTNUM_MAIN_WEIGHT_ADDR                 0x2008  //显示总重量
#define ARTNUM_MAIN_PROG_NUM_ADDR		    	      0x2009  //程序序号

#define ARTNUM_WORK_ADD_LIQUID_ADDR		    0x2010
#define ARTNUM_WORK_TOTAL_TIME_ADDR		    0x2011  //总时间
#define ARTNUM_WORK_TOTAL_TIME_HADDR		  0x2012

#define ARTNUM_FAC_BOTTOM_TEMP_ADDR		    0x2020
#define ARTNUM_FAC_SIDE_TEMP_ADDR		      0x2021


//功能图标

#define ICON_DOOR_ADDR  	    0x2501
#define ICON_RUN_STEP_ADDR  	0x2502
#define ICON_INFO_ADDR       	0x2503
#define ICON_POP_ADDR       	0x2508

#define ICON_INPUT_DISP1_ADDR  	0x2510
#define ICON_INPUT_DISP2_ADDR  	0x2511
#define ICON_INPUT_DISP3_ADDR   0x2512
#define ICON_INPUT_DISP4_ADDR   0x2513
#define ICON_INPUT_DISP5_ADDR   0x2514

#define ICON_INPUT_WORD1_ADDR  	0x2515
#define ICON_INPUT_WORD2_ADDR  	0x2516
#define ICON_INPUT_WORD3_ADDR   0x2517
#define ICON_INPUT_WORD4_ADDR   0x2518
#define ICON_INPUT_WORD5_ADDR   0x2519

#define ARTNUM_PASSCODE_ADDR					0x2060  //显示输入密码

#define ARTNUM_UT_ADDR								0x2072 //service used time	 总运行时间

//string
#define STRING_PRODUCT_ADDR 0x3000
#define STRING_VERSION_ADDR 0x3100



//nor flash address list,the address lentgh is 2 bytes 


// each addr include 2 byte
#define SYS_LOGO_OFFSET					0X00
#define SYS_LANGUAGE_OFFSET			0X01
#define SYS_LIGHT_OFFSET				0X02
#define SYS_BUZZ_OFFSET				  0X03
#define RUNTIME_MIN_HADDR 	   	0X04
#define RUNTIME_MIN_LADDR 			0X05
#define SYS_MEDIUM_OFFSET			  0X06
#define SYS_LAST_PROG_OFFSET		0X07

#define PROG1_BASE_NORADDR		0x10
#define PROG2_BASE_NORADDR		0x20
#define PROG3_BASE_NORADDR		0x30
#define PROG4_BASE_NORADDR		0x40
#define PROG5_BASE_NORADDR		0x50
#define PROG6_BASE_NORADDR		0x60
#define PROG7_BASE_NORADDR		0x70
#define PROG8_BASE_NORADDR		0x80
#define PROG9_BASE_NORADDR		0x90
#define PROG10_BASE_NORADDR		0xA0
#define PROG11_BASE_NORADDR		0xB0
#define PROG12_BASE_NORADDR		0xC0
#define PROG13_BASE_NORADDR		0xD0
#define PROG14_BASE_NORADDR		0xE0
#define PROG15_BASE_NORADDR		0xF0
#define PROG16_BASE_NORADDR		0x110
#define PROG17_BASE_NORADDR		0x120
#define PROG18_BASE_NORADDR		0x130
#define PROG19_BASE_NORADDR		0x140
#define PROG20_BASE_NORADDR		0x150
// each addr include 2 byte
#define PROG_COOLING_TIME_OFFSET	 0
#define PROG_VOLUME_OFFSET 			   1
#define PROG_GRIND_TEMP_OFFSET		 2
#define PROG_GRIND_SPEED_OFFSET		 3
#define PROG_GRIND_TIME_OFFSET		 4

/*-------------------------------------------------------*/


typedef enum{
	MENU_PAGE = 0, //菜单界面
	FAC_PAGE,      //LOGO界面
	INFO_PAGE_LOGO,     //信息界面
	INFO_PAGE,     //信息界面
	SET_PAGE,      //设置界面
	PASSWORD_PAGE,
	RESET_PAGE,
	N2_MAIN,
	N2_MAIN_CONFIRM,
	N2_RUN,	
	CO2_MAIN,
	CO2_MAIN_CONFIRM,
	CO2_RUN,
	N2_PROG,
	N2_PROG_CONFIRM,
	CO2_PROG,
	CO2_PROG_CONFIRM,
	BACK_PAGE,
}Change_Page_Def;



	
typedef struct{
	short min;	//para min value
	short max;	//para max value
	short unit;	//para unit步进
}PARA_RangeDef;

typedef struct{
	u16 model_id;
	char name[20];//product name

	PARA_RangeDef para1;//level
	PARA_RangeDef para2;//total time
	PARA_RangeDef para3;//work time
	PARA_RangeDef para4;//pause time
	
	u16 vol_min;		//the minimum level vs the minimum output voltage
	u16 vol_max;		//the maximum level vs the maximum output voltage

}Product_TypeDef;



typedef struct{
	u16 logo;
	u16 language;
	u16 light;
	u16 alarm;//
	u16 used_time_h;
	u16 used_time_l;	
	u16 medium_flag;    //介质标记位  1:液氮 2干冰
	u16 prog_grp;
	
	u16 start;	// 0:stop 1:run:2:run-pause
	
	float k_factor;
	float b_factor;
	u16 product_id;
	u16 paras_sel;//current work parasmeters selected 
	u16 pre_page;//the last page,use for reback last work page
	
	/********************/
	u16 passcode;
	
	u16 state;
  u16 door_state;
	u16 total_time;
	u16 run_step;
	
	u32 used_time;     //记总时间 秒数
	u16 disp_time;     ///显示时间 总小时数
	u16 delay;
	
	u16 input_disp1;
	u16 input_disp2;
	u16 input_disp3;
	u16 input_disp4;
	u16 input_disp5;
	u16 set_cooling_time_bit;
	u16 set_grind_time_bit;
	short set_cooling_time;  		  //冷冻时间
	short set_volume; 			      //设置体积
	short set_grind_temp;         //设置温度
	short set_grind_speed;  		  //设置转速
	short set_grind_time;         //研磨时间

	short use_cooling_time;  		  //冷冻时间
	short use_volume; 			      //设置体积
	short use_grind_temp;         //设置温度
	short use_grind_speed;  		  //设置转速
	short use_grind_time;         //研磨时间
	
	short side_temp;
	short bottom_temp;
	short disp_temperature;    //测量温度
	short measure_weight;      //测量重量

  short add_liquid;  //加入的液氮量
	short save_weight;
		
	
	
}STR_WIN_TypeDef;


//START_WIN结构体
extern STR_WIN_TypeDef xdata win_str_data;

typedef enum
{
	MAIN_TIMER=1,
	SET_TIMER=2,
	PROG_TIMER=4,
}Timer_CH_Def;

void win_disp_cooling_time(u16 val);
void win_disp_volume(u16 val);
void win_disp_temp(u16 val);
void win_disp_speed(u16 val);
void win_disp_grind_time(u16 val);
void win_disp_real_temper(u16 val1,u16 val2);
void win_disp_weight(u16 val);
void win_disp_total_time(u16 val);
void win_disp_prog_num(u16 val);

void Fresh_Service_Time(void);
void win_init(void);
void win_change_page(u16 page);
u16 win_level_trans_volt(u16 level);
void win_fresh_window(void);
#endif

