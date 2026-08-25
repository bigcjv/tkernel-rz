/* Busy waits based on the ARM generic physical counter. */
#include <sys/machine.h>
#ifdef CPU_RZG1E

#include <tk/tkernel.h>

IMPORT UD rzg1e_gtimer_counter(void);

LOCAL void wait_nsec(UD nsec)
{
	UD start;
	UD counts;

	counts = (nsec * (UD)COUNT_PER_SEC + 999999999ULL) / 1000000000ULL;
	start = rzg1e_gtimer_counter();
	while ((rzg1e_gtimer_counter() - start) < counts) {
		;
	}
}

EXPORT void WaitUsec(UW usec) { wait_nsec((UD)usec * 1000ULL); }
EXPORT void WaitNsec(UW nsec) { wait_nsec((UD)nsec); }

#endif
