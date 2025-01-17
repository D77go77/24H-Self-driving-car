#include "ti_msp_dl_config.h"
#include "ngpio.h"
_laser_light beep;
/***************************************
������:	void GPIO_Init(void)
˵��: GPIO��ʼ��
***************************************/
void nGPIO_Init(void)
{
  beep.port = PORTA_PORT;
  beep.pin = PORTA_beep_PIN;
  beep.period = 10; //200*5ms
  beep.light_on_percent = 0.01f;
	
  beep.reset = 1;
  beep.times = 0;
}
/***************************************
������:	laser_light_work(_laser_light *light)
˵��: gpio����״̬��
���:	_laser_light *light-gpio���ƽṹ��
***************************************/
void laser_light_work(_laser_light *light)
{
	if(light->reset==1)
	{
		light->reset=0;
		light->cnt=0;
		light->times_cnt=0;//��������������
		light->end=0;
	}
	if(light->times_cnt==light->times)
	{
		light->end=1;
		return;
	}
	light->cnt++;
	if(light->cnt<=light->period*light->light_on_percent)
	{
		DL_GPIO_setPins(light->port, light->pin);
	}
	else if(light->cnt<light->period)
	{
		DL_GPIO_clearPins(light->port, light->pin);
	}
	else//��ɵ���һ��
	{
		light->cnt=0;
		light->times_cnt++;
	}
}

void buzzer_setup(uint32_t _period, float _light_on_percent, uint16_t _times)
{
  beep.period = _period; //20*5ms
  beep.light_on_percent = _light_on_percent;
  beep.reset = 1;
  beep.times = _times;
}