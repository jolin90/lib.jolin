#include "sensors.h"

extern struct sensors_poll_context_t mxc622x_poll_context_t;
extern struct sensors_poll_context_t cm36288_poll_context_t;

struct sensors_poll_context_cfg sensors_poll_context[] = {
	[0] = {
		.probe_ok = 0,
		.sensors_poll_context_t = &mxc622x_poll_context_t,
	},
	[1] = {
		.probe_ok = 0,
		.sensors_poll_context_t = &cm36288_poll_context_t,
	},
};

static struct sensors_poll_device sensors_poll_device_dev;

static void process_event(sensors_event_t* data, int code, int value)
{
	switch(code)
	{
	case ABS_X:
		data->sensor = 0;
		data->type = SENSOR_TYPE_ACCELEROMETER;
		data->acceleration.x = value * CONVERT_A_X;
		LOGE("%s %d ABS_X=%d x=%d", __func__, __LINE__, ABS_X, (int)data->acceleration.x);
		break;
	case ABS_Y:
		data->sensor = 0;
		data->type = SENSOR_TYPE_ACCELEROMETER;
		data->acceleration.y = value * CONVERT_A_Y;
		LOGE("%s %d ABS_Y=%d y=%d", __func__, __LINE__, ABS_Y, (int)data->acceleration.y);
		break;
	case ABS_Z:
		data->sensor = 0;
		data->type = SENSOR_TYPE_ACCELEROMETER;
		data->acceleration.z = value * CONVERT_A_Z;
		LOGE("%s %d ABS_Z=%d z=%d", __func__, __LINE__, ABS_Z, (int)data->acceleration.z);
		break;
	case ABS_DISTANCE:
		data->sensor = 3;
		data->type = SENSOR_TYPE_PROXIMITY;
		data->distance = (float)value;
		LOGE("%s %d ABS_DISTANCE=%d distance=%d", __func__, __LINE__, ABS_DISTANCE, (int)data->distance);
		break;
	case ABS_MISC:
		data->sensor = 4;
		data->type = SENSOR_TYPE_LIGHT;
		data->light = (float)value;
		LOGE("%s %d ABS_MISC=%d light=%d", __func__, __LINE__, ABS_MISC, (int)data->light);
		break; 			
	default:
		LOGE("%s %d default and code=%d", __func__, __LINE__, code);
		break;
	}
}

static int open_input(const char* inputName) {
	int fd = -1;
	const char *dirname = "/dev/input";
	char devname[PATH_MAX];
	char *filename;
	DIR *dir;
	struct dirent *de;
	dir = opendir(dirname);
	if(dir == NULL)
		return -1;
	strcpy(devname, dirname);
	filename = devname + strlen(devname);
	*filename++ = '/';
	while((de = readdir(dir))) {
		if(de->d_name[0] == '.' &&
				(de->d_name[1] == '\0' ||
				 (de->d_name[1] == '.' && de->d_name[2] == '\0')))
			continue;
		strcpy(filename, de->d_name);
		fd = open(devname, O_RDONLY);
		if (fd>=0) {
			char name[80];
			if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
				name[0] = '\0';
			}
			if (!strcmp(name, inputName)) {
				// strcpy(input_name, filename);
				break;
			} else {
				close(fd);
				fd = -1;
			}
		}
	}
	closedir(dir);
	LOGE_IF(fd<0, "couldn't find '%s' input device", inputName);
	return fd;
}

static int poll__close(struct hw_device_t *dev)
{
	return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev, int handle, int enabled)
{
	int i, j, m, ret = -1;
	struct sensor_t * sensor_list_t;

	LOGE("%s handle=%d, enabled=%d", __func__, handle, enabled);

	for (i = 0; i < (int)ARRAY_SIZE(sensors_poll_context); i++)
	{
		if (1 != sensors_poll_context[i].probe_ok) 
			continue;

		m = 0;
		for (j = 0; j < (int)sensors_poll_context[i].sensors_poll_context_t->sensor_count; j++) 
		{
			sensor_list_t = (struct sensor_t *)(sensors_poll_context[i].sensors_poll_context_t->sensor_list + m);
			if (sensor_list_t->handle == handle) {
				sensors_poll_context[i].sensors_poll_context_t->set_enable(handle, enabled);
				break;
			}
			m++;
		}
	}

	return 0;
}

