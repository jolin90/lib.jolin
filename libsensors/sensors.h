#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <linux/input.h>
#include <utils/Log.h>
#include <utils/Atomic.h>
#include <utils/Timers.h>
#include <hardware/sensors.h>
#include <hardware/hardware.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct sensors_poll_context_t
{
	char dev_name[PATH_MAX];
	char input_name[PATH_MAX];

	int dev_fd;
	int input_fd;

	size_t sensor_count;
	struct sensor_t *sensor_list;

	struct sensors_event_t *event;

	int (*set_enable)(int handle, int enabled);
	int (*read_events)(sensors_event_t* data, int count);
};

struct sensors_poll_context_cfg
{
	int probe_ok;
	struct sensors_poll_context_t *sensors_poll_context_t;
};

struct sensors_poll_device
{
	struct sensors_poll_device_t device; // must be first

	size_t sensor_count;
	struct sensor_t *sensor_list;

	int pipefd[2];
	size_t pollfd_count;
	struct pollfd *pollfd_list;

	struct sensors_poll_context_cfg *sensors_poll_context_cfg;

};

#define LSG				(1.0f / 256.0f)
// conversion of acceleration data to SI units (m/s^2)
#define CONVERT_A			(GRAVITY_EARTH * LSG)
#define CONVERT_A_X			(CONVERT_A)
#define CONVERT_A_Y			(CONVERT_A)
#define CONVERT_A_Z			(CONVERT_A)

#define SENSORS_ACCELERATION_HANDLE				0
#define SENSORS_MAGNETIC_FIELD_HANDLE			1
#define SENSORS_ORIENTATION_HANDLE				2
#define SENSORS_LIGHT_HANDLE						3
#define SENSORS_PROXIMITY_HANDLE					4
#define SENSORS_GYROSCOPE_HANDLE					5
#define SENSORS_GRAVITY_HANDL						6
#define SENSORS_LINEAR_ACCELERATION_HANDLE		7
#define SENSORS_ROTATION_VECTOR_HANDLE			8
