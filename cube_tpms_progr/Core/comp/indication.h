#ifndef _FW_UPDATER_H
#define _FW_UPDATER_H

#include "app_cfg.h"

typedef enum {
    FIRMWARE_UPDATE,
	NO_TASK
} IND_TASK_COMMAND; // Задачи для сервиса индикации

extern void start_indication(void *argument);

extern void set_indication_task(UINT8 cmd);


#endif /* _FW_UPDATER_H */
