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
    struct our_driver_data *data = dev->data;

    ARG_UNUSED(chan);

    if (value == NULL) {
        return -EINVAL;
    }

    int ret = gpio_pin_set_dt(&config->led, 0);

    if (ret == 0) {
        data->led_state = false;
        value->val1 = data->led_state;
        value->val2 = 0;
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

int our_driver_set_state(const struct device *dev, int state)
{
    if (dev == NULL || state < 0 || state > 1) {
        return -EINVAL;
    }

    if (!device_is_ready(dev)) {
        return -ENODEV;
    }

    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;
    int ret = gpio_pin_set_dt(&config->led, state);

    if (ret == 0) {
        data->led_state = state;
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

/* Lesson 7
static int handler(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = shell_device_get_binding(argv[1]);
    if (!dev) {
        shell_error(sh, "Device not found: %s", argv[1]);
        return -EFAULT;
    }

    struct sensor_value value;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_X, &value);

    if (ret) {
        shell_error(sh, "Failed to get channel value: %d", ret);
        return -EFAULT;
    }

    shell_info(sh, "%d", value.val1);

    return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(sub_our_driver,
    SHELL_CMD_ARG(channel_get, NULL, "Get the channel value", handler, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(our_driver, &sub_our_driver, "Our driver set of commands", NULL);
*/