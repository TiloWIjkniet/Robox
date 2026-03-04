.. _SafetyIec60730bREADME:
.. Safety iec60730b library module

###################################
Safety IEC60730B library for Zephyr
###################################
* Based on and tested with Zephyr v4.2.0

.. contents::
   :local:
   :depth: 2


******************************************
Safety IEC60730B library environment setup
******************************************

Follow these steps to set up a development environment on your machine.

#. If you haven't already, please follow `this guide <https://docs.zephyrproject.org/latest/develop/getting_started/index.html>`_ to set up a Zephyr development environment and its dependencies first.

#. Get the Safety IEC60730B library. You can pick either of the options listed below.
    * Freestanding Zephyr iec60730b module - pulls in only the dependencies it needs including Zephyr itself. Run::

            west init -m <iec60730b_repository_URL> --mr <iec60730b_revision_or_branch> <folder_name>
            cd <folder_name>
            west update

    * Included Zephyr iec60730b module - if you already have your Zephyr environment set up.
        * To include iec60730b module, update your Zephyr's ``west.yml`` file::

            projects:
            - name: safety_iec60730b
              url: <iec60730b_repository_url>
              revision: <iec60730b_revision_or_branch>
              path: modules/safety/iec60730b

        * Then update module::

            west update safety_iec60730b

#. Use IEC60730B library in you application
    * Enable IEC60730B library in your Zephyr project “prj.conf” file::

        CONFIG_IEC60730B=y

    * Call the Library Zephyr HAL API, or if required, the Bare-Metal API directly, in your application.
      You can use the ``safety`` sample as a reference. 

*******************************
Build and run ``safety`` sample
*******************************

See Zephyr's `Building, Flashing and Debugging <https://docs.zephyrproject.org/latest/develop/west/build-flash-debug.html>`_ guide if you aren't familiar with it yet.

#. To **build** a project, run:
    ::

        west build -b <board> <path to example> -p

    For example, this compiles the iec60730b example for a frdm-rw612 board::

        cd modules/safety/iec60730b/zephyr
        west build -b frdm_rw612 samples/safety -p

#. To **run** a project, run:
    ::

        west flash

****************
Folder structure
****************
::

    iec60730b/
    ├─── ...
    └─── zephyr/                All Zephyr related files.
        ├── samples/            Sample examples.
        ├── src/                Zephyr specific library source code.
        ├── module.yml          Defines module name, Cmake and Kconfig locations.
        ├── CMakeList.txt       Defines module's build process.
        ├── Kconfig             Defines module's configuration.
        └── ...
