#ifndef __MPU_H
#define __MPU_H

#include "functions.h"
#include "filter.h"

#define PACKET_SIZE 14

int read_sensor(char *buffer, double * coefFilter);


#endif /* __MPU_H */