static int poll__setDelay(struct sensors_poll_device_t *dev,  int handle, int64_t ns)
{
	LOGE("%s %d", __func__, handle);

	return 0;
}

static int poll__poll(struct sensors_poll_device_t *dev, sensors_event_t* data, int count)
{
	int i;
	int numEventReceived = 0;
	ssize_t rdlen;
	struct input_event evbuff[count], *evbuff_p, *evbuff_p_end;
	int64_t timestamp;
	struct pollfd *pollfd_list_temp;

	LOGE("\n\n");
	LOGE("%s start", __func__);
#if 0 //	debug
	pollfd_list_temp = (struct pollfd *)(sensors_poll_device_dev.pollfd_list);
	for (i = 0; i < (int)sensors_poll_device_dev.pollfd_count; i++)
	{
		LOGE("%s input_fd=%d pollfd_count=%d count=%d", __func__, pollfd_list_temp->fd, (int)sensors_poll_device_dev.pollfd_count, count);
		pollfd_list_temp++;
	}
#endif

	poll(sensors_poll_device_dev.pollfd_list, sensors_poll_device_dev.pollfd_count, -1);

	timestamp = systemTime(SYSTEM_TIME_MONOTONIC);

	pollfd_list_temp = (struct pollfd *)(sensors_poll_device_dev.pollfd_list);

	for (i = 0; i < (int)sensors_poll_device_dev.pollfd_count; i++)
	{
		if (pollfd_list_temp->revents)
		{
			rdlen = read(pollfd_list_temp->fd, evbuff, sizeof(evbuff));
			LOGE("%s input_fd=%d rdlen=%d", __func__, pollfd_list_temp->fd, (int)rdlen/sizeof(struct input_event));
			if (rdlen <= 0) 
			{
				pollfd_list_temp++;
				continue;
			}
			
			memset(data, 0, sizeof(struct input_event));

			for (evbuff_p = evbuff, evbuff_p_end = evbuff + (int)rdlen/sizeof(struct input_event); evbuff_p < evbuff_p_end; evbuff_p++)
			{
				if (evbuff_p->type == EV_SYN)
				{
					data->version = sizeof(sensors_event_t);
					data->timestamp = timestamp;
					data++;
					numEventReceived++;
					LOGE("%s %d EV_SYN", __func__, __LINE__);
					break;
				} else if (evbuff_p->type == EV_ABS) {
					LOGE("%s %d EV_ABS", __func__, __LINE__);
					process_event(data, evbuff_p->code, evbuff_p->value);
					continue;
				}
			}				

			pollfd_list_temp->revents = 0;
			LOGE("%s input_fd=%d rdlen=%d", __func__, pollfd_list_temp->fd, (int)rdlen/sizeof(struct input_event));
		}
		pollfd_list_temp++;
	}
	
	//LOGE("%s numEventReceived=%d count=%d", __func__, numEventReceived, count);

	LOGE("%s end", __func__);
	LOGE("\n\n");
	return numEventReceived;
}

