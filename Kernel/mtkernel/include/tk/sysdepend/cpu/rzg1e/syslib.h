/* RZ/G1E system library definitions. */
#ifndef __TK_SYSDEPEND_CPU_RZG1E_SYSLIB_H__
#define __TK_SYSDEPEND_CPU_RZG1E_SYSLIB_H__

#include "../core/armv7a/syslib.h"

#define IM_LEVEL 0x00U
#define IM_EDGE  0x01U

Inline void out_w(UW port, UW data) { *(_UW *)port = data; }
Inline void out_h(UW port, UH data) { *(_UH *)port = data; }
Inline void out_b(UW port, UB data) { *(_UB *)port = data; }

Inline UW in_w(UW port) { return *(_UW *)port; }
Inline UH in_h(UW port) { return *(_UH *)port; }
Inline UB in_b(UW port) { return *(_UB *)port; }

Inline void and_w(UW port, UW data) { *(_UW *)port &= data; }
Inline void and_h(UW port, UH data) { *(_UH *)port &= data; }
Inline void and_b(UW port, UB data) { *(_UB *)port &= data; }

Inline void or_w(UW port, UW data) { *(_UW *)port |= data; }
Inline void or_h(UW port, UH data) { *(_UH *)port |= data; }
Inline void or_b(UW port, UB data) { *(_UB *)port |= data; }

#endif /* __TK_SYSDEPEND_CPU_RZG1E_SYSLIB_H__ */
