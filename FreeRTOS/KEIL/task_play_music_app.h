#ifndef _TASK_PLAY_MUSIC_APP_H_
#define _TASK_PLAY_MUSIC_APP_H_

#include "task_comm_type.h"

void Audio_AdjustVolume(void);
void loadPlayMusic_Menu(void);
void Task3_Play_Music_Contrl(void *p_arg);
void Task_Play_Music(void *p_arg);
void UpdateMusicPlayStatus(PLAY_CTRL_E status, BOOL updateFlag);

#endif
