#include "ti_msp_dl_config.h"
#include "datatype.h"
#include "system.h"
#include "wp_math.h"
#include "ni2c.h"
#include "sensor.h"
#include "neeprom.h"
#include "ngpio.h"
#include "icm20608.h"

IMU_ID_READ IMU_ID=WHO_AM_I_MPU6050;
static uint8_t imu_address=MPU6050_ADRESS;


/***************************************
������:	void ICM206xx_Init(void)
˵��: ���ٶȼ�/�����ǳ�ʼ��
����:	uint8_t ����ʧ�ܱ�־
***************************************/
uint8_t ICM206xx_Init(void)//ICM20689��ʼ��
{
	uint8_t fault=0;
	single_writei2c(imu_address,PWR_MGMT_1, 0x81);//���ǿ�Ƹ�λ81
	delay_ms(100);	
	IMU_ID=(IMU_ID_READ)(single_readi2c(imu_address,WHO_AM_I));
	if(IMU_ID==0xFF)
	{
		imu_address=MPU6050_ADRESS+1;
		single_writei2c(imu_address,PWR_MGMT_1, 0x81);//���ǿ�Ƹ�λ81
		delay_ms(100);	
		IMU_ID=(IMU_ID_READ)(single_readi2c(imu_address,WHO_AM_I));
	}
	switch(IMU_ID)
	{
		case WHO_AM_I_MPU6050:
		{
			single_writei2c(imu_address,PWR_MGMT_1  , 0x80);//�����λ
			delay_ms(200);
			single_writei2c(imu_address,SMPLRT_DIV  , 0x00); // sample rate.  Fsample= 1Khz/(<this value>+1) = 1000Hz
			single_writei2c(imu_address,PWR_MGMT_1  , 0x03); //ʱ��Դ PLL with Z Gyro reference
			single_writei2c(imu_address,MPU_CONFIG  , 0x02); //�ڲ���ͨ�˲�Ƶ�ʣ����ٶȼ�184hz��������188hz  //Ĭ��0x03	  
			single_writei2c(imu_address,GYRO_CONFIG , 0x08); //500deg/s
			single_writei2c(imu_address,ACCEL_CONFIG, 0x08); // Accel scale 4g (8192 LSB/g)			
		}
		break;
		default:
		{
			fault=1;
		}			
	}
  	delay_ms(500);
	
	imu_calibration_params_init();	
	return fault;
}

/***************************************
������:	void ICM206xx_Read_Data(vector3f *gyro,vector3f *accel,float *temperature)
˵��: ��ȡ���������ٶȼ�/������/�¶�����
���:	vector3f *gyro-��ȡ��������������ָ��
			vector3f *accelgyro-��ȡ������ٶ�����ָ��
			float *temperaturegyro-��ȡ�¶�����ָ��
��ע:	�����ǵ�λdeg/s,���ٶȼƵ�λg,�¶ȵ�λ��
***************************************/
void ICM206xx_Read_Data(vector3f *gyro,vector3f *accel,float *temperature)
{
	uint8_t buf[14];
	int16_t temp;
	i2creadnbyte(imu_address,ACCEL_XOUT_H,buf,14);
	switch(IMU_ID)
	{
		case WHO_AM_I_MPU6050:
		{
			accel->x=-(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z= (int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=-(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	= (int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=36.53f+(float)(temp/340.0f);	
		}
		break;
	}
	gyro->x*=GYRO_CALIBRATION_COFF;
	gyro->y*=GYRO_CALIBRATION_COFF;
	gyro->z*=GYRO_CALIBRATION_COFF;
	
	accel->x/=GRAVITY_RAW;
	accel->y/=GRAVITY_RAW;
	accel->z/=GRAVITY_RAW;
}
/***************************************
������:	void imu_calibration_params_init(void)
˵��: ���ٶȼ�/�����Ǳ궨���ݳ�ʼ��
***************************************/
void imu_calibration_params_init(void)
{
	vector3f gyro_offset_temp;
	
	smartcar_imu.gyro_offset.x=0;
	smartcar_imu.gyro_offset.y=0;
	smartcar_imu.gyro_offset.z=0;
	
	smartcar_imu.imu_cal_flag=0;
	
	//У׼��������ƫ
	smartcar_imu.accel_scale.x=1.0f;
	smartcar_imu.accel_scale.y=1.0f;
	smartcar_imu.accel_scale.z=1.0f;

	smartcar_imu.accel_offset.x=0;
	smartcar_imu.accel_offset.y=0;
	smartcar_imu.accel_offset.z=0;	
} 

#define gyro_delta_dps  3.0f
/***************************************
������:	void imu_calibration(vector3f *gyro,vector3f *accel)
˵��: ���ٶȼ�/�����Ǳ궨
***************************************/
void imu_calibration(vector3f *gyro,vector3f *accel)
{
	if(smartcar_imu.imu_cal_flag==1)  return;
	
	static uint16_t cnt=0;
	static vector3f last_gyro;
	static vector3f accel_sum,gyro_sum;
	
	if(ABS(gyro->x-last_gyro.x)<=gyro_delta_dps
	 &&ABS(gyro->y-last_gyro.y)<=gyro_delta_dps
	 &&ABS(gyro->z-last_gyro.z)<=gyro_delta_dps
	 )
	{
		gyro_sum.x+=gyro->x;
		gyro_sum.y+=gyro->y;
		gyro_sum.z+=gyro->z;
		accel_sum.x+=accel->x;
		accel_sum.y+=accel->y;
		accel_sum.z+=accel->z;
		cnt++;
	}
  else
	{
		gyro_sum.x=0;
		gyro_sum.y=0;
		gyro_sum.z=0;
		accel_sum.x=0;
		accel_sum.y=0;
		accel_sum.z=0;
		cnt=0;
	}
	last_gyro.x=gyro->x;
	last_gyro.y=gyro->y;
	last_gyro.z=gyro->z;

	if(cnt>=400)//��������2s
	{
		smartcar_imu.gyro_offset.x =(gyro_sum.x/cnt);//�õ��궨ƫ��
		smartcar_imu.gyro_offset.y =(gyro_sum.y/cnt);
		smartcar_imu.gyro_offset.z =(gyro_sum.z/cnt);
		
		smartcar_imu.accel_offset.x =(accel_sum.x/cnt);//�õ��궨ƫ��
		smartcar_imu.accel_offset.y =(accel_sum.y/cnt);
		smartcar_imu.accel_offset.z =(accel_sum.z/cnt)-safe_sqrt(1-sq2(smartcar_imu.accel_offset.x)-sq2(smartcar_imu.accel_offset.y));
			
		gyro_sum.x=0;
		gyro_sum.y=0;
		gyro_sum.z=0;
		accel_sum.x=0;
		accel_sum.y=0;
		accel_sum.z=0;		
		cnt=0;
		
		smartcar_imu.imu_cal_flag=1;
	}
}

