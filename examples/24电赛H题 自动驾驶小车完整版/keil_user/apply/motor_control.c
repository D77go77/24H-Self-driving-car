#include "headfile.h"
#include "motor_control.h"
#include "user.h"

float left_pwm,right_pwm;//���ҵ�����յ����ֵ

motor_config trackless_motor={
	.left_encoder_dir_config=left_motor_encoder_dir_default,	//��������������
	.right_encoder_dir_config=right_motor_encoder_dir_default,	//��������������
	.left_motion_dir_config=left_motion_dir_default,			//����˶���������
	.right_motion_dir_config=right_motion_dir_default,			//����˶���������
	.wheel_radius_cm=tire_radius_cm_default,					//�����ֵİ뾶
	.pulse_num_per_circle=pulse_cnt_per_circle_default,			//������ת��һȦʱ�������������ۼ�ֵ
};

uint8_t speed_ctrl_mode=1;//0:ֱ�ӿ��ơ�1:���ֵ�������
uint8_t speed_pid_mode=0; //0:λ��ʽPID����
float motion_ctrl_pwm=0,turn_ctrl_pwm=0;//�˶���ת��������
int16_t motion_test_pwm_default=500;

#define speed_err_max  50.0f				//�ٶ�ƫ���޷�ֵ
#define speed_integral_max  600.0f	//�����޷�ֵ
#define speed_ctrl_output_max 999		//�������޷�ֵ

//�ٶȿ���������
float speed_setup=0;//�ٶ��趨ֵ
float speed_kp=speed_kp_default,speed_ki=speed_ki_default,speed_kd=speed_kd_default;
float speed_error[2]={0,0},speed_expect[2]={speed_expect_default,speed_expect_default},speed_feedback[2]={0,0};
float	speed_integral[2]={0,0},speed_output[2]={0,0};

#define speed_smooth_period   (20/5)//20ms
uint16_t speed_smooth_output_cnt=0;
float speed_smooth_output=0;
//
float speed_output_delta[2]={0,0},speed_output_last[2]={0,0},speed_error_last[2]={0,0};

/***************************************************
������: void speed_control_100hz(uint8_t _speed_ctrl_mode)
˵��:	�ٶȿ���
���:	uint8_t _speed_ctrl_mode-�ٶȿ��Ʒ�ʽ,1:��ͨ�ٶȿ���;2:��ƽ���ٶȿ���
****************************************************/
void speed_control_100hz(uint8_t _speed_ctrl_mode)
{
	static uint16_t _cnt=0;
	switch(_speed_ctrl_mode)
	{
		case 1:
		{
			_cnt++;	if(_cnt<2)	return;	_cnt=0;//10ms����һ��
			if(speed_pid_mode==0)
			{	
					speed_feedback[0]=smartcar_imu.left_motor_speed_cmps;
					speed_error[0]=speed_expect[0]-speed_feedback[0];
					speed_error[0]=constrain_float(speed_error[0],-speed_err_max,speed_err_max);
					speed_integral[0]+=speed_ki*speed_error[0];
					speed_integral[0]=constrain_float(speed_integral[0],-speed_integral_max,speed_integral_max);
					speed_output[0]=speed_integral[0]+speed_kp*speed_error[0];
					speed_output[0]=constrain_float(speed_output[0],-speed_ctrl_output_max,speed_ctrl_output_max);
						
					speed_feedback[1]=smartcar_imu.right_motor_speed_cmps;
					speed_error[1]=speed_expect[1]-speed_feedback[1];
					speed_error[1]=constrain_float(speed_error[1],-speed_err_max,speed_err_max);
					speed_integral[1]+=speed_ki*speed_error[1];
					speed_integral[1]=constrain_float(speed_integral[1],-speed_integral_max,speed_integral_max);
					speed_output[1]=speed_integral[1]+speed_kp*speed_error[1];
					speed_output[1]=constrain_float(speed_output[1],-speed_ctrl_output_max,speed_ctrl_output_max);
			}	
		}
		break;	

	}		
}

//#define right_pwm_channel_1		GPIO_PWM_0_C0_IDX//TIMA0-CH0-PB14
//#define right_pwm_channel_2		GPIO_PWM_0_C1_IDX//TIMA0-CH1-PA3
//#define left_pwm_channel_1    GPIO_PWM_0_C2_IDX//TIMA0-CH2-PA7
//#define left_pwm_channel_2    GPIO_PWM_0_C3_IDX//TIMA0-CH3-PA4
#define right_pwm_channel_1		GPIO_PWM_0_C3_IDX//TIMA0-CH3-PA4
#define right_pwm_channel_2		GPIO_PWM_0_C2_IDX//TIMA0-CH2-PA7
#define left_pwm_channel_1    GPIO_PWM_0_C1_IDX//TIMA0-CH1-PA3
#define left_pwm_channel_2    GPIO_PWM_0_C0_IDX//TIMA0-CH0-PB14

void car_break(void);
/***************************************************
������: void motor_output(uint8_t _speed_ctrl_mode)
˵��:	����PWM�������
���:	uint8_t _speed_ctrl_mode-�ٶȿ��Ʒ�ʽ,0:����ģʽ;1:��ͨ�ٶȿ���;2:��ƽ���ٶȿ���
****************************************************/
void motor_output(uint8_t _speed_ctrl_mode)
{
	if(_speed_ctrl_mode==0)//���ڳ�ʼ���Ե���˶�����ʱʹ��
	{
		left_pwm =motion_ctrl_pwm;
		right_pwm=motion_ctrl_pwm;
	}
	else if(_speed_ctrl_mode==1)//�����ٶȵ�������
	{
		right_pwm=speed_output[1];
		left_pwm =speed_output[0];
	}
	else
	{
		right_pwm=0;
		left_pwm =0;	
	}
	
	if(trackless_output.unlock_flag==LOCK)
	{
		left_pwm =0;
		right_pwm=0;	
		car_break();
	}
	
	if(trackless_output.unlock_flag==LOCK)return;
	
	left_pwm =constrain_float(left_pwm,-999,999);
	right_pwm=constrain_float(right_pwm,-999,999);
	
	
	//�ұߵ�����
	if(trackless_motor.right_motion_dir_config==0)
	{
		if(right_pwm>=0)
		{
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0,         right_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, right_pwm, right_pwm_channel_2);
		}
		else
		{
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, ABS(right_pwm), right_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, 			 right_pwm_channel_2);	
		}
	}
	else
	{
		if(right_pwm>=0)
		{
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, right_pwm, right_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0,         right_pwm_channel_2);
		}
		else
		{
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, 			 right_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, ABS(right_pwm), right_pwm_channel_2);
		}
	}
	
	
	//��ߵ�����
	if(trackless_motor.left_motion_dir_config==0)
	{	
		if(left_pwm>=0)
		{
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, left_pwm, left_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0,        left_pwm_channel_2);				
		}
		else
		{			
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0,             left_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, ABS(left_pwm), left_pwm_channel_2);		
		}
	}
	else
	{
		if(left_pwm>=0)
		{
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0,        left_pwm_channel_1);	
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, left_pwm, left_pwm_channel_2);	
		}
		else
		{		
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, ABS(left_pwm), left_pwm_channel_1);
			DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0,             left_pwm_channel_2);		
		}	
	}
}

void car_break(void)
{
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, 1000, left_pwm_channel_1);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, 1000, left_pwm_channel_2);		
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, 1000, right_pwm_channel_1);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, 1000, right_pwm_channel_2);
}



