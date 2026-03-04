# Crypto Benchmark Overview

- [Common Layer](#common-layer)
- [Port Layer](#port-layer)
  - [Wrappers](#wrappers)
  - [Benchmarking cases](#benchmarking-cases)

## Common Layer
The common layer takes care of the benchmarking itself. It should be mostly unchanging and should provide a robust base for the port layer. The concrete benchmarks that should be run can be enabled via a two-layer configuration setup. The `cb_common_config.h` header file provides options for the common layer - options for output formats, perfcurve options, etc. On the other hand, `cases/` directory provides a collection of modules defining lists of benchmark cases for platform-specific benchmarking - these modules are described in the [Port Layer](#benchmarking-cases) section.

The common layer is divided into two main sections denoted by the `opaque/` and `transparent/` directories. This division is based on the opacity of the keys used in the given benchmarks. For **opaque** key benchmarks, additional setup wrappers are provided in order to facilitate the port layer's ability to prepare and clean up any opaque services that may be required for running the benchmark.
With **transparent** keys, no opaque service setup wrappers are provided. Instead, only context initialization wrappers are used.

## Port Layer
This layer is provided by the user. It is required of the port layer to define all necessary `wrapper_*()` functions for benchmarking to work correctly. The documented wrapper declarations for individual benchmarks are present in the declaration headers, which can be found in the `include/` directory of the project.

### Wrappers
As mentioned above, all wrappers that are expected to be defined by the port layer, should be documented in the declaration headers. Here a more abstract view of the wrappers is provided.

In general each benchmarking case flows like this: `wrapper_*_init()` -> `wrapper_*_compute()` -> `wrapper_*_deinit()`. Opaque benchmarks have additional wrappers at either side of `wrapper_*_compute()` - these are explained further below.

Opaque and transparent benchmarks both declare the `wrapper_*_init()` and `wrapper_*_deinit()` functions:
- `wrapper_*_init()` provides an interface for the port layer to set up the current benchmark context. The context should be port-layer-specific for the given benchmarking category and should be `malloc`'d by this function. This approach makes the port layer opaque to the common layer, so the two layers can be decoupled and independent of each other.
- `wrapper_*_deinit()` provides an interface for the port layer to tear down the current benchmark context. Generally this means that the `malloc`'d context should be `free`'d.
For transparent benchmarks these are all of the setup and teardown functions.

#### Opaque wrappers
For specific benchmarking cases a small set of wrappers are declared. In general these are: `wrapper_*_opaque_setup()`, `wrapper_*_opaque_cleanup()`, `wrapper_*_opaque_key_generate()`, `wrapper_*_opaque_key_delete()`, `wrapper_*_opaque_compute()`.

The thinking behind these is that opaque key operations may require that opaque key services are enabled and disabled before and after each benchmarking case. Additionally, opaque keys need to be generated internally, for which apropriate declarations are provided. If required, these calls may be wrapped in the common layer in one of the benchmarking macros in order gain performance metrics on these operations as well.

## Benchmarking cases
All benchmarking cases are defined in directory `cases/`.
For each cryptographic operation, there is at least one module `{op}_all.c` (where `op` is the name of the operation) that defines the list of all benchmarking cases that are supported by the crypto benchmark.
In case a platform does not support the full set of benchmarking cases for a given cryptographic operation, a specific module `{op}_{id}.c` (where `id` is an explicit string identifying the list) defines a subset of the full list.
The relevant modules **must** be built and linked to the project for a given platform.