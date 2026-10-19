#ifndef __MOTOR_CMD_H_
#define __MOTOR_CMD_H_



typedef enum
{
  MC_FWD = 0X01,
	MC_REV = 0X02,
	MC_STOP = 0X04,
}MC_Cmd;

//void motor_set_station(u8 station);
void motor_set_run(u16 paras);
//void motor_set_rpm(u16 rpm);
//void motor_set_door(u16 enable);

//void motor_get_runstate(MC_GetState state);
//void motor_get_rpm(void);
//void motor_get_door_lock(void);
//void motor_get_roller(void);


#endif


