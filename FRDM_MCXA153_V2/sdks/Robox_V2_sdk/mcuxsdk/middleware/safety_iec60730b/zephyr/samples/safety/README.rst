.. zephyr:code-sample:: safety_iec60730b
   :name: Safety IEC 60730-B
   :relevant-api: safety_iec60730b

   Demonstrates NXP Safety IEC 60730-B library functionality for safety-critical applications.

Overview
********

This sample demonstrates the use of the Safety IEC 60730-B library within the Zephyr RTOS environment. 
The IEC 60730-B standard defines safety requirements for automatic electrical controls used in household 
appliances and similar equipment.

The sample implements various safety tests including:

* CPU register tests
* Program Counter (PC) tests  
* Interrupt handling tests
* RAM tests (March algorithms)
* Flash/ROM tests (CRC/Checksum)
* Clock frequency tests
* Analog-to-Digital Converter (ADC) tests
* Digital I/O tests

These tests help ensure the microcontroller operates correctly and can detect hardware failures that 
could compromise safety in appliances and industrial control systems.

Requirements
************

Your board must:

* Be supported by the Safety IEC 60730-B library
* Support the required peripherals (ADC, timers, GPIO) for comprehensive testing

Building and Running
********************

Build and flash safety_iec60730b as follows, changing ``frdm_rw612`` for your board:

.. zephyr-app-commands::
   :zephyr-app: modules/safety/iec60730b/zephyr/samples/safety
   :board: frdm_rw612
   :goals: build flash
   :compact:

After flashing, the application will:

1. Execute startup safety tests (CPU, PC, Flash/ROM)
2. Begin periodic runtime safety tests in the safety thread.
3. Start an user application.
4. Indicate safety status through blinking LED indicator (if available).

Sample Output
*************

The console output will show the safety test execution status:

.. code-block:: console

   *** Booting Zephyr OS build v4.2.0-1468-gbfdab166e3d4 ***
   [00:00:00.018,355] <inf> safety: Task WDT channel 0 added with timeout 10000 ms
   [00:00:00.026,125] <inf> safety: Executing CPU Registers tests
   [00:00:00.032,575] <inf> safety: Safety test thread started automatically
   [00:00:00.039,811] <inf> safety: Executing CPU Registers tests
   LED state: OFF
   [00:00:01.046,357] <inf> safety: Executing CPU Registers tests
   LED state: ON
   [00:00:02.052,856] <inf> safety: Executing CPU Registers tests
   LED state: OFF
   [00:00:03.059,356] <inf> safety: Executing CPU Registers tests
   LED state: ON
   [00:00:04.065,856] <inf> safety: Executing CPU Registers tests
   LED state: OFF
   [00:00:05.072,356] <inf> safety: Executing CPU Registers tests
   LED state: ON
   [00:00:06.078,856] <inf> safety: Executing CPU Registers tests
   LED state: OFF
   [00:00:07.085,356] <inf> safety: Executing CPU Registers tests
   LED state: ON
   [00:00:08.091,856] <inf> safety: Executing CPU Registers tests
   LED state: OFF
   [00:00:09.098,356] <inf> safety: Executing CPU Registers tests
   LED state: ON
   [00:00:10.104,856] <inf> safety: Executing CPU Registers tests
   LED state: OFF
   [00:00:11.111,356] <inf> safety: Executing CPU Registers tests
   LED state: ON


Safety Considerations
*********************

When using this sample in safety-critical applications:

* Configure appropriate test intervals based on your safety requirements
* Implement proper error handling and safe state transitions
* Consider certification requirements for your specific application domain
* Validate the complete system including this safety library implementation

For production use, consult the NXP Safety IEC 60730-B library documentation and
consider engaging with safety certification experts.

References
**********

* `IEC 60730-1 Standard <https://webstore.iec.ch/publication/3154>`_
* `NXP Safety IEC 60730-B Library Documentation`
* `Zephyr Safety Documentation <https://docs.zephyrproject.org/latest/safety/>`_
