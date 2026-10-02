#include <errno.h>
#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include "our_driver.h"

static const struct device *get_sensor(const struct shell *sh)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device is not ready");
        return NULL;
    }

    return dev;
}

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    (void)argc;
    (void)argv;

    const struct device *dev = get_sensor(sh);
    if (dev == NULL) {
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", ret);
        return ret;
    }

    shell_print(sh, "Sample fetched");
    return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    (void)argc;
    (void)argv;

    const struct device *dev = get_sensor(sh);
    if (dev == NULL) {
        return -ENODEV;
    }

    struct sensor_value value = {0};
    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &value);

    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", ret);
        return ret;
    }

    shell_print(sh, "Value: %d.%06d", value.val1, value.val2);
    return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    (void)argc;
    (void)argv;

    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

    shell_print(sh, "Device: %s", dev->name);
    shell_print(sh, "Ready: %s", device_is_ready(dev) ? "yes" : "no");
    return 0;
}

static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 1 || argv[0] == NULL) {
        shell_error(sh, "Missing LED state; use 0 (off) or 1 (on)");
        return -EINVAL;
    }

    char *end;
    errno = 0;
    long state = strtol(argv[0], &end, 10);
    if (errno == ERANGE || end == argv[0] || *end != '\0' ||
        state < 0 || state > 1) {
        shell_error(sh, "Invalid LED state; use 0 (off) or 1 (on)");
        return -EINVAL;
    }

    const struct device *dev = get_sensor(sh);
    if (dev == NULL) {
        return -ENODEV;
    }

    int ret = our_driver_set_state(dev, (int)state);
    if (ret < 0) {
        shell_error(sh, "Failed to set LED state: %d", ret);
        return ret;
    }

    shell_print(sh, "LED turned %s", state == 1 ? "on" : "off");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_commands,
    SHELL_CMD(fetch, NULL, "Fetch a sensor sample", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Read and print the sensor value", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Show sensor device information", cmd_sensor_info),
    SHELL_CMD_ARG(set, NULL, "Set LED state: 0=off, 1=on", cmd_sensor_set, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_commands, "Sensor commands", NULL);