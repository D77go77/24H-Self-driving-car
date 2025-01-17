#include "headfile.h"
#include "sdk.h"
#include "subtask.h"
#include "user.h"
#include "developer_mode.h"

position pos;

int16_t sdk_work_mode=0; 

void sdk_duty_run(void)
{
	if(trackless_output.init==0)
	{		
		trackless_output.yaw_ctrl_mode=ROTATE;
		trackless_output.yaw_outer_control_output=0;
		trackless_output.init=1;
		flight_subtask_reset();//��λsdk������״̬��
	}
	
	switch(sdk_work_mode)
	{	
		case 0://����1-2����ȷ��
		{	
			smartcar_imu.state_estimation.pos.y=smartcar_imu.state_estimation.pos.x=0;
		}
		break;
		case 1://����1
		{
			auto_line_go();//ֱ��
		}
		break;
		//
		case 2://����2
		{
			auto_line_go_2();//�ڶ���Բ��
		}
		break;
		//
		case 3://����3-4����ȷ��
				smartcar_imu.state_estimation.pos.y=smartcar_imu.state_estimation.pos.x=0;
		break;
		//
		case 4://����3
			auto_line_cha_1();
		break;
		//
		case 5://����4
			auto_line_cha_2();
		break;
		//
		case 6:
				
				speed_ctrl_mode=1;//�ٶȿ��Ʒ�ʽΪ���ֵ�������
				position_control();
				turn_ctrl_pwm=steer_gyro_output;
				speed_setup=distance_ctrl.output;
				//�����ٶ�
				speed_expect[0]=speed_setup-turn_ctrl_pwm*steer_gyro_scale;//��������ٶ�����
				speed_expect[1]=speed_setup+turn_ctrl_pwm*steer_gyro_scale;//�ұ������ٶ�����
				//�ٶȿ���
				speed_control_100hz(speed_ctrl_mode);
		break;
		//
		default:
			trackless_output.unlock_flag=LOCK;//����������
			flight_subtask_reset();	//��λsdk������״̬��
			smartcar_imu.state_estimation.pos.y=smartcar_imu.state_estimation.pos.x=0;
		break;
	}
}

