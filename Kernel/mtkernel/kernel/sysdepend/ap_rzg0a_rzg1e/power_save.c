/* CPU0 power-save hooks. Keep WFI disabled during initial bring-up. */
#include <sys/machine.h>
#ifdef AP_RZG0A_RZG1E

#include <tk/tkernel.h>

EXPORT void low_pow(void) { }
EXPORT void off_pow(void) { }

#endif
