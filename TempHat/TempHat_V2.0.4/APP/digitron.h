#ifndef __DIGITRON_H
#define __DIGITRON_H

#include "configure.h"

void digitron_init(void);
void digitron_enable(void);
void digitron_disable(void);
void digitron_set_value(float value);
void digitron_task_loop(void);

#endif