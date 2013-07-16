#include "sensors.h"
#include <utils/Timers.h>

#define	LIS3DE_ACC_IOCTL_BASE 77
#define	LIS3DE_ACC_IOCTL_SET_DELAY		_IOW(LIS3DE_ACC_IOCTL_BASE, 0, int)
#define	LIS3DE_ACC_IOCTL_GET_DELAY		_IOR(LIS3DE_ACC_IOCTL_BASE, 1, int)
#define	LIS3DE_ACC_IOCTL_SET_ENABLE		_IOW(LIS3DE_ACC_IOCTL_BASE, 2, int)
#define	LIS3DE_ACC_IOCTL_GET_ENABLE		_IOR(LIS3DE_ACC_IOCTL_BASE, 3, int)

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

static int set_enable(int handle, int enabled);

struct sensors_poll_context_t lis3de_poll_context_t = 
{
	.input_name = "lis3de_accelerometer",
	.dev_name = "/dev/lis3de_acc",

	.dev_fd = -1,
	.input_fd = -1,

	.sensor_count = ARRAY_SIZE(sSensorList),
	.sensor_list = sSensorList,

	.set_enable = set_enable,
};

static int set_enable(int handle, int enabled)
{
	int ret = -1;
	if (lis3de_poll_context_t.dev_fd > 0)
		ret = ioctl(lis3de_poll_context_t.dev_fd, LIS3DE_ACC_IOCTL_SET_ENABLE, &enabled);

	LOGE("%s %d %d %d ret=%d", __func__, handle, enabled, lis3de_poll_context_t.dev_fd, ret);

	return ret;
}

