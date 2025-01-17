#ifndef __SDK_H
#define __SDK_H


typedef enum
{
	ROTATE=0,							//�ֶ�ƫ������ģʽ
  AZIMUTH=1,						//����ƫ���Ƕȿ���ģʽ
	CLOCKWISE=2,					//���ƫ���Ƕ�˳ʱ�����ģʽ	
	ANTI_CLOCKWISE=3,			//���ƫ���Ƕ���ʱ�����ģʽ	
	CLOCKWISE_TURN=4,			//���ٶȿ���˳ʱ��ģʽ
	ANTI_CLOCKWISE_TURN=5,//���ٶȿ�����ʱ��ģʽ
}YAW_CTRL_MODE;

typedef enum
{
	MOTOR1=0,
	MOTOR2,
	MOTOR3,
	MOTOR4,
	MOTOR5,
	MOTOR6,
	MOTOR7,
	MOTOR8,
	MOTOR_NUM
}motor;


typedef struct
{
	float roll_outer_control_output;  	//�����̬����������
	float pitch_outer_control_output; 	//������̬����������
	float yaw_outer_control_output;	  	//ƫ����̬����������

	uint16_t yaw_ctrl_cnt;							//ƫ�����Ƽ�����
	YAW_CTRL_MODE yaw_ctrl_mode;					//ƫ������ģʽ
	uint8_t yaw_ctrl_start;							//ƫ�����ƿ�ʼ��־λ
	uint8_t yaw_ctrl_end;							//ƫ�����ƽ�����־λ
	uint32_t start_time_ms;							//ƫ�����ƿ�ʼʱ��
	uint32_t execution_time_ms;						//ƫ������ִ��ʱ��
	uint8_t init;									//ƫ�����Ƴ�ʼ����־λ

	uint8_t roll_pitch_angle_limit_enable;
	LOCK_STATE unlock_flag;
}controller_output;

void ctrl_params_init(void);

void position_control(void);
void distance_control(void);	

extern float steer_gyro_scale;
extern float	steer_gyro_expect,steer_gyro_output;

extern controller steerangle_ctrl,steergyro_ctrl;
extern controller distance_ctrl,azimuth_ctrl;
extern controller_output trackless_output;

extern nav_ctrl ngs_nav_ctrl;

void e2prom_init(void);
#endif


