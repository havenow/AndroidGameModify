
#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdbool.h>

#include "scanmem.h"

bool getcommand(globals_t *vars, char **line);


#ifdef __cplusplus
}
#endif

#endif
