#ifndef __MPU_H
#define __MPU_H

#include "functions.h"
#include "filter.h"

#define PACKET_SIZE 14
#define FRAME_SIZE sizeof(data_t)

typedef struct {
    float rax;
    float ray;
    float raz;
    float rtmp;
    float rgx;
    float rgy;
    float rgz;

    float fax;
    float fay;
    float faz;
    float ftmp;
    float fgx;
    float fgy;
    float fgz;
} data_t;


int read_sensor(data_t *datos, double * coefFilter);


#endif /* __MPU_H */
