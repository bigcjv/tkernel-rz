#ifndef _SYSDEPEND_CPU_RZG1E_SYS_TIMER_H_
#define _SYSDEPEND_CPU_RZG1E_SYS_TIMER_H_

IMPORT void rzg1e_gtimer_start(UW period_count);
IMPORT void rzg1e_gtimer_reload(UW period_count);
IMPORT void rzg1e_gtimer_stop(void);
IMPORT W rzg1e_gtimer_value(void);

#define RZG1E_TIMER_RELOAD \
	((UW)(((UD)TIMER_PERIOD * (UD)COUNT_PER_SEC) / 1000ULL))

Inline void knl_start_hw_timer(void)
{
	UINT imask;

	DI(imask);
	rzg1e_gtimer_start(RZG1E_TIMER_RELOAD);
	EnableInt(INTNO_SYSTICK, INTPRI_SYSTICK);
	EI(imask);
}

Inline void knl_clear_hw_timer_interrupt(void)
{
	rzg1e_gtimer_reload(RZG1E_TIMER_RELOAD);
}

Inline void knl_end_of_hw_timer_interrupt(void)
{
	disint();
	EndOfInt(INTNO_SYSTICK);
}

Inline void knl_terminate_hw_timer(void)
{
	rzg1e_gtimer_stop();
	DisableInt(INTNO_SYSTICK);
}

Inline UW knl_get_hw_timer_nsec(void)
{
	W remain;
	UW elapsed;

	remain = rzg1e_gtimer_value();
	if (remain <= 0) {
		return (UW)TIMER_PERIOD * 1000000UL;
	}

	elapsed = RZG1E_TIMER_RELOAD - (UW)remain;
	return (UW)(((UD)elapsed * 1000000000ULL) / (UD)COUNT_PER_SEC);
}

#endif
