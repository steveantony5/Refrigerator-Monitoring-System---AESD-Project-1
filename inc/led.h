#ifndef LED_H
#define LED_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>



int led_on();

int led_off();

int gpio_pin_init();
#endif