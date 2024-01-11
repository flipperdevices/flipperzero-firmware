#ifndef MINESWEEPER_SPEAKER_H
#define MINESWEEPER_SPEAKER_H

#define NOTE_OK 3078.95f //G_4
#define NOTE_FLAG 384.87f //G_4
#define NOTE_OOB 342.88f //F_4
#define NOTE_WIN 432.00f //Divine
#define NOTE_LOSE 4170.00f //Cursed

void mine_sweeper_play_ok_sound(void* context);
void mine_sweeper_play_flag_sound(void* context);
void mine_sweeper_play_oob_sound(void* context);
void mine_sweeper_play_win_sound(void* context);
void mine_sweeper_play_lose_sound(void* context);
void mine_sweeper_stop_all_sound(void* context);

#endif
