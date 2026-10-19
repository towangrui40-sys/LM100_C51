#ifndef __START_WIN_H__
#define __START_WIN_H__
#include "sys.h"

#define PRODUCT_TOTAL 1      //��������
/*---------------------define arae-------------------------*/
/********************************************************
��ַOX2000��ͷ ��ʾ����
��ַOX2000��ͷ ��ʾ����
ϵͳ�����ӿڣ�0x0000-0x0FFF �����洢���ռ䣩
0x1000-0xFFFF �����洢�ռ��û���������ʹ�á�

**********************LOGO*****************************/
#define ANIMOTION_LOGO_ADDR		    0x2100  

//WORK PAGE VAR
/*******************��ҳ����ʾ*************************/
//0x2000~0x201F
#define ARTNUM_MAIN_COOLING_TIME_ADDR		    	  0x2000  //����ʱ��
#define ARTNUM_MAIN_COOLING_TIME_HADDR		    	0x2001  //����ʱ��
#define ARTNUM_MAIN_VOLUME_ADDR		    	        0x2002  //Һ����
#define ARTNUM_MAIN_TEMP_ADDR		    	          0x2003  //�����¶�
#define ARTNUM_MAIN_SPEED_ADDR		    	        0x2004  //��ĥ�ٶ�
#define ARTNUM_MAIN_GRINDING_TIME_ADDR		      0x2005  //��ĥʱ��
#define ARTNUM_MAIN_GRINDING_TIME_HADDR		    	0x2006  //��ĥʱ��
#define ARTNUM_MAIN_DISP_TEMP_ADDR		          0x2007  //��ʾ�¶�
#define ARTNUM_MAIN_WEIGHT_ADDR                 0x2008  //��ʾ������
#define ARTNUM_MAIN_PROG_NUM_ADDR		    	      0x2009  //�������

#define ARTNUM_WORK_ADD_LIQUID_ADDR		    0x2010
#define ARTNUM_WORK_TOTAL_TIME_ADDR		    0x2011  //��ʱ��
#define ARTNUM_WORK_TOTAL_TIME_HADDR		  0x2012

#define ARTNUM_FAC_BOTTOM_TEMP_ADDR		    0x2020
#define ARTNUM_FAC_SIDE_TEMP_ADDR		      0x2021


//����ͼ��

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

#define ARTNUM_PASSCODE_ADDR					0x2060  //��ʾ��������

#define ARTNUM_UT_ADDR								0x2072 //service used time	 ������ʱ��

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
	MENU_PAGE = 0, //�˵�����
	FAC_PAGE,      //LOGO����
	INFO_PAGE_LOGO,     //��Ϣ����
	INFO_PAGE,     //��Ϣ����
	SET_PAGE,      //���ý���
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
	short unit;	//para unit
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
	u16 medium_flag;    //���ʱ��λ  1:Һ�� 2�ɱ�
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
	
	u32 used_time;
	u16 disp_time;
	u16 delay;
	
	u16 input_disp1;
	u16 input_disp2;
	u16 input_disp3;
	u16 input_disp4;
	u16 input_disp5;
	u16 set_cooling_time_bit;
	u16 set_grind_time_bit;
	short set_cooling_time;
	short set_volume;
	short set_grind_temp;
	short set_grind_speed;
	short set_grind_time;

	short use_cooling_time;
	short use_volume;
	short use_grind_temp;
	short use_grind_speed;
	short use_grind_time;
	
	short side_temp;
	short bottom_temp;
	short disp_temperature;
	short measure_weight;

 	short add_liquid;
	short save_weight;
		
	
	
}STR_WIN_TypeDef;


//START_WIN�ṹ��
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

