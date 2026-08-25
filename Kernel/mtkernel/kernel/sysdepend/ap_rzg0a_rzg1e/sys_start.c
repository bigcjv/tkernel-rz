/* IAR C runtime entry shim for the AP-RZG-0A target. */
#include <sys/machine.h>
#ifdef AP_RZG0A_RZG1E

#include "kernel.h"

EXPORT void *knl_lowmem_top;
EXPORT void *knl_lowmem_limit;

#if USE_DEBUG_SYSMEMINFO
EXPORT void *knl_sysmem_top;
EXPORT void *knl_sysmem_end;
#endif

#if USE_STATIC_SYS_MEM
Section(EXPORT UW knl_system_mem[SYSTEM_MEM_SIZE / sizeof(UW)], MTK_SYS_MEM);
#endif

IMPORT INT knl_main(void);

EXPORT INT main(void)
{
	knl_startup_hw();

#if USE_STATIC_SYS_MEM
	knl_lowmem_top = knl_system_mem;
	knl_lowmem_limit = &knl_system_mem[SYSTEM_MEM_SIZE / sizeof(UW)];
#else
#error "AP-RZG-0A first bring-up requires USE_STATIC_SYS_MEM"
#endif

#if USE_DEBUG_SYSMEMINFO
	knl_sysmem_top = knl_lowmem_top;
	knl_sysmem_end = knl_lowmem_limit;
#endif

	return knl_main();
}

#endif
