/* AP-RZG-0A device lifecycle hooks. */
#include <sys/machine.h>
#ifdef AP_RZG0A_RZG1E

#include "kernel.h"

EXPORT ER knl_init_device(void) { return E_OK; }
EXPORT ER knl_start_device(void) { return E_OK; }

#if USE_SHUTDOWN
EXPORT ER knl_finish_device(void) { return E_OK; }
#endif

#endif
