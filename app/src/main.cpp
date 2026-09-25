#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "our_driver.h"

//#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led2)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

namespace {
    void test_driver()
    {
        const struct device *driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

        if (!device_is_ready(driver)) {
            LOG_ERR("Our driver is not ready");
            return;
        }

        int ret = sensor_sample_fetch(driver);
        LOG_INF("sample_fetch returned %d", ret);

        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        struct sensor_value value;
        ret = sensor_channel_get(driver, SENSOR_CHAN_AMBIENT_TEMP, &value);
        LOG_INF("channel_get returned %d", ret);

        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        ret = our_driver_toggle(driver);
        LOG_INF("our_driver_toggle returned %d", ret);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
}

int main(void)
{

    while (1) {
        test_driver();
    }
    return 0;
}