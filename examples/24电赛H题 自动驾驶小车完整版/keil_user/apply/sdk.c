#include "headfile.h"
#include "sdk.h"
#include "subtask.h"
#include "user.h"


controller_output trackless_output={
	.unlock_flag=LOCK,
};

nav_ctrl ngs_nav_ctrl={
	.ctrl_finish_flag=1,
	.update_flag=0,
	.cnt=0,
	.dis_cm=0,
}; 
//����Ƕȡ����ٶȿ�����
float	steer_angle_expect=0,steer_angle_error=0,steer_angle_output=0;
float	steer_gyro_expect=0,steer_gyro_output=0;
float	steer_gyro_scale=steer_gyro_scale_default;//0.5f

controller steergyro_ctrl;		//ת����ٶ��ڻ��������ṹ��//���ٶȿ��Ʋ���   1.2  0  3							 
controller distance_ctrl,azimuth_ctrl;//���ֲ���λ�á��������

void e2prom_init(void)
{
	//Ԥ������
	float tmp_reserved_params[20];
	for(uint16_t i=0;i<8;i++)
	{
		ReadFlashParameterOne(RESERVED_PARAMS_1+i,&tmp_reserved_params[i]);
	}
	if(isnan(tmp_reserved_params[0])==0) 	pos.b_x=tmp_reserved_params[0];	else pos.b_x=200;
	if(isnan(tmp_reserved_params[1])==0) 	pos.b_y=tmp_reserved_params[1];	else pos.b_y=-2;
	if(isnan(tmp_reserved_params[2])==0) 	pos.d_x=tmp_reserved_params[2];	else pos.d_x=-40;
	if(isnan(tmp_reserved_params[3])==0) 	pos.d_y=tmp_reserved_params[3];	else pos.d_y=-85;
	if(isnan(tmp_reserved_params[4])==0) 	pos.ac_x=tmp_reserved_params[4];else pos.ac_x=207;
	if(isnan(tmp_reserved_params[5])==0) 	pos.ac_y=tmp_reserved_params[5];else pos.ac_y=-142;
	if(isnan(tmp_reserved_params[6])==0) 	pos.bd_x=tmp_reserved_params[6];else pos.bd_x=-50;
	if(isnan(tmp_reserved_params[7])==0) 	pos.bd_y=tmp_reserved_params[7];else pos.bd_y=-136;
}
/***************************************************
������: void ctrl_params_init(void)
˵��:	pid������������ʼ��
****************************************************/
void ctrl_params_init(void)
{
	pid_control_init(&seektrack_ctrl[0],//����ʼ���������ṹ��
										turn_kp_default1,//��������
										turn_ki_default1,//���ֲ���
										turn_kd_default1,//΢�ֲ���
										20, //ƫ���޷�ֵ
										0,  //�����޷�ֵ
										500,//����������޷�ֵ
										1,	//ƫ���޷���־λ
										0,0,//���ַ����־λ��������ֿ���ʱ���޷�ֵ
										6); //΢�ּ��ʱ��


	pid_control_init(&azimuth_ctrl,//����ʼ���������ṹ��
										5.0f,//��������
										0,	 //���ֲ���
										0,	 //΢�ֲ���
										50,  //ƫ���޷�ֵ
										0,   //�����޷�ֵ
										500, //����������޷�ֵ
										1,   //ƫ���޷���־λ
										0,0, //���ַ����־λ��������ֿ���ʱ���޷�ֵ
										1);  //΢�ּ��ʱ��

	pid_control_init(&steergyro_ctrl,//����ʼ���������ṹ��
										steer_gyro_kp_default,//��������
										steer_gyro_ki_default,//���ֲ���
										steer_gyro_kd_default,//΢�ֲ���
										200, //ƫ���޷�ֵ
										300, //�����޷�ֵ
										500, //����������޷�ֵ
										1,  	//ƫ���޷���־λ
										0,0, //���ַ����־λ��������ֿ���ʱ���޷�ֵ
										1);  //΢�ּ��ʱ��
	
	pid_control_init(&distance_ctrl,//����ʼ���������ṹ��
										0.5,//��������
										0,//���ֲ���
										0,//΢�ֲ���
										75, //ƫ���޷�ֵ
										0, //�����޷�ֵ
										500, //����������޷�ֵ
										1,  	//ƫ���޷���־λ
										0,0, //���ַ����־λ��������ֿ���ʱ���޷�ֵ
										1);  //΢�ּ��ʱ��
}

