/* Polling T-Monitor console on AP-RZG-0A SCIF0. */
#include <tk/tkernel.h>

#if USE_TMONITOR
#include "../../libtm.h"

#if defined(AP_RZG0A_RZG1E) && TM_COM_SERIAL_DEV

#define SCIF_SMR  (RZG1E_SCIF0_BASE + 0x00UL)
#define SCIF_BRR  (RZG1E_SCIF0_BASE + 0x04UL)
#define SCIF_SCR  (RZG1E_SCIF0_BASE + 0x08UL)
#define SCIF_FTDR (RZG1E_SCIF0_BASE + 0x0CUL)
#define SCIF_FSR  (RZG1E_SCIF0_BASE + 0x10UL)
#define SCIF_FRDR (RZG1E_SCIF0_BASE + 0x14UL)
#define SCIF_FCR  (RZG1E_SCIF0_BASE + 0x18UL)
#define SCIF_SPTR (RZG1E_SCIF0_BASE + 0x20UL)
#define SCIF_LSR  (RZG1E_SCIF0_BASE + 0x24UL)

#define SCR_TE    0x0020U
#define SCR_RE    0x0010U
#define FSR_ER    0x0080U
#define FSR_TEND  0x0040U
#define FSR_TDFE  0x0020U
#define FSR_BRK   0x0010U
#define FSR_FER   0x0008U
#define FSR_PER   0x0004U
#define FSR_RDF   0x0002U
#define FSR_DR    0x0001U
#define FSR_ERROR (FSR_ER | FSR_BRK | FSR_FER | FSR_PER)
#define FCR_TFRST 0x0004U
#define FCR_RFRST 0x0002U
#define LSR_ORER  0x0001U

EXPORT void tm_snd_dat(const UB *buf, INT size)
{
	while (size-- > 0) {
		while ((in_h(SCIF_FSR) & FSR_TDFE) == 0U) { ; }
		out_b(SCIF_FTDR, *buf++);
		and_h(SCIF_FSR, (UH)~(FSR_TEND | FSR_TDFE));
	}
	while ((in_h(SCIF_FSR) & FSR_TEND) == 0U) { ; }
}

EXPORT void tm_rcv_dat(UB *buf, INT size)
{
	while (size-- > 0) {
		while ((in_h(SCIF_FSR) & (FSR_RDF | FSR_DR)) == 0U) {
			if (((in_h(SCIF_FSR) & FSR_ERROR) != 0U)
		    || ((in_h(SCIF_LSR) & LSR_ORER) != 0U)) {
				and_h(SCIF_SCR, (UH)~SCR_RE);
				or_h(SCIF_FCR, FCR_RFRST);
				and_h(SCIF_FCR, (UH)~FCR_RFRST);
				and_h(SCIF_FSR, (UH)~FSR_ERROR);
				and_h(SCIF_LSR, (UH)~LSR_ORER);
				or_h(SCIF_SCR, SCR_RE);
			}
		}
		*buf++ = in_b(SCIF_FRDR);
		and_h(SCIF_FSR, (UH)~(FSR_RDF | FSR_DR));
	}
}

EXPORT void tm_com_init(void)
{
	out_h(SCIF_SCR, 0U);
	out_h(SCIF_FCR, FCR_RFRST | FCR_TFRST);
	and_h(SCIF_FSR, (UH)~(FSR_ER | FSR_BRK | FSR_DR));
	and_h(SCIF_LSR, (UH)~LSR_ORER);
	out_h(SCIF_SMR, 0U);
	out_b(SCIF_BRR, 17U);
	out_h(SCIF_FCR, 0x0030U);
	or_h(SCIF_SPTR, 0x00C3U);
	out_h(SCIF_SCR, SCR_TE | SCR_RE);
}

#endif
#endif
