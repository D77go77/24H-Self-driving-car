#ifndef __USER_H
#define __USER_H
/***********************************************************************************************************************/
#define left_motor_encoder_dir_default   0
#define right_motor_encoder_dir_default  0
#define left_motion_dir_default   0
#define right_motion_dir_default  0
#define tire_radius_cm_default  3.4f			 //��̥�뾶����λΪcm
#define pulse_cnt_per_circle_default  1060 //ʵ������ת��һ�ܵ���������
#define rangefinder_type_default 0

#define no_voltage_enable_default 0
#define no_voltage_upper_default 11.5f
#define no_voltage_lower_default 10.5f
/***********************************************************************************************************************/
#define speed_expect_default 			50.0f			//����Ѱ��ʱ���ٶ�Ĭ���趨ֵ
#define work_mode_default   			0 				//С������ģʽ����Ĭ��ֵ
/***********************************************************************************************************************/
#define turn_kp_default1						102.0f 	//���ں���Թ�ʶ��켣ʱ,����Ѱ����λ�ÿ�������������KP	
#define turn_ki_default1						0.0f		//���ں���Թ�ʶ��켣ʱ,����Ѱ����λ�ÿ��������ֲ���KI	
#define turn_kd_default1						220			//���ں���Թ�ʶ��켣ʱ,����Ѱ����λ�ÿ�����΢�ֲ���KD	
#define turn_scale_default        				0.15f		//ת����������ת����������������ʱ������ת��ϵ��
/***********************************************************************************************************************/
#define steer_gyro_kp_default  		1.5f			//ת�������ǿ���ʱ�����ٶ��ڻ�����ϵ��KP
#define steer_gyro_ki_default  		0.0f			//ת�������ǿ���ʱ�����ٶ��ڻ�����ϵ��KI
#define steer_gyro_kd_default  		3.0f			//ת�������ǿ���ʱ�����ٶ��ڻ�΢��ϵ��KD	
#define	steer_gyro_scale_default	0.5f			//ת�������ǿ���ʱ�����ٶ��ڻ����������ת����������������ʱ������ת��ϵ��
/***********************************************************************************************************************/
#define speed_kp_default  				8.0f			//�ٶȿ�������������KP
#define	speed_ki_default				0.6f			//�ٶȿ��������ֲ���KI
#define	speed_kd_default				0.0f			//�ٶȿ�����΢�ֲ���KD

#endif




