#include "sensors.h"

#define LTR_IOCTL_MAGIC 				0x1C
#define LTR_IOCTL_GET_PFLAG  			_IOR(LTR_IOCTL_MAGIC, 1, int)
#define LTR_IOCTL_GET_LFLAG  			_IOR(LTR_IOCTL_MAGIC, 2, int)
#define LTR_IOCTL_SET_PFLAG  			_IOW(LTR_IOCTL_MAGIC, 3, int)
#define LTR_IOCTL_SET_LFLAG  			_IOW(LTR_IOCTL_MAGIC, 4, int)
#define LTR_IOCTL_GET_DATA  			_IOW(LTR_IOCTL_MAGIC, 5, unsigned char)

static  struct sensor_t sSensorList[] =  {
	{
		.name = "CM36283 Light sensor",
		.vendor = "tritonFN",
		.version = 1, 
		.handle = SENSORS_PROXIMITY_HANDLE,
		.type = SENSOR_TYPE_LIGHT,
		.maxRange =  1.0f,
		.resolution = 1.0f,
		.power = 0.005f,
		.minDelay = 0, 
	},
	{
		.name = "CM36283 Proximity sensor",
		.vendor = "tritonFN",
		.version = 1, 
		.handle = SENSORS_LIGHT_HANDLE,
		.type = SENSOR_TYPE_PROXIMITY,
		.maxRange =  1.0f,
		.resolution = 1.0f,
		.power = 0.005f,
		.minDelay = 0, 
	},
};

static int set_enable(int handle, int enabled);

struct sensors_poll_context_t cm36288_poll_context_t = 
{
	.input_name = "proximity",
	.dev_name = "/dev/cm36283",

	.dev_fd = -1,
	.input_fd = -1,

	.sensor_count = ARRAY_SIZE(sSensorList),
	.sensor_list = sSensorList,

	.set_enable = set_enable,
};

static int set_enable(int handle, int enabled)
{
	int ret = -1;

	if (cm36288_poll_context_t.dev_fd > 0)
	{
		if (handle == 4) {
			ret = ioctl(cm36288_poll_context_t.dev_fd, LTR_IOCTL_SET_LFLAG, &enabled);
			LOGE("%s %s %d dev_fd=%d ret=%d handle=%d enabled=%d %s", \ 
					"cm36288.c", __func__, __LINE__, cm36288_poll_context_t.dev_fd, \
					ret, handle, enabled, strerror(errno));
		}
		if (handle == 3) {
			ret = ioctl(cm36288_poll_context_t.dev_fd, LTR_IOCTL_SET_PFLAG, &enabled);
			LOGE("%s %s %d dev_fd=%d ret=%d handle=%d enabled=%d %s",  \
					"cm36288.c", __func__, __LINE__, cm36288_poll_context_t.dev_fd, \
					ret, handle, enabled, strerror(errno));
		}
	}

	LOGE("%s %s %d dev_fd=%d ret=%d handle=%d enabled=%d %s",  \
			"cm36288.c", __func__, __LINE__, cm36288_poll_context_t.dev_fd, \
			ret, handle, enabled, strerror(errno));

	return ret;
}

