#include "bluetooth.h"

#define PACK_HEAD 0x1A   
#define PACK_TAIL 0x3E


uint8_t bt_app_output_buf[50];
uint8_t bt_app_input_buf[50];

void bluetooth_app_prase(uint8_t byte)
{
	static uint8_t state=0,cnt=0,data_len=0;
	
	switch(state)
	{
		case 0: //֡ͷ
			if(byte==0xA5) 
			{
				state=10;
				bt_app_input_buf[0]=byte;
				data_len=5;
			}
			else
			{
				state=0,cnt=0,data_len=0;
			}
		break;
			
		case 10: //��Ч����λ
			data_len--;
			bt_app_input_buf[1+cnt++]=byte;
			if(data_len==0) state = 20;
		break;
		
		case 20: //֡β
			if(byte==PACK_TAIL)
			{
				bt_app_input_buf[1+cnt++]=byte;//֡β
				state=0,cnt=0,data_len=0;
				get_bluetooth_data();
			}
			else
			{
				state=0,cnt=0,data_len=0;
			}
		break;
			
	}	
}

char data_hex[9];
float hex_to_float(const char* hex_str) ;


void get_bluetooth_data(void)
{
	uint8_t ch=0;
	float tmp=0;
	snprintf(data_hex, sizeof(data_hex), "%02X%02X%02X%02X", 
             (unsigned char)bt_app_input_buf[5], (unsigned char)bt_app_input_buf[4], 
             (unsigned char)bt_app_input_buf[3], (unsigned char)bt_app_input_buf[2]);  //�ַ���ƴ��
	tmp=hex_to_float(data_hex);
	
	switch(bt_app_input_buf[1])
	{
		case 0x2A: //P����
			seektrack_ctrl[0].kp=tmp;
		break;
		
//		case 0x2B: //I����
//			seektrack_ctrl[0].kp=tmp;
//		break;
		
		case 0x2C: //D����
			seektrack_ctrl[0].kd=tmp;
		break;
		
		case 0x2D: //�ٶ�����
			speed_setup=tmp;
		break;

	}
}

// ʮ�������ַ���ת��Ϊ������
float hex_to_float(const char* hex_str) 
{
    uint32_t temp;
    sscanf(hex_str, "%X", &temp);
    float value;
    memcpy(&value, &temp, sizeof(value));
    return value;
}