/* RZ/G1E GIC-400 initialization and micro T-Kernel vector tables. */
#include <sys/machine.h>
#ifdef CPU_RZG1E

#include <kernel.h>
#include "../../sysdepend.h"

Noinit(EXPORT FP knl_intvec_tbl[N_INTVEC]);
Noinit(EXPORT FP knl_hll_inthdr_tbl[N_INTVEC]);

EXPORT const FP knl_svcvec_tbl[N_SVCHDR] = {
	NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,
	knl_dispatch_to_schedtsk,
	knl_dispatch_entry,
	NULL,
	NULL
};

EXPORT ER knl_define_inthdr(INT intno, ATR intatr, FP inthdr)
{
	if ((inthdr != NULL) && ((intatr & TA_HLNG) != 0U)) {
		knl_hll_inthdr_tbl[intno] = inthdr;
		inthdr = knl_hll_inthdr;
	}
	knl_intvec_tbl[intno] = inthdr;
	return E_OK;
}

EXPORT ER knl_init_interrupt(void)
{
	INT i;
	_UW *reg;

	out_w(GICD_CTLR, 0U);
	out_w(GICC_CTLR, 0U);

	for (i = 0; i < N_INTVEC; i++) {
		knl_intvec_tbl[i] = (FP)NULL;
		knl_hll_inthdr_tbl[i] = (FP)NULL;
	}

	reg = (_UW *)GICD_ICENABLER(0);
	for (i = 0; i < GICD_ICENABLER_N; i++) {
		reg[i] = 0xFFFFFFFFUL;
	}
	reg = (_UW *)GICD_ICPENDR(0);
	for (i = 0; i < GICD_ICENABLER_N; i++) {
		reg[i] = 0xFFFFFFFFUL;
	}
	reg = (_UW *)GICD_IGROUPR(0);
	for (i = 0; i < GICD_IGROUPR_N; i++) {
		reg[i] = 0x00000000UL;
	}
	reg = (_UW *)GICD_ICFGR(0);
	for (i = 2; i < GICD_ICFGR_N; i++) {
		reg[i] = 0x55555555UL;
	}
	reg = (_UW *)GICD_IPRIORITYR(0);
	for (i = 0; i < GICD_IPRIORITYR_N; i++) {
		reg[i] = 0xF8F8F8F8UL;
	}
	reg = (_UW *)GICD_ITARGETR(0);
	for (i = 8; i < GICD_ITARGETR_N; i++) {
		reg[i] = 0x01010101UL;
	}

	out_w(GICC_PMR, 0xF8U);
	out_w(GICC_BPR, 2U);
	out_w(GICC_CTLR, 3U);
	out_w(GICD_CTLR, 1U);

	knl_define_inthdr(INTNO_SYSTICK, TA_HLNG, (FP)knl_timer_handler);
	return E_OK;
}

#endif
