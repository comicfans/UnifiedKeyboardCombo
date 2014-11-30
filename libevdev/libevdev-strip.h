#ifndef LIBEVDEV_STRIP_H_RDKJ89BB
#define LIBEVDEV_STRIP_H_RDKJ89BB

extern "C"
{

int
libevdev_event_code_from_name_n(unsigned int type, const char *name, size_t len);

int
libevdev_event_type_get_max(unsigned int type);

const char*
libevdev_event_code_get_name(unsigned int type, unsigned int code);
}

#endif /* end of include guard: LIBEVDEV_STRIP_H_RDKJ89BB */
