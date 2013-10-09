#include "sensors.h"
#include <utils/Timers.h>

#define	LIS3DE_ACC_IOCTL_BASE 77
#define	LIS3DE_ACC_IOCTL_SET_DELAY		_IOW(LIS3DE_ACC_IOCTL_BASE, 0, int)
#define	LIS3DE_ACC_IOCTL_GET_DELAY		_IOR(LIS3DE_ACC_IOCTL_BASE, 1, int)
#define	LIS3DE_ACC_IOCTL_SET_ENABLE		_IOW(LIS3DE_ACC_IOCTL_BASE, 2, int)
#define	LIS3DE_ACC_IOCTL_GET_ENABLE		_IOR(LIS3DE_ACC_IOCTL_BASE, 3, int)

static int lis3de_set_enable(int handle, int enabled);

static  struct sensor_t sSensorList[] =  {
	{
		.name = "ST LIS3DH 3-axis Accelerometer",
		.vendor = "ST",
		.version = 1, 
		.handle = SENSORS_ACCELERATION_HANDLE,
		.type = SENSOR_TYPE_ACCELEROMETER,
		.maxRange = (GRAVITY_EARTH * 2.0f),
		.resolution = (GRAVITY_EARTH) / 1024.0f,
		.power = 0.145f,
		.minDelay = 10000, 
	},
};

struct sensors_context lis3de_sensors_context = 
{
	.input_name = "accelerometer",
	.dev_name = "/dev/lis3de_acc",

	.dev_fd = -1,
	.input_fd = -1,

	.sensor_count = ARRAY_SIZE(sSensorList),
	.sensor_list = sSensorList,

	.set_enable = lis3de_set_enable,
};

static int lis3de_set_enable(int handle, int enabled)
{
	int ret = -1, dev_fd;
	struct sensors_context *sensors_context_t;

	sensors_context_t = &lis3de_sensors_context;
	dev_fd = sensors_context_t->dev_fd;

	if (dev_fd > 0)
		ret = ioctl(dev_fd, LIS3DE_ACC_IOCTL_SET_ENABLE, &enabled);

	SENSORS_DEBUG("%s handle:[%d] enable:[%d] dev_fd:[%d]", __func__, handle, enabled, dev_fd);
	
	return ret;
}

