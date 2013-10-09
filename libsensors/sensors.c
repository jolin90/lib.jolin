#include "sensors.h"

extern int init_input_context();
extern int open_input(const char* inputName);
extern void close_input_unused();

extern int get_sensors_context_lenght();
extern struct sensors_context_cfg sensors_context_cfg[];

static struct sensors_poll_device sensors_poll_device_dev;

static void process_event(sensors_event_t* data, int code, int value)
{
	return;
}

static int poll__close(struct hw_device_t *dev)
{
	return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev, int handle, int enabled)
{
	int ret;
	SENSORS_DEBUG("%s %d enabled:[%d]", __func__, handle, enabled);

	
	return 0;
}

static int poll__setDelay(struct sensors_poll_device_t *dev,  int handle, int64_t ns)
{
	SENSORS_DEBUG("%s %d", __func__, handle);

	return 0;
}

static int poll__poll(struct sensors_poll_device_t *dev, sensors_event_t* data, int count)
{
	while(1)
		sleep(1);
	return 0;
}

static int init_input(int sensor_cfg_lenght)
{

	int i;
	int length, input_fd, dev_fd;
	struct sensors_context *sensors_context_t;

	length = sensor_cfg_lenght;
	
	for (i = 0; i < length ; i++)
	{
		sensors_context_t = sensors_context_cfg[i].sensors_context;
		sensors_context_t->is_probeok = 0;

		SENSORS_DEBUG("%s inputname:%s", __func__, sensors_context_t->input_name);

		if ((input_fd = open_input(sensors_context_t->input_name)) > 0)
		{
			sensors_context_t->input_fd = input_fd;
			sensors_context_t->is_probeok = 1;

			if (sensors_context_t->dev_name != NULL)
			{
				SENSORS_DEBUG("%s devname:[%s]", __func__, sensors_context_t->dev_name);

				dev_fd = open(sensors_context_t->dev_name, O_RDWR);
				if (dev_fd < 0) 
					SENSORS_LOGE("%s Couldn't open %s (%s)", __func__, sensors_context_t->dev_name, strerror(errno));
				else 
					sensors_context_t->dev_fd = dev_fd;
			}
			sensors_poll_device_dev.sensor_count += sensors_context_t->sensor_count;
			sensors_poll_device_dev.pollfd_count++;
		}

		SENSORS_DEBUG("%s intputfd:[%d], devfd:[%d]", __func__, sensors_context_t->input_fd, sensors_context_t->dev_fd);
	}

	SENSORS_DEBUG("%s sensor list count is [%d], pollfd count is [%d]", __func__, sensors_poll_device_dev.sensor_count, sensors_poll_device_dev.pollfd_count);

	close_input_unused();

	return 0;
}

static int init_pollfd(int sensor_cfg_lenght)
{
	int i;
	struct sensors_context *sensors_context_t;
	struct sensors_context_cfg *sensors_context_cfg_t;

	struct pollfd *pollfd_list_t;

	sensors_poll_device_dev.pollfd_list = (struct pollfd *)malloc(sensors_poll_device_dev.pollfd_count * sizeof(struct pollfd));
	pollfd_list_t = (struct pollfd *)(sensors_poll_device_dev.pollfd_list);

	for (i = 0; i < sensor_cfg_lenght; i++)
	{
		sensors_context_t = sensors_context_cfg[i].sensors_context;
		if (sensors_context_t->is_probeok == 1)
		{
			pollfd_list_t->fd =  sensors_context_t->input_fd;
			pollfd_list_t->events = POLLIN;
			pollfd_list_t->revents = 0;
			pollfd_list_t++;		
		}
		sensors_context_cfg_t++;
	}

#ifdef JOLIN_DEBUG
	pollfd_list_t = (struct pollfd *)(sensors_poll_device_dev.pollfd_list);
	for (i = 0; i < (int)sensors_poll_device_dev.pollfd_count; i++)
	{
		SENSORS_DEBUG("%s fd=%d pollfd_count=%d", __func__, pollfd_list_t->fd, (int)sensors_poll_device_dev.pollfd_count);
		pollfd_list_t++;
	}
#endif

	return sensors_poll_device_dev.pollfd_count; 
}

