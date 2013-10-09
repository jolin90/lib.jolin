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

static int mxc622x_set_enable(int handle, int enabled);

static  struct sensor_t sSensorList[] =  {
	{
		.name = "Memsic MXC622x 2-axis Accelerometer",
		.vendor = "Memsic",
		.version = 1, 
		.handle = SENSORS_ACCELERATION_HANDLE,
		.type = SENSOR_TYPE_ACCELEROMETER,
		.maxRange = (GRAVITY_EARTH * 2.0f),
		.resolution = (GRAVITY_EARTH) / 256.0f,
		.power = 0.7f,
		.minDelay = 10000, 
	},
};

struct sensors_context mxc622x_sensors_context = 
{
	.input_name = "mxc622x_accelerometer",
	.dev_name = "/dev/mxc622x",

	.dev_fd = -1,
	.input_fd = -1,

	.sensor_count = ARRAY_SIZE(sSensorList),
	.sensor_list = sSensorList,

	.set_enable = mxc622x_set_enable,
};

static int mxc622x_set_enable(int handle, int enabled)
{
	int ret = -1;

	return ret;
}

