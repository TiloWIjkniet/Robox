/*
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iec60730b_test.h"
#include <iec60730b.h>
#include <iec60730b_core.h>

#ifdef CONFIG_IEC60730B_TEST_DIO

/* Zephyr GPIO driver configuration */
#ifdef CONFIG_GPIO_MCUX
    /* from zephyr/drivers/gpio/gpio_mcux.c */
    struct gpio_mcux_config {
        struct gpio_driver_config common;
        GPIO_Type *gpio_base;
        PORT_Type *port_base;
        unsigned int flags;
        uint32_t port_no;
    };
#elif defined(CONFIG_GPIO_MCUX_IGPIO)
    /* from zephyr/drivers/gpio/gpio_mcux_igpio.c */
    #define DEV_CFG(_dev) ((const struct mcux_igpio_config *)(_dev)->config)
    struct mcux_igpio_config {
        struct gpio_driver_config common;
        DEVICE_MMIO_NAMED_ROM(igpio_mmio);
        const struct pinctrl_soc_pinmux *pin_muxes;
        uint8_t mux_count;
    };
#elif defined(CONFIG_GPIO_MCUX_LPC)
    /* from zephyr/drivers/gpio/gpio_mcux_lpc.c */
    struct gpio_mcux_lpc_config {
        /* gpio_driver_config needs to be first */
        struct gpio_driver_config common;
        GPIO_Type *gpio_base;
        uint8_t int_source;
    #ifdef IOPCTL
        IOPCTL_Type *pinmux_base;
    #endif
    #ifdef IOCON
        IOCON_Type *pinmux_base;
    #endif
    #ifdef MCI_IO_MUX
        MCI_IO_MUX_Type * pinmux_base;
    #endif
        uint32_t port_no;
        const struct pinctrl_dev_config *pincfg;
    };
#endif

/* Test digital input pin for IEC 60730 Class B compliance */
int iec60730b_test_dio_input(const struct device *port, gpio_pin_t pin, bool pin_expected_value)
{
#ifdef CONFIG_GPIO_MCUX
    fs_dio_test_t tested_pin;

    tested_pin.gpio = (uint32_t)((struct gpio_mcux_config *) port->config)->gpio_base;
    tested_pin.pinNum = pin;

    if (FS_DIO_Input(&tested_pin, (bool_t)pin_expected_value) != FS_PASS) {
        return IEC60730B_TEST_DIO_ERROR;
    }
#else
    return IEC60730B_TEST_NOT_SUPPORTED;
#endif
    return IEC60730B_TEST_OK;
}

/* Test digital output pin for IEC 60730 Class B compliance */
int iec60730b_test_dio_output(const struct device *port, gpio_pin_t pin)
{
    #define DIO_WAIT_CYCLE     75
#ifdef CONFIG_GPIO_MCUX
    fs_dio_test_t tested_pin;

    tested_pin.gpio = (uint32_t)((struct gpio_mcux_config *) port->config)->gpio_base;
    tested_pin.pinNum = pin;

    if (FS_DIO_Output(&tested_pin, DIO_WAIT_CYCLE) != FS_PASS){
        return IEC60730B_TEST_DIO_ERROR;
    }
#elif defined(CONFIG_GPIO_MCUX_IGPIO)
    /* Zephyr driver has a config limitation.
       Disable it until the driver is fixed */
    #if 0 
        fs_dio_test_imx_t tested_pin;

        tested_pin.gpio = (uint32_t)DEVICE_MMIO_NAMED_GET(port, igpio_mmio);
        tested_pin.pinNum = pin;

        if (FS_DIO_Output_IMXRT(&tested_pin, DIO_WAIT_CYCLE) != FS_PASS){
            return IEC60730B_TEST_DIO_ERROR;
        }
    #else
        return IEC60730B_TEST_NOT_SUPPORTED;
    #endif
#elif defined(CONFIG_GPIO_MCUX_LPC)
    fs_dio_test_lpc_t tested_pin;

    const struct gpio_mcux_lpc_config *config = port->config;
    GPIO_Type *gpio_base = config->gpio_base;
    uint32_t port_no = config->port_no;

    tested_pin.pPort_byte = (uint8_t *)&gpio_base->B[port_no][pin];   /* Adress of byte register in GPIO */
    tested_pin.pPort_dir = (uint32_t *)&gpio_base->DIR[port_no];   /* Adress of dir register in GPIO */
    #ifdef IOCON /* LPC SOCs */
    {
        tested_pin.pPort_Iocon =(uint32_t *)&(config->pinmux_base->PIO[port_no][pin]); /* Adress of concrete IOCON register */
	    tested_pin.iocon_digimode = 1;
    }
    #else
    tested_pin.iocon_digimode = 0; 
    #endif
    tested_pin.pinNum = pin;

    if (FS_DIO_Output_LPC(&tested_pin, DIO_WAIT_CYCLE) != FS_PASS){
        return IEC60730B_TEST_DIO_ERROR;
    }
#else
    return IEC60730B_TEST_NOT_SUPPORTED;
#endif
    return IEC60730B_TEST_OK;
}
#endif /* CONFIG_IEC60730B_TEST_DIO */
