#ifndef _SYSDEPEND_CPU_RZG1E_CPU_STATUS_H_
#define _SYSDEPEND_CPU_RZG1E_CPU_STATUS_H_

#include <tk/syslib.h>
#include <sys/sysdef.h>

#include "sysdepend.h"

#define BEGIN_CRITICAL_SECTION { UINT _intsts_ = disint();
#define END_CRITICAL_SECTION \
	if (!isDI(_intsts_) && knl_ctxtsk != knl_schedtsk \
	    && !knl_isTaskIndependent() && !knl_dispatch_disabled) { \
		knl_dispatch(); \
	} \
	enaint(_intsts_); }

#define BEGIN_DISABLE_INTERRUPT { UINT _intsts_ = disint();
#define END_DISABLE_INTERRUPT enaint(_intsts_); }

#define ENABLE_INTERRUPT { enaint(0); }
#define DISABLE_INTERRUPT { disint(); }
#define ENABLE_INTERRUPT_UPTO(level) { enaint(0); }

IMPORT W knl_taskindp;
IMPORT UW rzg1e_get_cpsr(void);
IMPORT void rzg1e_svc_force_dispatch(void);
IMPORT void rzg1e_svc_dispatch(void);

Inline BOOL knl_isTaskIndependent(void)
{
	return (knl_taskindp > 0) ? TRUE : FALSE;
}

Inline void knl_EnterTaskIndependent(void) { knl_taskindp++; }
Inline void knl_LeaveTaskIndependent(void) { knl_taskindp--; }

#define ENTER_TASK_INDEPENDENT { knl_EnterTaskIndependent(); }
#define LEAVE_TASK_INDEPENDENT { knl_LeaveTaskIndependent(); }

#define in_indp() (knl_isTaskIndependent() || knl_ctxtsk == NULL)
#define in_ddsp() (knl_dispatch_disabled || in_indp() \
		|| (rzg1e_get_cpsr() & PSR_I))
#define in_loc() ((rzg1e_get_cpsr() & PSR_I) || in_indp())
#define in_qtsk() (knl_ctxtsk->sysmode > knl_ctxtsk->isysmode)

Inline void knl_force_dispatch(void) { rzg1e_svc_force_dispatch(); }
Inline void knl_dispatch(void) { rzg1e_svc_dispatch(); }

#endif
