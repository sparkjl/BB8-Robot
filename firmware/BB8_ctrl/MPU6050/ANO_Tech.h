#ifndef __ANO_TECH_H
#define __ANO_TECH_H
#include "config.h"

void Send_data_to_ANO(unsigned char fun, unsigned char *data, unsigned char len);
void ANO_report_1(short aacx,short aacy,short aacz,short gyrox,short gyroy, \
  short gyroz,short roll,short pitch,short yaw);
void ANO_report_2(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);

#endif