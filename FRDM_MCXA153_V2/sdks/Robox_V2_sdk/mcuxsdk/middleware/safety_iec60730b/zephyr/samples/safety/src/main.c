/*
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 *
 * This demo application:
 * - Configuring an LED as output and toggling it periodically.
 * - Configuring a button as input with interrupt handling.
 * - Printing status messages when button is pressed and LED state changes.
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* Get led configuration from the devicetree led0 alias. */
#define LED0_NODE DT_ALIAS(led0)
#if !DT_NODE_HAS_STATUS_OKAY(LED0_NODE)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Get button configuration from the devicetree sw0 alias. */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback button_cb_data;


/* GPIO interrupt callback function for button press events. */
static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printf("Button pressed.\n");
}

/* System initialization hook for GPIO initialization, to ensure that pins are initialized before safety runtime tests start */
static int main_task_gpio_init(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		printk("Error: led device %s is not ready\n", led.port->name);
		return 0;
	}
	if (!gpio_is_ready_dt(&button)) {
		printk("Error: button device %s is not ready\n", button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error %d: failed to configure %s pin %d\n", ret, led.port->name, led.pin);
		return 0;
	}
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n", ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button.port->name, button.pin);
	printk("Set up led at %s pin %d\n", led.port->name, led.pin);

	return 0;
}
SYS_INIT(main_task_gpio_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

int main(void)
{
	int ret;
	bool led_state = true;

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}

		led_state = !led_state;
		printk("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
