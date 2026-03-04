
if(CONFIG_MCUX_COMPONENT_middleware.safety)
    mcux_component_version(5.0)

    mcux_add_include(
        INCLUDES source/core_test/cm0/register
				 source/core_test/cm0/ram
				 source/core_test/cm0/flash
				 source/core_test/cm0/stack
				 source/core_test/cm0/programCounter
				 source/core_test/cm0
		CORES cm0p
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )
    mcux_add_include(
        INCLUDES source/core_test/cm4_cm7/register
				 source/core_test/cm4_cm7/ram
				 source/core_test/cm4_cm7/flash
				 source/core_test/cm4_cm7/stack
				 source/core_test/cm4_cm7/programCounter
				 source/core_test/cm4_cm7
		CORES cm4 cm4f cm7 cm7f
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )	
    mcux_add_include(
        INCLUDES source/core_test/cm33/register
				 source/core_test/cm33/ram
				 source/core_test/cm33/flash
				 source/core_test/cm33/stack
				 source/core_test/cm33/programCounter
				 source/core_test/cm33
		CORES cm33
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )	  
    mcux_add_include(
        INCLUDES source
				 source/compiler
				 source/common_test/wdog
				 source/common_test/tsi
				 source/common_test/dio
				 source/common_test/clock
				 source/common_test/flash
				 source/common_test/aio
		CORES cm4 cm4f cm7 cm7f cm0p cm33
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )
	
	# as-include
    mcux_add_asm_include(
        INCLUDES source/core_test/cm0/register
				 source/core_test/cm0/ram
				 source/core_test/cm0/flash
				 source/core_test/cm0/stack
				 source/core_test/cm0/programCounter
				 source/core_test/cm0
		CORES cm0p
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )
    mcux_add_asm_include(
        INCLUDES source/core_test/cm4_cm7/register
				 source/core_test/cm4_cm7/ram
				 source/core_test/cm4_cm7/flash
				 source/core_test/cm4_cm7/stack
				 source/core_test/cm4_cm7/programCounter
				 source/core_test/cm4_cm7
		CORES cm4 cm4f cm7 cm7f
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )	
    mcux_add_asm_include(
        INCLUDES source/core_test/cm33/register
				 source/core_test/cm33/ram
				 source/core_test/cm33/flash
				 source/core_test/cm33/stack
				 source/core_test/cm33/programCounter
				 source/core_test/cm33
		CORES cm33
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )	  
    mcux_add_asm_include(
        INCLUDES safety
				 source/compiler
				 source/common_test/wdog
				 source/common_test/tsi
				 source/common_test/dio
				 source/common_test/clock
				 source/common_test/aio
		CORES cm4 cm4f cm7 cm7f cm0p cm33
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
    )
	
	mcux_add_source(
		SOURCES source/core_test/cm0/register/iec60730b_cm0_reg.h
				source/core_test/cm0/register/iec60730b_cm0_reg.S
				source/core_test/cm0/ram/iec60730b_cm0_ram.h
				source/core_test/cm0/ram/iec60730b_cm0_ram.S
				source/core_test/cm0/flash/iec60730b_cm0_flash.h
				source/core_test/cm0/flash/iec60730b_cm0_flash.S
				source/core_test/cm0/stack/iec60730b_cm0_stack.h
				source/core_test/cm0/stack/iec60730b_cm0_stack.S
				source/core_test/cm0/programCounter/iec60730b_cm0_pc.h
				source/core_test/cm0/programCounter/iec60730b_cm0_pc.S
				source/core_test/cm0/programCounter/iec60730b_cm0_pc_object.S
				source/core_test/cm0/iec60730b_core.h
		CORES cm0p
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)

	mcux_add_source(
		SOURCES source/core_test/cm4_cm7/register/iec60730b_cm4_cm7_reg.h
				source/core_test/cm4_cm7/register/iec60730b_cm4_cm7_reg.S
				source/core_test/cm4_cm7/register/iec60730b_cm4_cm7_reg_fpu.S
				source/core_test/cm4_cm7/ram/iec60730b_cm4_cm7_ram.h
				source/core_test/cm4_cm7/ram/iec60730b_cm4_cm7_ram.S
				source/core_test/cm4_cm7/flash/iec60730b_cm4_cm7_flash.h
				source/core_test/cm4_cm7/flash/iec60730b_cm4_cm7_flash.S
				source/core_test/cm4_cm7/stack/iec60730b_cm4_cm7_stack.h
				source/core_test/cm4_cm7/stack/iec60730b_cm4_cm7_stack.S
				source/core_test/cm4_cm7/programCounter/iec60730b_cm4_cm7_pc.h
				source/core_test/cm4_cm7/programCounter/iec60730b_cm4_cm7_pc.S
				source/core_test/cm4_cm7/programCounter/iec60730b_cm4_cm7_pc_object.S
				source/core_test/cm4_cm7/iec60730b_core.h
		CORES cm4 cm4f cm7 cm7f
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)

	mcux_add_source(
		SOURCES source/core_test/cm33/register/iec60730b_cm33_reg.h
				source/core_test/cm33/register/iec60730b_cm33_reg.S
				source/core_test/cm33/ram/iec60730b_cm33_ram.h
				source/core_test/cm33/ram/iec60730b_cm33_ram.S
				source/core_test/cm33/flash/iec60730b_cm33_flash.h
				source/core_test/cm33/flash/iec60730b_cm33_flash.S
				source/core_test/cm33/stack/iec60730b_cm33_stack.h
				source/core_test/cm33/stack/iec60730b_cm33_stack.S
				source/core_test/cm33/programCounter/iec60730b_cm33_pc.h
				source/core_test/cm33/programCounter/iec60730b_cm33_pc.S
				source/core_test/cm33/programCounter/iec60730b_cm33_pc_object.S
				source/core_test/cm33/iec60730b_core.h
		CORES cm33
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)
	
	mcux_add_source(
		SOURCES source/core_test/cm33/register/iec60730b_cm33_reg_fpu.S
		CORES cm33
		FPU SP_FPU DP_FPU
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)
	
	mcux_add_source(
		SOURCES source/core_test/cm33/register/iec60730b_cm33_reg_dsp_ext.S
		CORES cm33
		DSP HAS_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)
	
	mcux_add_source(
		SOURCES source/core_test/cm33/register/iec60730b_cm33_reg_dsp_fpu.h
		CORES cm33
		FPU SP_FPU DP_FPU
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)

	mcux_add_source(
		SOURCES source/iec60730b.h
				source/iec60730b_types.h
				source/compiler/asm_mac_common.h
				source/common_test/wdog/iec60730b_wdog.c
				source/common_test/wdog/iec60730b_wdog.h
				source/common_test/tsi/iec60730b_tsi.h
				source/common_test/tsi/iec60730b_tsi.c
				source/common_test/dio/iec60730b_dio_ext.h
				source/common_test/dio/iec60730b_dio_ext.c
				source/common_test/dio/iec60730b_dio.h
				source/common_test/dio/iec60730b_dio.c
				source/common_test/clock/iec60730b_clock.h
				source/common_test/clock/iec60730b_clock.c
				source/common_test/aio/iec60730b_aio.h
				source/common_test/aio/iec60730b_aio.c
				source/common_test/flash/iec60730b_invariable_memory.h
				source/common_test/flash/iec60730b_invariable_memory.c
		CORES cm4 cm4f cm7 cm7f cm0p cm33
        BASE_PATH ${SdkRootDirPath}/middleware/safety_iec60730b/
	)
	
endif()

# Needs to be defined to satisfy project dependency
if (CONFIG_MCUX_PRJSEG_middleware.safety.generated_config)
endif ()