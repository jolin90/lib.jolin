#include "sensors.h"
#include <utils/Timers.h>

#define	MXC622X_ACC_IOCTL_BASE 77
/** The following define the IOCTL command values via the ioctl macros */
#define	MXC622X_ACC_IOCTL_SET_DELAY		_IOW(MXC622X_ACC_IOCTL_BASE, 0, int)
#define	MXC622X_ACC_IOCTL_GET_DELAY		_IOR(MXC622X_ACC_IOCTL_BASE, 1, int)
#define	MXC622X_ACC_IOCTL_SET_ENABLE		_IOW(MXC622X_ACC_IOCTL_BASE, 2, int)
#define	MXC622X_ACC_IOCTL_GET_ENABLE		_IOR(MXC622X_ACC_IOCTL_BASE, 3, int)
#define	MXC622X_ACC_IOCTL_GET_COOR_XYZ       _IOW(MXC622X_ACC_IOCTL_BASE, 22, int)
#define	MXC622X_ACC_IOCTL_GET_CHIP_ID        _IOR(MXC622X_ACC_IOCTL_BASE, 255, char[32])

static  struct sensor_t sSensorList[] =  {
	{
		.name = "Memsic MXC622x 2-axis Accelerometer",
		.vendor = "Memsic",
		.version = 1, 
		.handle = 0,
		.type = SENSOR_TYPE_ACCELEROMETER,
		.maxRange = (GRAVITY_EARTH * 2.0f),
		.resolution = (GRAVITY_EARTH) / 256.0f,
		.power = 0.7f,
		.minDelay = 10000, 
	},
};

static int set_enable(int handle, int enabled);

struct sensors_poll_context_t mxc622x_poll_context_t = 
{
	.input_name = "accelerometer",
	.dev_name = "/dev/mxc622x",

	.dev_fd = -1,
	.input_fd = -1,

	.sensor_count = ARRAY_SIZE(sSensorList),
	.sensor_list = sSensorList,

	.set_enable = set_enable,
};

static int set_enable(int handle, int enabled)
{
	int ret = -1;
	if (mxc622x_poll_context_t.dev_fd > 0)
		ret = ioctl(mxc622x_poll_context_t.dev_fd, MXC622X_ACC_IOCTL_SET_ENABLE, &enabled);

	LOGE("%s %d %d %d ret=%d", __func__, handle, enabled, mxc622x_poll_context_t.dev_fd, ret);

	return ret;
}

