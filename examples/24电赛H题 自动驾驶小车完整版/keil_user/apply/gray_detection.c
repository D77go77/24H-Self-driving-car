#include "headfile.h"
#include "gray_detection.h"
#include "user.h"

typedef struct
{
	uint16_t state;
  bool gray_bit[16];
}_gray_state; 
_gray_state gray_state;
float gray_status[2]={0},gray_status_backup[2][20]={0};//�Ҷȴ�����״̬����ʷֵ
float turn_output=0,turn_output_last=0;//���������ֵ
float	turn_scale=turn_scale_default;//ת����Ʋ���ϵ��  0.15
uint32_t gray_status_worse=0;	//�Ҷȹ��쳣״̬������
controller seektrack_ctrl[2];		//����Ѱ���������ṹ��

#define read_gray_bit1   ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT0_PIN ) ? 0x01 : 0x00)
#define read_gray_bit2   ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT1_PIN ) ? 0x01 : 0x00)
#define read_gray_bit3   ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT2_PIN ) ? 0x01 : 0x00)
#define read_gray_bit4   ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT3_PIN ) ? 0x01 : 0x00)
#define read_gray_bit5   ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT4_PIN ) ? 0x01 : 0x00)
#define read_gray_bit6   ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT5_PIN ) ? 0x01 : 0x00)
#define read_gray_bit7   ((PORTB_PORT->DIN31_0 & PORTB_GRAY_BIT6_PIN ) ? 0x01 : 0x00)
#define read_gray_bit8   ((PORTB_PORT->DIN31_0 & PORTB_GRAY_BIT7_PIN ) ? 0x01 : 0x00)
#define read_gray_bit9   ((PORTB_PORT->DIN31_0 & PORTB_GRAY_BIT8_PIN ) ? 0x01 : 0x00)
#define read_gray_bit10  ((PORTB_PORT->DIN31_0 & PORTB_GRAY_BIT9_PIN ) ? 0x01 : 0x00)
#define read_gray_bit11  ((PORTA_PORT->DIN31_0 & PORTA_GRAY_BIT10_PIN) ? 0x01 : 0x00)
#define read_gray_bit12  ((PORTB_PORT->DIN31_0 & PORTB_GRAY_BIT11_PIN) ? 0x01 : 0x00)