vector2f target={0,0};
/***************************************************
������: void get_distance_error(vector2f t)
˵��:	��ȡĿ��λ�ú͵�ǰλ�õ�ƫ��
���:	vector2f t-Ŀ��λ��
****************************************************/
float get_distance_error(vector2f t)
{
	return sqrtf(sq(t.x-smartcar_imu.state_estimation.pos.x)+sq(t.y-smartcar_imu.state_estimation.pos.y));
}
/***************************************************
������: void position_control(void)
˵��:	��άĿ��λ�ÿ���
��ע:	
		y+(��ʼ��ͷ���)
		*
		*
		*******x+(��ʼ��ͷ����)
****************************************************/
void position_control(void)//��������̬����Ŀ���,���ٶ�ĩ�˽Ƕ�Լ��
{
	if(ngs_nav_ctrl.update_flag==1)
	{
		target.x=ngs_nav_ctrl.x;
		target.y=ngs_nav_ctrl.y;
		ngs_nav_ctrl.update_flag=0;
		ngs_nav_ctrl.ctrl_finish_flag=0;
		ngs_nav_ctrl.cnt=0;	
	}
	//�������
	float theta=atan2f((target.y-smartcar_imu.state_estimation.pos.y),(target.x-smartcar_imu.state_estimation.pos.x));//Ŀ�꺽��
	//�Ƕȿ���
	azimuth_ctrl.expect=theta*RAD2DEG;
	azimuth_ctrl.feedback=smartcar_imu.rpy_deg[_YAW];
	azimuth_ctrl.error=azimuth_ctrl.expect-azimuth_ctrl.feedback;
	/***********************ƫ����ƫ���+-180����*****************************/
	if(azimuth_ctrl.error<-180) azimuth_ctrl.error=azimuth_ctrl.error+360;
	if(azimuth_ctrl.error> 180) azimuth_ctrl.error=azimuth_ctrl.error-360;				
	azimuth_ctrl.error=constrain_float(azimuth_ctrl.error,-30,30);
	azimuth_ctrl.output=azimuth_ctrl.kp*azimuth_ctrl.error;
	
	//��������������ٶȣ�������ױ�Ħ����Сʱ���´�ֵ���ܼ�С��̼Ƶľ������
	steer_angle_output=constrain_float(azimuth_ctrl.output,-120,120);
	
	steergyro_ctrl.expect=steer_angle_output;//����
	steergyro_ctrl.feedback=smartcar_imu.yaw_gyro_enu;//ƫ���Ƕȷ���
	pid_control_run(&steergyro_ctrl);		  //����������
	steer_gyro_output=steergyro_ctrl.output;

	//�������
	distance_ctrl.expect=0;
	distance_ctrl.feedback=-get_distance_error(target);
	distance_ctrl.error=distance_ctrl.expect-distance_ctrl.feedback;
	distance_ctrl.error=constrain_float(distance_ctrl.error,-180,180);
	distance_ctrl.output=distance_ctrl.kp*distance_ctrl.error;
	distance_ctrl.output=constrain_float(distance_ctrl.output,-90,90);//���ٶ������������ƣ�������̥�������̼����
	
	//�������������־�ж�
	if(ngs_nav_ctrl.cnt<10)//����50ms����
	{
		if(ABS(distance_ctrl.error)<=3.0f) ngs_nav_ctrl.cnt++;
		else ngs_nav_ctrl.cnt/=2;
	}
	else
	{
		ngs_nav_ctrl.ctrl_finish_flag=1;
		ngs_nav_ctrl.cnt=0;
	}
	
	if(ngs_nav_ctrl.ctrl_finish_flag==1)//����Ŀ��λ�ú��ٶ�������0
	{
		steer_gyro_output=0;
		distance_ctrl.output=0;
	}
}
/***************************************************
������: void distance_control(void)
˵��:	�������
****************************************************/
void distance_control(void)
{
	//�������
	distance_ctrl.feedback=smartcar_imu.state_estimation.distance;
	distance_ctrl.error=distance_ctrl.expect-distance_ctrl.feedback;
	distance_ctrl.error=constrain_float(distance_ctrl.error,-50,50);
	distance_ctrl.output=3.0f*distance_ctrl.error;//
	distance_ctrl.output=constrain_float(distance_ctrl.output,-50,50);//���ٶ������������ƣ�������̥�������̼����
}


