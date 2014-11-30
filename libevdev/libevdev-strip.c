#include <stdlib.h>
#include "input.h"
#include "event-names.h"

int
libevdev_event_type_get_max(unsigned int type)
{
	if (type > EV_MAX)
		return -1;

	return ev_max[type];
}

const char*
libevdev_event_code_get_name(unsigned int type, unsigned int code)
{
	int max = libevdev_event_type_get_max(type);

	if (max == -1 || code > (unsigned int)max)
		return NULL;

	return event_type_map[type][code];
}
