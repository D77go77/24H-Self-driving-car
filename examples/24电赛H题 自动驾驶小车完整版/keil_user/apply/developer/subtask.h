#ifndef __SUBTASK_H
#define __SUBTASK_H


extern uint16_t flight_subtask_cnt[20];

void flight_subtask_reset(void);

void auto_line_go(void);//����1
void auto_line_go_2(void);//����2
void auto_line_cha_1(void);//����3
void auto_line_cha_2(void);//����4

extern u8 round_grey;
#endif