u8 gpio_gray_num=0;
uint8_t startpoint_check_flag=0;
float startpoint_straightaway_cm=0;
/***************************************************
������: void gpio_input_check_channel_12(void)
˵��:	12·�Ҷȹ�gpio���
****************************************************/
void gpio_input_check_channel_12(void)
{
	gray_state.gray_bit[0]=read_gray_bit1;
	gray_state.gray_bit[1]=read_gray_bit2;
	gray_state.gray_bit[2]=read_gray_bit3;
	gray_state.gray_bit[3]=read_gray_bit4;

	gray_state.gray_bit[4]=read_gray_bit5;
	gray_state.gray_bit[5]=read_gray_bit6;
	gray_state.gray_bit[6]=read_gray_bit7;
	gray_state.gray_bit[7]=read_gray_bit8;
	
	gray_state.gray_bit[8] =read_gray_bit9;
	gray_state.gray_bit[9] =read_gray_bit10;
	gray_state.gray_bit[10]=read_gray_bit11;
	gray_state.gray_bit[11]=read_gray_bit12;
	gray_state.state=0x0000;	
	gpio_gray_num=0;
	for(uint16_t i=0;i<=11;i++)
	{
		gray_state.state|=gray_state.gray_bit[i]<<i;
		gpio_gray_num+=gray_state.gray_bit[i];
		
	}	
	switch(round_grey)//�뻷���
	{
		case 0:
			break;
		case 1:
			gray_state.state&=0x0fc0;
			break;
		case 2:
			gray_state.state&=0x003f;
			break;
	}
	gray_status_backup[0][0]=gray_status[0];
	switch(gray_state.state)
	{
		case 0x0001:gray_status[0]=-11; gray_status_worse/=2;break;									//000000000001b
		case 0x0003:gray_status[0]=-10; gray_status_worse/=2;break;									//000000000011b
		case 0x0002:gray_status[0]=-9;	gray_status_worse/=2;break;									//000000000010b
		case 0x0006:gray_status[0]=-8;	gray_status_worse/=2;break;									//000000000110b
		case 0x0004:gray_status[0]=-7;	gray_status_worse/=2;break;									//000000000100b
		case 0x000C:gray_status[0]=-6;	gray_status_worse/=2;break;									//000000001100b
		case 0x0008:gray_status[0]=-5;	gray_status_worse/=2;break;									//000000001000b
		case 0x0018:gray_status[0]=-4;	gray_status_worse/=2;break;									//000000011000b
		case 0x0010:gray_status[0]=-3;	gray_status_worse/=2;break;									//000000010000b
		case 0x0030:gray_status[0]=-2;	gray_status_worse/=2;break;									//000000110000b
		case 0x0020:gray_status[0]=-1;	gray_status_worse/=2;break;									//000000100000b
		case 0x0060:gray_status[0]=0;	gray_status_worse/=2;break;									//000001100000b
		case 0x0040:gray_status[0]=1;	gray_status_worse/=2;break;									//000001000000b
		case 0x00C0:gray_status[0]=2;	gray_status_worse/=2;break;									//000011000000b
		case 0x0080:gray_status[0]=3;	gray_status_worse/=2;break;									//000010000000b
		case 0x0180:gray_status[0]=4;	gray_status_worse/=2;break;									//000110000000b
		case 0x0100:gray_status[0]=5;	gray_status_worse/=2;break;									//000100000000b
		case 0x0300:gray_status[0]=6;	gray_status_worse/=2;break;									//001100000000b
		case 0x0200:gray_status[0]=7;	gray_status_worse/=2;break;									//001000000000b
		case 0x0600:gray_status[0]=8;	gray_status_worse/=2;break;									//011000000000b
		case 0x0400:gray_status[0]=9;	gray_status_worse/=2;break;									//010000000000b
		case 0x0C00:gray_status[0]=10;	gray_status_worse/=2;break;									//110000000000b
		case 0x0800:gray_status[0]=11;	gray_status_worse/=2;break;									//100000000000b
		case 0x0000:gray_status[0]=gray_status_backup[0][0];gray_status_worse++;break; //00000000b
		default://����������������ж�
		{
			gray_status[0]=gray_status_backup[0][0];
			gray_status_worse++;
		}
	}	
	
	float pos_delta=smartcar_imu.state_estimation.speed*0.001f;
}
/***************************************************
������: void gray_turn_control_200hz(float *output)
˵��:	�Ҷȹ�Ѱ��ʱ��ת�����
���:	float *output-���������
****************************************************/
void gray_turn_control_200hz(float *output)
{
	//�����ϴο��������
	turn_output_last=turn_output;
	//ת��PD����������������ʵʱ��Ҫ��ߣ��������I��ʹ������Ӧ�ͺ�
	seektrack_ctrl[0].expect=0;							//����
	seektrack_ctrl[0].feedback=gray_status[0];	//����
	pid_control_run(&seektrack_ctrl[0]);		  //����������
	turn_output=seektrack_ctrl[0].output;
	//������������
	if(turn_output>0) turn_output+=steer_deadzone;
	if(turn_output<0) turn_output-=steer_deadzone;
	//����޷�
	turn_output=constrain_float(turn_output,-500,500);//ת���������޷�
	
	*output=0.75f*turn_output+0.25f*turn_output_last;//���Ϊǰ�����μ���ľ�ֵ
}

void gray_break(void)
{
	gray_status_backup[0][0]=gray_status[0]=0;
}


