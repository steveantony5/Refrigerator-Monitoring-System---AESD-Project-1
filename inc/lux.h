#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "i2c.h"
#include "logger.h"

#define CONTROL_REGISTER (0X00)
#define TIMING_REGISTER (0X01)
#define THRESHLOWLOW (0x02)
#define THRESHLOWHIGH (0x03)
#define THRESHHIGHLOW (0x04)
#define THRESHHIGHHIGH (0x05)
#define INTERRUPT (0x06)

#define INDICATION_REGISTER (0x0A)
#define DATA0LOW_REGISTER (0X0C)
#define DATA0HIGH_REGISTER (0X0D)
#define DATA1LOW_REGISTER (0X0E)
#define DATA1HIGH_REGISTER (0X0F)

#define WRITE_COMMAND (0x80)
#define WRITE_COMMAND_WORD (0xA0)
#define CLEAR_PENDING_INTERUPTS (0x40)

extern pthread_mutex_t lock_res;

int lux_sensor_setup();
int read_channel_0();
int read_channel_1();
float lux_measurement(float , float );
void has_state_transition_occurred(float );
float get_lux();
enum Status get_current_state_fridge(float);
int indication_register();
