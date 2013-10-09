#include "sensors.h"

struct input_config
{
	int    input_fd;
	int    is_used;
	char   input_name[256];
	char   input_event[256];
};

static struct input_config *input_cfg;
static int input_event_count = 0;
static int is_input_init = 0;

static int get_event_count()
{
	const char *dirname = "/dev/input";
	DIR *dir;
	struct dirent *de;

	int event_count = 0;

	dir = opendir(dirname);
	if(dir == NULL)
		return -1;

	while ((de = readdir(dir)))
	{
		if(de->d_name[0] == '.' &&	(de->d_name[1] == '\0' ||  (de->d_name[1] == '.' && de->d_name[2] == '\0')))
			continue;
		event_count++;
		SENSORS_DEBUG("%s %s", __func__, de->d_name);
	}

	return event_count; 
}

static int init_input_context()
{
	struct input_config *input_cfg_t;

	int ioctl_ret;
	int event_count;
	int input_fd;

	const char *dirname = "/dev/input";
	char devname[PATH_MAX];
	char *filename;
	DIR *dir;
	struct dirent *de;

	if (is_input_init == 0)
	{
		dir = opendir(dirname);
		if(dir == NULL)
			return -1;
		strcpy(devname, dirname);
		filename = devname + strlen(devname);
		*filename++ = '/';

		event_count = get_event_count();
		input_event_count = event_count;

		SENSORS_DEBUG("Get event count is %d", event_count);

		input_cfg = (struct input_config *)malloc(event_count * sizeof(struct input_config));
		memset(input_cfg, 0, event_count * sizeof(struct input_config));

		input_cfg_t = input_cfg;

		while((de = readdir(dir))) 
		{
			if(de->d_name[0] == '.' &&
					(de->d_name[1] == '\0' ||
					 (de->d_name[1] == '.' && de->d_name[2] == '\0')))
				continue;

			strcpy(filename, de->d_name);

			SENSORS_DEBUG("%s de->d_name:%s filename:%s devname:%s", __func__, de->d_name, filename, devname);

			input_fd = open(devname, O_RDONLY);
			if (input_fd > 0) 
			{
				char name[80];
				ioctl_ret = ioctl(input_fd, EVIOCGNAME(sizeof(name) - 1), &name);
				strcpy(input_cfg_t->input_name, name);
				strcpy(input_cfg_t->input_event, filename);
				input_cfg_t->input_fd = input_fd;

				SENSORS_DEBUG("%s input_fd:%d  input_name:%s input_event:%s", __func__, input_fd, input_cfg_t->input_name, input_cfg_t->input_event);
				input_cfg_t++;
			} else {
				SENSORS_LOGE("%s Couldn't open %s (%s)", __func__, devname, strerror(errno));
			}

		}

		is_input_init = 1;
	}

	return -1;
}

int open_input(const char* inputName)
{
	struct input_config *input_cfg_t;
	int input_fd = -1, i;

	SENSORS_DEBUG("%s input_fd:%d %s start >>>>>>>>>>>>>>>", __func__, input_fd, inputName);

	init_input_context();
	input_cfg_t = input_cfg;

	for (i = 0; i < input_event_count; i++) {

		SENSORS_DEBUG("%s input_fd:%d  input_name:%s input_event:%s", __func__, input_cfg_t->input_fd, input_cfg_t->input_name, input_cfg_t->input_event);

		if (!strcmp(input_cfg_t->input_name, inputName)) {
			input_cfg_t->is_used= 1;
			SENSORS_DEBUG("%s fd:%d end <<<<<<<<<<<<<<<<<<", __func__, input_cfg_t->input_fd);
			return input_cfg_t->input_fd;
		}

		input_cfg_t++;
	}

	SENSORS_DEBUG("%s fd:%d end <<<<<<<<<<<<<<<<<<", __func__, input_fd);

	return input_fd;
}

void close_input_unused()
{
	int i;
	struct input_config *input_cfg_t;

	input_cfg_t = input_cfg;
	for (i = 0; i < input_event_count; i++) {
		SENSORS_DEBUG("jolin: %s input_name:%s input_event:%s", __func__, input_cfg_t->input_name, input_cfg_t->input_event);
		if ((input_cfg_t->is_used == 0) && (input_cfg_t->input_fd > 0)){
				close(input_cfg_t->input_fd);
				input_cfg_t->is_used = -1;
				SENSORS_DEBUG("jolin: %s closed input_name:%s input_event:%s", __func__, input_cfg_t->input_name, input_cfg_t->input_event);
		}
		input_cfg_t++;
	}
}
