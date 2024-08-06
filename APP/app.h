#ifndef __APP_H__
#define __APP_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp_driver.h"
#include "main.h"
#include "segment_driver.h"

void System_Run(void);

typedef enum { Normal = 0, Special = 1 } DevMode_t;
typedef enum { regulate_Temperatue = 0, regulate_lightness = 1 } lightMode_t;

#endif //
