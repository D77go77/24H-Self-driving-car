/****************************************************************************************
	�������MPWM	
		PA4-A0-PWM-CH3	  �ұߵ������INA1
		PA7-->A0-PWM-CH2	�ұߵ������INA2
		PA3-->A0-PWM-CH1	��ߵ������INB1
		PB14-->A0-PWM-CH0	��ߵ������INB2		
	����������ENC	
		PB4-RIGHT-PULSE	  �ұߵ�����屶Ƶ���P1
		PB5-->LEFT-PULSE	��ߵ�����屶Ƶ���P2
		PB6-->RIGHT-DIR	  �ұߵ������������D1
		PB7-->LEFT-DIR	  ��ߵ������������D2
	����IMU�ӿ�IMU	
		PA29-I2C-SCL	MPU6050-SCL
		PA30-->I2C-SDA	MPU6050-SDA
	��ص�ѹ�ɼ�	
		PA26-ADC-VBAT	��Ҫ�ⲿ��ѹ����������
****************************************************************************************/
#include "ti_msp_dl_config.h"
#include "headfile.h"

void maple_duty_200hz(void);
void duty_1000hz(void);

int main(void)
{
	usart_irq_config();             //�����ж�����
	SYSCFG_DL_init();	         	//ϵͳ��Դ���ó�ʼ��	
	OLED_Init();					//��ʾ����ʼ��
	nADC_Init();					//ADC��ʼ��
	nGPIO_Init();					//��������ʼ��
	w25qxx_gpio_init();     		//����w25q64��ʼ��
	e2prom_init();					//e2prom��ʼ��
	ctrl_params_init();				//���Ʋ�����ʼ��
	ICM206xx_Init();				//���ٶȼ�/�����ǳ�ʼ��	
	Encoder_Init();					//��������Դ��ʼ��
	Button_Init();					//���ذ�����ʼ��
	timer_irq_config();
	while(1)
	{
		maple_duty_200hz();
	}
}
/***************************************
������:	void duty_200hz(void)
˵��: 200hz������������
***************************************/
void maple_duty_200hz(void)
{
	if(!f_5ms)return;
	f_5ms=0;
	if(!low_voltage_flag)
		screen_display();//��Ļ��ʾ
	
	laser_light_work(&beep);       				//��Դ�����������
	bling_working(0);							//RGB��״̬��
	read_button_state_all();       				//����״̬��ȡ
	
	// static u16 cnt_time_put=0;
	// if(++cnt_time_put>=4)
	// {
	// 	cnt_time_put=0;
	// 	printf("%.1f,%.1f,%.1f \n",seektrack_ctrl[0].kp,seektrack_ctrl[0].kd,speed_setup);
	// }
	
	static u16 cnt_100ms=0;
	if(++cnt_100ms>=20)
	{
		cnt_100ms=0;	
		adc_statemachine();							//adc�ɼ�״̬��
		battery_voltage_detection();	 			//��ص�ѹ���
		get_battery_voltage();				 		//ADC���ݻ�ȡ
	}
}
/***************************************
������:	void duty_200hz(void)
˵��: 200hzʵʱ������
***************************************/
void duty_200hz(void)
{
	sdk_duty_run();					  			//SDK���������
	get_wheel_speed();					   		//��ȡ��̥ת��
	motor_output(speed_ctrl_mode); 				//���������
	imu_data_sampling();					 	//���ٶȼơ����������ݲɼ�
	trackless_ahrs_update();			 		//ahrs��̬����
}

void duty_1000hz(void)
{

		gpio_input_check_channel_12();//���12·�ҶȻҶȹ�״̬

}
