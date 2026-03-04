# Crypto Benchmark Integration Guide

- [Adding the framework to a project](#adding-the-framework-to-a-project)
  - [Integration via the West meta build system](#integration-via-the-west-meta-build-system)
- [Coding the Port Layer](#coding-the-port-layer)
  - [Initialization and port layer context](#initialization-and-port-layer-context)
  - [Wrappers](#wrappers)

Some devices will have out-of-the-box SDK generator support for generating a standalone benchmarking project. To check which boards have support, please refer to the `examples_int/src/demo_apps/crypto_benchmark/example.yml` YAML file. To generate a CB project for one of these boards, use the west build tool, e.g.:
```
west build -b evkmimxrt1180 examples_int/src/demo_apps/crypto_benchmark -Dcore_id=cm33 -t guiproject --toolchain iar
```

The rest of this guide is for integrating the framework into your own project in case the device you are benchmarking is not supported by default.

A prerequisite for this guide is having a working base project, to which we can add (or which already contains) a cryptographic layer that is going to be benchmarked.

**Note: Currently the framework is supported on IAR due to a compiler-specific method of variable placement at absolute addresses. If this functionality is not needed, it can be removed when using different compilers. This will be reworked to support other compilers as well.**

## Adding the framework to a project
### Integration via the West meta build system
This method is the simplest when working in the SDK repository. Currently the framework may still needs some additional setup even when using the West build tool.

As an example, we'll use:
 - **LPCExpresso55s69** as the board,
 - **hello_world** project as a base for the project,
 - **MbedTLS 2.x** as the cryptographic layer we'd like to benchmark,
 - and we will add the Crypto Benchmark framework for doing the benchmarking itself.

First, generate the hello_world base project with
```
west build -b lpcxpresso55s69 examples/src/demo_apps/hello_world -Dcore_id=cm33_core0 -t guiproject --toolchain iar
```
then add the required components to the project with the command
```
west build -t guiconfig
```
The components to select are
 - `Middleware -> mbedtls -> Component middleware.mbedtls`
 - `Component Configuration -> Security -> Crypto Benchmark -> Component component.crypto_benchmark.custom`

The "custom" Crypto Benchmark port component is prepared as a template for implementing wrappers, crypto initialization functions and benchmarking configuration.

Lastly, rebuild the IAR GUI project with
```
west build -t guiproject
```
When attempting to build the project there will be redefinition errors for
 - the `main()` function from the hello_world example,
 - the `CRYPTO_InitHardware()` function from the Crypto Benchmark project,
both of which may be removed (the correct `CRYPTO_InitHardware()` from MbedTLS will be used).

What the port layer files should contain is discussed in section [Coding the Port Layer](#coding-the-port-layer)

## Coding the Port Layer
### Benchmarking cases
Benchmarking cases are defined in modules in the `cases/` directory. Please refer to the [crypto_benchmark_overview.md](crypto_benchmark_overview.md) document for further information.

### Initialization and port layer context
In case the crypto layer requires special initialization, the user must provide everything necessary for making the cryptographic layer work. The main crypto-layer initialization is performed by the `CRYPTO_InitHardware()` function. In case this function is already defined in a middleware (e.g. MbedTLS), there is no need to redefine it. This function can be used to initialize the hardware, any crypto services, keystores or anything else that needs special attention before utilizing the crypto layer. For a reference implementation, see the ELE S400 port layer.

If the port layer requires additional context, this can be handled by a global context - generally for opaque service IDs or state values.

### Wrappers
The port layer wrappers can be implemented in a single file (the custom port layer provides the `wrappers.c` file) or they can be separated into separate implementation files. In case of the latter, the project will need to be updated to include the new implementation files.

In order to correctly implement the wrappers, the user should consult the wrapper declaration docstrings in the `include/` directory. Each benchmark category has multiple wrappers declared, where there is always at least 3 main wrappers, which are explained below. Other wrappers may be needed, especially for opaque crypto benchmarking or keygen benchmarking. All of these should be implementable based on the provided docstrings.

#### `wrapper_*_init()`
This is the initialization step that is called before benchmarking a crypto computation. In order to minimize the amount of overhead in the compute wrapper, this initialization step should prepare everything that is needed when calling the actual computation API. This can be achieved by using an internal context, which can be declared as required for any port layer. Examples for the context usage could be enum value translations, parameter preparation, or anything else, that can be factored out from handling it in the benchmarked computation function.

Below is an example of a hash context and a `wrapper_hash_init()` implementation specific for MbedTLS hashing. If we'd like to support multiple types of hashes, we need to handle translating the Crypto Benchmark `cb_hash_t` enum values to a format that is directly usable by MbedTLS. For this reason, the context contains a callback declaration for the MbedTLS SHA functions and a SHA type modifier. These are prepared in advance in the opaque context we declared, so that the translation overhead is not benchmarked in `wrapper_hash_compute()`.
```C
typedef struct _cb_mbedtls_hash_ctx
{
    int is224_or_is384;
    int (*mbedtls_hash_callback)(const unsigned char *, size_t, unsigned char *, int);
} cb_mbedtls_hash_ctx;

status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type)
{
    /* Allocate the internal MbedTLS context */
    cb_mbedtls_hash_ctx *ctx =
        (cb_mbedtls_hash_ctx *)malloc(sizeof(cb_mbedtls_hash_ctx));

    /* Based on hash type, set up the correct function to call.
     * Done to reduce overhead in the cumpute wrapper.
     */
    switch (hash_type)
    {
        case CB_HASH_TYPE_SHA224:
            ctx->is224_or_is384 = 1;
            ctx->mbedtls_hash_callback = mbedtls_sha256_ret;
            break;
        case CB_HASH_TYPE_SHA256:
            ctx->is224_or_is384 = 0;
            ctx->mbedtls_hash_callback = mbedtls_sha256_ret;
            break;
        case CB_HASH_TYPE_SHA384:
            ctx->is224_or_is384 = 1;
            ctx->mbedtls_hash_callback = mbedtls_sha512_ret;
            break;
        case CB_HASH_TYPE_SHA512:
            ctx->is224_or_is384 = 0;
            ctx->mbedtls_hash_callback = mbedtls_sha512_ret;
            break;
        default:
            break;
    }

    /* Save the context */
    *ctx_internal = (void *)ctx;
    return kStatus_Success;
}
```

#### `wrapper_*_compute()`
This is the benchmarked function during a benchmarking run. In order to get performance values as close to the actual performance of the called API, there should be as little overhead between calling the wrapper and calling the crypto API itself.

Below is an example of the MbedTLS port layer wrapper for hashes. We use the port context, that was set up during the previous initialization step, to call the correct API with the correct arguments. We do not worry about translating the return value - MbedTLS return values and `status_t` both define success as `0`. As we only need to know if the API succeeded, this is good enough for our usecase.
```C
status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message,
                              size_t message_size, uint8_t *hash)
{
    cb_mbedtls_hash_ctx *ctx = (cb_mbedtls_hash_ctx *)ctx_internal;

    return ctx->mbedtls_hash_callback(message, message_size, hash, ctx->is224_or_is384);
}
```

#### `wrapper_*_deinit()`
This is a cleanup routine called after the benchmarking of the current case is finished. Used to free the context and for any other usecase. An example of the deinit function:
```C
status_t wrapper_hash_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        free(ctx_internal);
        ctx_internal = NULL;
    }

    return kStatus_Success;
}
```


