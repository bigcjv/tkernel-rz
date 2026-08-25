/* AP-RZG-0A CPU0 hardware initialization used by micro T-Kernel. */
#include <sys/machine.h>
#ifdef AP_RZG0A_RZG1E

#include "kernel.h"
#include "sysdepend.h"

#define PFC_PMMR       (RZG1E_PFC_BASE + 0x0000UL)
#define PFC_GPSR3      (RZG1E_PFC_BASE + 0x0010UL)
#define PFC_GPSR6      (RZG1E_PFC_BASE + 0x001CUL)
#define PFC_IPSR8      (RZG1E_PFC_BASE + 0x0040UL)
#define PFC_MOD_SEL3   (RZG1E_PFC_BASE + 0x0098UL)
#define PFC_PUPR3      (RZG1E_PFC_BASE + 0x010CUL)

#define GPIO_IOINTSEL  (RZG1E_GPIO6_BASE + 0x0000UL)
#define GPIO_INOUTSEL  (RZG1E_GPIO6_BASE + 0x0004UL)
#define GPIO_POSNEG    (RZG1E_GPIO6_BASE + 0x0020UL)
#define GPIO_OUTDTSEL  (RZG1E_GPIO6_BASE + 0x0040UL)

#define SYSCNT_ENABLE  0x00000001UL
#define SYSCNT_HDBG    0x00000002UL

IMPORT void _kernel_synch_cache(void);

LOCAL void pfc_write(UW address, UW value)
{
	out_w(PFC_PMMR, ~value);
	out_w(address, value);
}

LOCAL void setup_uart0_pins(void)
{
	UW value;

	value = in_w(PFC_GPSR3) & ~(0x3UL << 27);
	pfc_write(PFC_GPSR3, value);

	value = in_w(PFC_MOD_SEL3);
	value = (value & ~(0x3UL << 30)) | (0x3UL << 30);
	pfc_write(PFC_MOD_SEL3, value);

	value = in_w(PFC_IPSR8);
	value = (value & ~(0x3FUL << 9)) | (0x12UL << 9);
	pfc_write(PFC_IPSR8, value);

	value = in_w(PFC_GPSR3) | (0x3UL << 27);
	pfc_write(PFC_GPSR3, value);

	out_w(PFC_PUPR3, in_w(PFC_PUPR3) & ~0x02000000UL);
	_kernel_synch_cache();
}

LOCAL void setup_led_pins(void)
{
	UW value;

	value = in_w(PFC_GPSR6) & ~(0x3UL << 24);
	pfc_write(PFC_GPSR6, value);

	out_w(GPIO_POSNEG, in_w(GPIO_POSNEG) & ~(0x3UL << 24));
	out_w(GPIO_IOINTSEL, in_w(GPIO_IOINTSEL) & ~(0x3UL << 24));
	out_w(GPIO_INOUTSEL, in_w(GPIO_INOUTSEL) | (0x3UL << 24));
	out_w(GPIO_OUTDTSEL, in_w(GPIO_OUTDTSEL) | (0x3UL << 24));
	_kernel_synch_cache();
}

LOCAL void start_system_counter(void)
{
	out_w(RZG1E_SYSCNT_CNTFID0, (UW)COUNT_PER_SEC);
	out_w(RZG1E_SYSCNT_CNTCR, SYSCNT_ENABLE | SYSCNT_HDBG);
	_kernel_synch_cache();
}

EXPORT void knl_startup_hw(void)
{
	startup_clock();
	setup_uart0_pins();
	setup_led_pins();
	start_system_counter();
}

#if USE_SHUTDOWN
EXPORT void knl_shutdown_hw(void)
{
	disint();
	shutdown_clock();
	for (;;) {
		;
	}
}
#endif

EXPORT ER knl_restart_hw(W mode)
{
	(void)mode;
	return E_NOSPT;
}

#endif
