#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#include "our_driver.h"

#define DT_DRV_COMPAT our_driver

struct our_driver_config {
    struct gpio_dt_spec led;
};

struct our_driver_data {
    bool led_state;
};

static int sample_fetch(const struct device *dev,
                        enum sensor_channel chan)
{
    const struct our_driver_config *config = dev->config;

    ARG_UNUSED(chan);

    int ret = gpio_pin_set_dt(&config->led, 1);

    if (ret == 0) {
        ((struct our_driver_data *)dev->data)->led_state = true;
    }

    return ret;
}

static int channel_get(const struct device *dev,
                       enum sensor_channel chan,
                       struct sensor_value *value)
{
    const struct our_driver_config *config = dev->config;

    ARG_UNUSED(chan);
    ARG_UNUSED(value);

    int ret = gpio_pin_set_dt(&config->led, 0);

    if (ret == 0) {
        ((struct our_driver_data *)dev->data)->led_state = false;
    }

    return ret;
}

int our_driver_toggle(const struct device *dev)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;

    data->led_state = !data->led_state;

    int ret = gpio_pin_set_dt(&config->led, data->led_state);

    if (ret != 0) {
        data->led_state = !data->led_state;
    }

    return ret;
}

static const DEVICE_API(sensor, our_driver_api) = {
    .sample_fetch = sample_fetch,
    .channel_get = channel_get,
};

static int init(const struct device *dev)
{
    const struct our_driver_config *config = dev->config;

    if (!gpio_is_ready_dt(&config->led)) {
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);

    if (ret == 0) {
        ((struct our_driver_data *)dev->data)->led_state = false;
    }

    return ret;
}

#define OUR_DRIVER_DEFINE(inst)                                      \
    static struct our_driver_data data_##inst;                      \
    static const struct our_driver_config config_##inst = {         \
        .led = GPIO_DT_SPEC_INST_GET(inst, gpios),                  \
    };                                                               \
    DEVICE_DT_INST_DEFINE(inst, init, NULL, &data_##inst,            \
                          &config_##inst,                           \
                          POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, \
                          &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)

/* Example from the lesson
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);


static int channel_get_my_implementation(const struct device *dev,
    enum sensor_channel chan, struct sensor_value *val)
    {
        LOG_INF("channel_get_my_implementation called for channel %d", chan);

        return 0;
    }

static DEVICE_API(sensor, api_lecture) = {
    .channel_get = channel_get_my_implementation,
};


static int init(const struct device *dev)
{
    LOG_INF("Initializing our_driver");
    return 0;
}

#define DEV_INST(inst) DEVICE_DT_INST_DEFINE(inst, init, NULL, NULL, NULL, POST_KERNEL, 80, &api_lecture);

DT_INST_FOREACH_STATUS_OKAY(DEV_INST);
*/
