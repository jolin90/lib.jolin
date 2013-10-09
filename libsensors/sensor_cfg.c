#include "sensors.h"

//extern struct sensors_poll_context_t cm36288_poll_context_t;
//extern struct sensors_poll_context_t msg21xx_poll_context_t;

extern struct sensors_context mxc622x_sensors_context;
extern struct sensors_context lis3de_sensors_context;

struct sensors_context_cfg sensors_context_cfg[] = {
	[0] = {.sensors_context = &mxc622x_sensors_context,},
	[1] = {.sensors_context = &lis3de_sensors_context,},
};

int get_sensors_context_lenght()
{
	return (int)ARRAY_SIZE(sensors_context_cfg);
}
