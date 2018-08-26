#ifndef DEVICE_FILE_H_
#define DEVICE_FILE_H_

#include <linux/compiler.h>

__must_check int register_device(void);
void unregister_device(void);

#endif
