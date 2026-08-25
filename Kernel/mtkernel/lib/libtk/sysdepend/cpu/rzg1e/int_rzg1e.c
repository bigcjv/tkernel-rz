/* RZ/G1E CPU and GIC interrupt control. */
#include <sys/machine.h>
#ifdef CPU_RZG1E

#include <tk/tkernel.h>

IMPORT UW rzg1e_disint(void);
IMPORT void rzg1e_enaint(UW intsts);

EXPORT UW disint(void) { return rzg1e_disint(); }
EXPORT void enaint(UW intsts) { rzg1e_enaint(intsts); }

EXPORT void SetCtrlIntLevel(INT level)
{
	if ((level < INTPRI_HIGHEST) || (level >= INTPRI_LOWEST)) return;
	level = ((level + 1) << INTPRI_SHIFT) & 0xFF;
	out_w(GICC_PMR, (UW)level);
}

EXPORT INT GetCtrlIntLevel(void)
{
	INT level = (INT)((in_w(GICC_PMR) & 0xFFU) >> INTPRI_SHIFT);
	return level - 1;
}

EXPORT void EnableInt(UINT intno, INT level)
{
	UW value;
	UW shift;
	UW address;

	if ((intno >= N_INTVEC) || (level < INTPRI_HIGHEST)
	    || (level > INTPRI_LOWEST)) return;

	address = GICD_IPRIORITYR(intno >> 2);
	shift = (intno & 3U) << 3;
	value = in_w(address);
	value &= ~(0xFFUL << shift);
	value |= (((UW)level << INTPRI_SHIFT) & 0xFFUL) << shift;
	out_w(address, value);
	out_w(GICD_ISENABLER(intno >> 5), 1UL << (intno & 31U));
}

EXPORT void DisableInt(UINT intno)
{
	if (intno < N_INTVEC) {
		out_w(GICD_ICENABLER(intno >> 5), 1UL << (intno & 31U));
	}
}

EXPORT void SetIntMode(UINT intno, UINT mode)
{
	UW bit;
	_UW *address;

	if ((intno >= N_INTVEC) || (mode > IM_EDGE)) return;
	address = (_UW *)GICD_ICFGR(intno >> 4);
	bit = 1UL << (((intno & 15U) << 1) + 1U);
	if (mode == IM_LEVEL) *address &= ~bit;
	else *address |= bit;
}

EXPORT void ClearInt(UINT intno)
{
	if (intno < N_INTVEC) {
		out_w(GICD_ICPENDR(intno >> 5), 1UL << (intno & 31U));
	}
}

EXPORT BOOL CheckInt(UINT intno)
{
	if (intno >= N_INTVEC) return FALSE;
	return ((in_w(GICD_ISPENDR(intno >> 5)) & (1UL << (intno & 31U))) != 0U)
		? TRUE : FALSE;
}

EXPORT void EndOfInt(UINT intno)
{
	if (intno < N_INTVEC) out_w(GICC_EOIR, intno);
}

#endif
