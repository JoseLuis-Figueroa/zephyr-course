#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

int our_driver_toggle(const struct device *dev);
int our_driver_set_state(const struct device *dev, int state);

#ifdef __cplusplus
}
#endif

#endif