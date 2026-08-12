#ifndef AUTO_LOCK_H
#define AUTO_LOCK_H

extern int g_auto_unlock_active;
extern int g_auto_unlock_door_opened;
extern int g_autoUnlockEnabled;

void AutoLock_Update(void);
void AutoLock_SetEnabled(int enabled);

#endif