static int init_sensors_list(int sensor_cfg_lenght)
{
	int i, count;
	struct sensor_t *sensor_list_t;
	struct sensor_t *sensors_list_t;
	struct sensors_context *sensors_context_t;

	sensors_poll_device_dev.sensor_list = (struct sensor_t*)malloc(sensors_poll_device_dev.sensor_count * sizeof(struct sensor_t));
	sensors_list_t = sensors_poll_device_dev.sensor_list;
	memset(sensors_list_t, 0, (sensors_poll_device_dev.sensor_count) * sizeof(struct sensor_t));

	for (i = 0; i < sensor_cfg_lenght; i++)
	{
		sensors_context_t = sensors_context_cfg[i].sensors_context;
		if (sensors_context_t->is_probeok == 1)
		{
			count = sensors_context_t->sensor_count;
			sensor_list_t 	= sensors_context_t->sensor_list;

			while(count--)
			{
				sensors_list_t->name    	= sensor_list_t->name;
				sensors_list_t->vendor  	= sensor_list_t->vendor;
				sensors_list_t->version 	= sensor_list_t->version;
				sensors_list_t->handle  	= sensor_list_t->handle;
				sensors_list_t->type    	      = sensor_list_t->type;
				sensors_list_t->maxRange 	= sensor_list_t->maxRange;
				sensors_list_t->resolution    = sensor_list_t->resolution;
				sensors_list_t->power 		= sensor_list_t->power;
				sensors_list_t->minDelay 	= sensor_list_t->minDelay;

				sensor_list_t++;
				sensors_list_t++;
			}
		}
	}

#if 1
	sensors_list_t = sensors_poll_device_dev.sensor_list;
	for (i = 0; i < (int)sensors_poll_device_dev.sensor_count; i++)
	{
		SENSORS_DEBUG("%s name is %s", __func__, sensors_list_t->name);
	}
#endif

	return sensors_poll_device_dev.sensor_count;
}

static int open_sensors(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
	int sensor_cfg_length;

	SENSORS_DEBUG("%s is start", __func__);

	memset(&sensors_poll_device_dev, 0, sizeof(struct sensors_poll_device));
	memset(&sensors_poll_device_dev.device, 0, sizeof(struct sensors_poll_device_t));

	sensors_poll_device_dev.device.common.tag = HARDWARE_DEVICE_TAG;
	sensors_poll_device_dev.device.common.version  = 0;
	sensors_poll_device_dev.device.common.module   = (struct hw_module_t *)(module);
	sensors_poll_device_dev.device.common.close    = poll__close;
	sensors_poll_device_dev.device.activate        = poll__activate;
	sensors_poll_device_dev.device.setDelay        = poll__setDelay;
	sensors_poll_device_dev.device.poll            = poll__poll;

	*device = &sensors_poll_device_dev.device.common;

	sensors_poll_device_dev.sensors_context_cfg = sensors_context_cfg;
	sensors_poll_device_dev.sensor_count = 0;
	sensors_poll_device_dev.pollfd_count = 0;

	sensor_cfg_length = get_sensors_context_lenght();
	SENSORS_DEBUG("%s sensor_cfg_length=%d", __func__, sensor_cfg_length);

	// open input , devices.
	init_input(sensor_cfg_length);

	// init pollfd
	init_pollfd(sensor_cfg_length);

	// init sensor_list
	init_sensors_list(sensor_cfg_length);

	SENSORS_DEBUG("%s is eng", __func__);

	return 0;
}

static int sensors_get_sensors_list(struct sensors_module_t* module, struct sensor_t const** list)
{	
	*list = sensors_poll_device_dev.sensor_list;
	return sensors_poll_device_dev.sensor_count;
}

static struct hw_module_methods_t sensors_module_methods = {
	.open = open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM =
{
	.common =
	{
		.tag = HARDWARE_MODULE_TAG,
		.version_major = 1,
		.version_minor = 0,
		.id = SENSORS_HARDWARE_MODULE_ID,
		.name = "Sensor HAL",
		.author = "Jolin Zhang",
		.methods = &sensors_module_methods
	},

	.get_sensors_list = sensors_get_sensors_list
};