static int open_sensors(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
	int i , input_fd, dev_fd;
	int m ,n, count;

	LOGE("%s", __func__);

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

	sensors_poll_device_dev.sensors_poll_context_cfg = sensors_poll_context;
	sensors_poll_device_dev.sensor_count = 0;
	sensors_poll_device_dev.pollfd_count = 0;

	for (i = 0; i < (int)ARRAY_SIZE(sensors_poll_context); i++)
	{
		if ((input_fd = open_input(sensors_poll_context[i].sensors_poll_context_t->input_name))  < 0)
			continue;
		sensors_poll_context[i].probe_ok = 1;
		sensors_poll_context[i].sensors_poll_context_t->input_fd = input_fd;

		if ( NULL != sensors_poll_context[i].sensors_poll_context_t->dev_name) 
		{
			dev_fd = open(sensors_poll_context[i].sensors_poll_context_t->dev_name, O_RDWR);
			if (dev_fd < 0) 
				LOGE("Couldn't open %s (%s)", sensors_poll_context[i].sensors_poll_context_t->dev_name, strerror(errno));
			else 
				sensors_poll_context[i].sensors_poll_context_t->dev_fd = dev_fd;
		}

		sensors_poll_device_dev.sensor_count += sensors_poll_context[i].sensors_poll_context_t->sensor_count;
		sensors_poll_device_dev.pollfd_count++;

		LOGE("%s i=%d input_fd=%d input_name=%s dev_fd=%d", \
				__func__, i, input_fd, sensors_poll_context[i].sensors_poll_context_t->input_name, \
				 sensors_poll_context[i].sensors_poll_context_t->dev_fd);
	}

	//==================================================================================================================
	// init poll
	struct pollfd *pollfd_list_temp;
	sensors_poll_device_dev.pollfd_list = (struct pollfd *)malloc(sensors_poll_device_dev.pollfd_count * sizeof(struct pollfd));
	pollfd_list_temp = (struct pollfd *)(sensors_poll_device_dev.pollfd_list);

	for (i = 0; i < (int)ARRAY_SIZE(sensors_poll_context); i++)
	{
		if (1 == sensors_poll_context[i].probe_ok) 
		{
			pollfd_list_temp->fd = sensors_poll_context[i].sensors_poll_context_t->input_fd;
			pollfd_list_temp->events = POLLIN;
			pollfd_list_temp->revents = 0;
			pollfd_list_temp++;
		}
	}

#if 1 //	debug
	pollfd_list_temp = (struct pollfd *)(sensors_poll_device_dev.pollfd_list);
	for (i = 0; i < (int)sensors_poll_device_dev.pollfd_count; i++)
	{
		LOGE("fd=%d pollfd_count=%d", pollfd_list_temp->fd, (int)sensors_poll_device_dev.pollfd_count);
		pollfd_list_temp++;
	}
#endif

	//==================================================================================================================
	// init sensor_list
	m = 0;
	struct sensor_t *sensor_list;
	struct sensor_t *sensor_list_t;

	sensors_poll_device_dev.sensor_list = (struct sensor_t*)malloc(sensors_poll_device_dev.sensor_count * sizeof(struct sensor_t));
	memset(sensors_poll_device_dev.sensor_list, 0, sensors_poll_device_dev.sensor_count * sizeof(struct sensor_t));


	for (i = 0; i < (int)ARRAY_SIZE(sensors_poll_context); i++)
	{
		if (1 == sensors_poll_context[i].probe_ok) 
		{
			n = 0;
			count = sensors_poll_context[i].sensors_poll_context_t->sensor_count;
			while(count--) 
			{
				sensor_list = (struct sensor_t *)(sensors_poll_device_dev.sensor_list + m) ;
				sensor_list_t = (struct sensor_t *)(sensors_poll_context[i].sensors_poll_context_t->sensor_list + n);

				sensor_list->name = sensor_list_t->name;
				sensor_list->vendor = sensor_list_t->vendor;
				sensor_list->version = sensor_list_t->version;
				sensor_list->handle =  sensor_list_t->handle;
				sensor_list->type =  sensor_list_t->type;
				sensor_list->maxRange =  sensor_list_t->maxRange;
				sensor_list->resolution =  sensor_list_t->resolution;
				sensor_list->power =  sensor_list_t->power;
				sensor_list->minDelay =  sensor_list_t->minDelay;

				LOGE("%s i=%d name=%s handle=%d type%d", __func__, i, sensor_list->name, sensor_list->handle, sensor_list->type);
				m++;
				n++;
			}
		}
	}

	LOGE("%s %d sensors_poll_device_dev.sensor_count=%d", __func__, __LINE__, sensors_poll_device_dev.sensor_count);
	LOGE("%s %d sensors_poll_device_dev.sensor_count=%d", __func__, __LINE__, sensors_poll_device_dev.pollfd_count);

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
