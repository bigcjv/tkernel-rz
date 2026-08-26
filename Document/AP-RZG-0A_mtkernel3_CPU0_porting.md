# AP-RZG-0A（RZ/G1E）CPU0 移植 micro T-Kernel 3 详细说明

## 阅读导航

1. 第 1 章：迁移范围、原 uC3 模型、目标模型和难度。
2. 第 3 章：板卡上电、CSide 下载、Reset、内核启动、tick 和调度完整链路。
3. 第 5 章：IAR 汇编、GIC、Generic Timer、MMU/Cache、ICF、PFC、SCIF0 的具体改法。
4. 第 6 章：IAR 工程中删除了哪些 uC3 构建项、加入了哪些 micro T-Kernel 构建组。
5. 第 7～9 章：测试任务、实板步骤和分阶段故障定位。
6. 第 10 章：全部 38 个新增/修改/删除路径及每个文件的职责。

## 1. 目标与范围

本次工作把 `Sample/Standard/RZG1E/AP-RZG-0A.UART` 的 CPU0 运行环境从 uC3 切换为 `Kernel/mtkernel` 中的 micro T-Kernel 3 源码，继续使用 IAR EWARM 编译和 CSide 下载。

当前移植范围如下：

- 只启动 CPU0；CPU1 在启动汇编中进入 `WFE` 等待，不参与内核运行。
- 新增独立的 `ap_rzg0a_rzg1e` 板级目标和 `rzg1e` CPU 目标，不修改 RZA2M 的源文件。
- CPU0 完成异常向量、任务上下文切换、GIC、1 ms 系统节拍、MMU/Cache、SCIF0 T-Monitor 输出和测试任务接入。
- 第一阶段关闭 FPU 上下文切换。
- uC3 代码和旧示例保留，旧 `sample.c` 保存为 `sample_uc3.c`，但不再加入当前 IAR 构建。

本次没有处理 CPU1 启动、SMP、FPU/NEON 上下文、设备驱动框架扩展和低功耗优化。

### 1.1 文档和代码基线

| 项目 | 内容 |
|---|---|
| 目标板 | Alpha Project AP-RZG-0A |
| SoC | Renesas RZ/G1E（R8A77450，双 Cortex-A7） |
| 运行 CPU | CPU0 |
| 目标 RTOS | micro T-Kernel 3.00.08 |
| 编译器 | IAR Embedded Workbench for Arm |
| 下载/运行方式 | CSide 下载到 DDR，RAM 运行 |
| 比较基准 | `234308d feat:move tkernel source file and unchanged` |
| 当前分支 | `feat/move_tkernel` |
| 当前验证状态 | 源码、工程路径和交叉语法检查完成；真实 IAR/实板待验证 |

本文中的“新增、修改、删除”均相对提交 `234308d` 统计。原 uC3 文件多数只是从当前 IAR 构建项中移除，并没有从仓库物理删除。

### 1.2 原 uC3 工程的运行模型

迁移前的 `AP-RZG-0A.UART` 不是源码形式的 micro T-Kernel，而是：

- 链接预编译库 `Kernel/Standard/lib/CortexA/EWARM/v8/uC3cortexahl.a`。
- `sample.c/main()` 构造 `T_CSYS`，运行时填写对象数量、tick 和三块 uC3 内存范围。
- 调用 `start_uC3(&csys, initpr)` 启动内核。
- `initpr()` 初始化 Generic Timer、SCIF0，并用 `acre_tsk/acre_mpf/acre_mbx` 创建对象。
- `prst.s79`、`vector.s79`、`exception.c` 负责 uC3 启动和异常。
- `DDR_CortexA_GIC.c`、`DDR_CortexA_GTIMER.c` 和 `DDR_RZG1_SCIF.c` 服务 uC3 示例。
- 任务使用 `TA_FPU`，IAR 工程开启 FPU/NEON。

micro T-Kernel 的启动入口、内核对象配置、任务栈帧、SVC 调度、tick hook 和中断表都与上述机制不同，因此本次不是 API 名称替换，而是完整替换 CPU0 的 RTOS 运行边界。

### 1.3 迁移后的运行模型

```text
CSide/Loader 已完成 DDR 和主时钟初始化
  -> RZ/G1E IAR Reset/vector
  -> MMU/Cache
  -> IAR C Runtime
  -> AP-RZG-0A board main
  -> micro T-Kernel knl_main
  -> initial task
  -> sample usermain
  -> producer/consumer/LED tasks
```

内核由约 34 个 micro T-Kernel 公共源码文件、RZ/G1E CPU 适配、AP-RZG-0A 板级适配和两份 IAR 汇编共同构成，不再依赖 uC3 静态库。

### 1.4 迁移难度和风险

总体难度属于 Cortex-A RTOS bring-up 的高难度部分。应用任务改写并不复杂，真正困难的是内核运行前后的 ABI 和硬件状态。

| 项目 | 难度 | 主要风险 |
|---|---|---|
| IAR 启动汇编 | 高 | GNU/IAR 段、符号、C Runtime 入口不同 |
| 异常和调度 | 很高 | 异常返回栈帧、TCB offset、SVC/IRQ 返回任何一处不匹配都会在首次调度崩溃 |
| GIC | 高 | Distributor 和 CPU Interface 必须一起配置；PPI/SPI 路由不同 |
| Generic Timer | 中高 | CP15 访问、频率、PPI 29 和 tick 延迟补偿 |
| MMU/Cache | 高 | 链接地址、页表和 Cache 属性不一致会在开 MMU 后立即 abort |
| IAR 工程 | 中高 | 从一个预编译库切换到 50 个源码/汇编构建项，容易出现重复入口 |
| CPU0/CPU1 隔离 | 高 | CPU1 误入同一内核会破坏 CPU0 全局状态和栈 |
| 实板调试 | 高 | 早期异常发生在串口可用之前，只能按启动阶段断点定位 |

为降低首次 bring-up 风险，本阶段同时关闭 CPU1、FPU/NEON、低功耗 `WFI` 和串口中断，只保留 CPU0、轮询串口和最小内核功能。

## 2. 参考依据

硬件配置同时参考了仓库内原 uC3 示例和以下资料：

- `E:/work/refer/1、AP-RZG-0A/ap_rzg_0a_说明书.pdf`
  - 第 13 页：板卡采用双核 Cortex-A7，提供 SCIF0 接口。
  - 第 18、39 页：SCIF0 使用 GP3_27/SCIF0_RXD_D 和 GP3_28/SCIF0_TXD_D。
  - 第 20 页：GP6_24、GP6_25 连接板上 LED。
- `E:/work/refer/1、AP-RZG-0A/RZ_G1E_UsersManualHardware.pdf`
  - 第 9、10 页：RZ/G1E 为双核 Cortex-A7。
  - 第 47 页：确认 GP3_27、GP3_28 的 SCIF0 复用功能。
- `E:/work/refer/1、AP-RZG-0A/RZG_detai_User’s Manual Hardware.pdf`
  - 第 283 页：Generic Counter 控制寄存器基址为 `0xE6080000`，设置 `CNTCR.bit0` 启动计数器；计数频率为 ZS 时钟的 1/8。
  - 第 136 页：ZS 时钟为 260 MHz，因此系统计数器使用 32.5 MHz。
  - 第 290 页：INTC-SYS 基于 ARM Generic Interrupt Controller。

GIC 基址、Secure Physical Timer 中断号和 MMU 属性还与现有 `Driver/Standard` 及 AP-RZG-0A uC3 示例交叉核对：

- `Driver/Standard/inc/RZG1_UC3.h`：GIC 基址 `0xF1000000`，Secure Physical Timer 中断号 29。
- `Driver/Standard/src/DDR_CortexA_GIC.c`：Distributor/CPU Interface 偏移分别为 `0x1000`/`0x2000`。
- `Sample/Standard/RZG1E/AP-RZG-0A.UART/src/mmutbl_cfg.c`：保留原板级 MMU 映射。
- `Driver/Standard/src/EWARM/DDR_CortexA_MMU.s79`：复用已经验证过的 IAR MMU/Cache 初始化入口。

## 3. 板卡上电、下载和运行流程

### 3.1 本 image 运行前的硬件前提

本工程是 RAM-download image，不包含完整的 DDR 控制器和 PLL 冷启动程序。板卡上电后，必须先由已有 Loader/U-Boot 或 CSide 初始化流程完成：

1. SoC Boot ROM 按启动模式读取板载启动介质。
2. 板级 Loader 建立最基本的时钟和 DDR 环境。
3. U-Boot 或 CSide 初始化脚本使 `0x40000000` DDR 可读写。
4. CSide 连接 CPU0、停止目标并下载 IAR 输出映像。

`cpu_clock.c` 的 `startup_clock()` 是空实现，表示本移植明确沿用 CSide/Loader 已建立的主时钟。若绕过前级初始化，冷启动时直接跳到 `0x40000000`，程序可能在第一条 DDR 取指之前失败。

### 3.2 CSide RAM 下载路径

```text
板卡上电
  -> Loader/U-Boot/CSide 初始化 DDR、主时钟
  -> CSide halt CPU0
  -> 下载 sample_uart.out 或 sample_uart.srec 到链接地址
  -> 0x40000000 放置 .intvec
  -> PC 指向 Reset_Handler/0x40000000
  -> Run CPU0
```

本阶段是 CPU0 单核 image，不下载 CPU1 stub，也不主动释放 CPU1。RAM 下载稳定后才能另行设计 QSPI 固化；不要在 RTOS 首次 bring-up 时同时修改 Flash 启动链。

### 3.3 向量入口和 Reset Handler

`.intvec` 固定在 `0x40000000`，8 个 ARM low vector slot 如下：

| 偏移 | 异常 | 入口 |
|---:|---|---|
| `0x00` | Reset | `Reset_Handler` |
| `0x04` | Undefined | `undef_entry` |
| `0x08` | SVC | `svc_entry` |
| `0x0C` | Prefetch Abort | `iabort_entry` |
| `0x10` | Data Abort | `dabort_entry` |
| `0x14` | Reserved | `NOP` |
| `0x18` | IRQ | `irq_entry` |
| `0x1C` | FIQ | `fiq_entry` |

`Reset_Handler` 的实际执行顺序：

| 顺序 | 实现 | 目的 |
|---:|---|---|
| 1 | `CPSID IF, #SYS_MODE` | 在确定模式下屏蔽 IRQ/FIQ |
| 2 | `SP=SFE(CSTACK)` | 在调用任何 helper 前提供临时有效栈 |
| 3 | 读取 MPIDR Affinity0 | 判断当前 Cortex-A7 核 |
| 4 | 非 CPU0 跳 `secondary_cpu_park` | 阻止 CPU1 进入 CPU0 内核 |
| 5 | 写 `RWDT_RWTCSRA=0xA5A5A500` | 关闭/解除原 watchdog 干扰 |
| 6 | 清 SCTLR.I/C/A/M/V | 关闭 I/D Cache、alignment、MMU 和高向量 |
| 7 | `DSB/ISB` | 同步控制寄存器修改 |
| 8 | `_kernel_invalid_cache()` | 失效 Cache 和 branch predictor |
| 9 | `_kernel_invalid_tlb()` | 清除前一个系统留下的 TLB |
| 10 | VBAR=`mtk_vector_table` | 异常改走新内核入口 |
| 11 | 设置 ACTLR.SMP bit6 | 建立 Cortex-A7 多核一致性基础状态 |
| 12 | `r0=SFB(TLB_RAMSEC)` | 传入 `0x50000000` 页表地址 |
| 13 | `r1=mmu_cfgtbl` | 传入板级 MMU 映射表 |
| 14 | `_ddr_cortexa_mmu_init()` | 建页表、写 TTBR/DACR、开 MMU/Cache |
| 15 | 跳到 `mmu_enabled` | 在 identity mapping 下刷新取指 |
| 16 | 设置 FIQ/UND/ABT/SVC/IRQ/SYS SP | 每种异常使用独立栈 |
| 17 | 跳 IAR `?main` | 先执行 C Runtime，再调用板级 `main()` |

`0x40000000` 程序区采用 VA=PA identity mapping，因此 MMU 打开前后的指令地址数值不变。

### 3.4 IAR C Runtime 到 micro T-Kernel

IAR `?main` 完成 data copy 和 BSS 初始化后进入 `sys_start.c/main()`：

```text
main
  -> knl_startup_hw
       -> startup_clock()          空实现，保持前级时钟
       -> setup_uart0_pins()       GP3_27/28 -> SCIF0 D
       -> setup_led_pins()         GP6_24/25 -> GPIO output
       -> start_system_counter()   CNTFID0=32500000, CNTCR=3
  -> knl_lowmem_top/limit = 128 KiB MTK_SYS_MEM
  -> knl_sysmem_top/end = 同一范围，供调试
  -> knl_main
```

`ADD_PREFIX_MAIN_FUNC=1` 让 micro T-Kernel 公共 `sysinit.c` 导出 `knl_main()` 而不是第二个 `main()`，避免 IAR 入口冲突。

### 3.5 `knl_main()` 的内核初始化顺序

```text
knl_main
  -> DISABLE_INTERRUPT
  -> libtm_init / tm_com_init
  -> knl_init_Imalloc
  -> knl_init_device
  -> knl_init_interrupt
  -> knl_init_object
       -> task / semaphore / flag / mailbox / message buffer
       -> mutex / memory pool / cyclic / alarm
  -> knl_timer_startup
       -> 初始化软件时钟和 timer queue
       -> 配置 CNTP_TVAL/CNTP_CTL
       -> EnableInt(PPI 29)
  -> tk_cre_tsk(knl_init_ctsk)
  -> tk_sta_tsk
  -> knl_force_dispatch
       -> SVC #7
       -> 第一次任务切换
```

第一次调度进入 `inittask.c/init_task_main()`，它调用 `knl_start_device()`，输出 micro T-Kernel 版本，然后调用 `sample.c` 中的强符号 `usermain()`。内核自带的 `usermain.c` 是弱默认实现，会被 sample 覆盖。

### 3.6 1 ms tick 中断链

```text
Generic Physical Timer 到期
  -> GIC PPI 29
  -> irq_entry 读取 GICC_IAR
  -> knl_intvec_tbl[29]
  -> knl_hll_inthdr
  -> knl_hll_inthdr_tbl[29]
  -> knl_timer_handler
       -> 重装 CNTP_TVAL
       -> 软件时间 +1 ms
       -> 执行 delay/timeout/cyclic 到期事件
       -> EndOfInt(29)，写 GICC_EOIR
  -> knl_return_inthdr
       -> 比较 knl_ctxtsk/knl_schedtsk
       -> 如需抢占则 knl_dispatch_entry
       -> 否则 RFE 返回原任务
```

当前按非嵌套 IRQ bring-up：高层 IRQ 处理中不主动重新开放普通 IRQ，避免首次移植时出现内核重入、栈增长和 GIC priority/EOI 时序问题。

### 3.7 任务上下文切换

ARMv7-A `SStackFrame` 包含 `r4-r11` 和异常现场 `r0-r3/r12/lr/pc/cpsr`。汇编使用：

- `TCB_CTX=24`：`TCB.tskctxb` 在当前内核 TCB 中的固定 offset。
- `CTX_SSP=0`：`CTXB.ssp` 在 context block 内的 offset。
- `knl_ctxtsk`：当前已保存上下文的任务。
- `knl_schedtsk`：调度器选择的任务。
- `knl_dispatch_disabled`：调度禁止状态。

`knl_dispatch_entry` 保存当前任务的 `r4-r11` 和 SP；`dispatch_restore` 从下一任务 TCB 恢复 SP、寄存器和异常现场，最后用 `RFEFD` 同时恢复 PC/CPSR。本阶段不保存 FPSCR/D0-D31。

### 3.8 CPU1 当前状态

CPU1 读取 MPIDR 后直接进入：

```text
secondary_cpu_park:
    WFE
    B secondary_cpu_park
```

因此 CPU1 不使用 CPU0 栈、不进入 IAR Runtime、不初始化 GIC Distributor，也不写 micro T-Kernel 全局变量。后续支持 CPU1 时必须另建入口、栈、GIC banked 状态和共享内存协议，不能直接删掉 `WFE`。

## 4. 目标目录结构

新增目标采用“板级 + CPU 级”分层：

```text
Kernel/mtkernel/
  include/sys/sysdepend/ap_rzg0a_rzg1e/   板级 machine/profile/sysdef
  include/sys/sysdepend/cpu/rzg1e/        Cortex-A7、GIC、Timer、外设常量
  include/tk/sysdepend/ap_rzg0a_rzg1e/    对外板级 T-Kernel 定义
  include/tk/sysdepend/cpu/rzg1e/         对外 CPU 定义
  kernel/sysdepend/ap_rzg0a_rzg1e/        板级启动、时钟、设备生命周期
  kernel/sysdepend/cpu/rzg1e/              异常、调度、GIC、Timer
  lib/libtk/sysdepend/cpu/rzg1e/           中断控制和微秒等待
  lib/libtm/sysdepend/ap_rzg0a_rzg1e/      SCIF0 T-Monitor
```

`machine.h` 新增 `_AP_RZG0A_RZG1E_` 选择项，展开为 `AP_RZG0A_RZG1E`、`CPU_RZG1E`、`CPU_CORE_ARMV7A`、`CPU_CORE_ACA7`，同时提供 IAR 所需的内联、段、弱符号和汇编声明宏。

## 5. 关键移植内容

### 5.1 IAR 启动和异常汇编

`mtk_startup.s79` 和 `mtk_context.s79` 使用 IAR ARM 汇编语法重新实现 ARMv7-A 入口，没有直接修改官方 RZA2M/GNU 汇编。

主要语法转换包括：

| GNU 形式 | IAR 形式 |
|---|---|
| `.section`、`.text` | `SECTION ...:CODE/DATA` |
| `.global`、`.extern` | `PUBLIC`、`IMPORT` |
| `.word` | `DCD` |
| `.equ` | `EQU` |
| `.align` | `SECTION` 对齐属性 |
| 链接脚本栈符号 | `SFB(section)`、`SFE(section)` |

异常向量表放到 `.intvec`，包含 Undefined、SVC、Prefetch Abort、Data Abort、IRQ 和 FIQ 入口。IRQ 入口读取 `GICC_IAR`，按中断号查 micro T-Kernel 向量表，执行高层处理函数后写 `GICC_EOIR`。调度入口保存/恢复 `r4-r11` 以及异常现场，使任务可在中断退出或 SVC 后切换。

### 5.2 GIC

RZ/G1E 使用：

- Distributor：`0xF1001000`
- CPU Interface：`0xF1002000`
- 向量数：415
- CPU target：只路由到 CPU0

初始化时先关闭 GIC，清除 enable/pending，初始化 group、trigger、priority 和 CPU target，再设置 PMR/BPR 并开启 CPU Interface 和 Distributor。系统节拍使用 PPI 29，所以不通过 SPI target 寄存器配置。

### 5.3 Generic Physical Timer

系统节拍使用 ARM Generic Physical Timer 的 CP15 寄存器：

- Generic Counter：32.5 MHz
- micro T-Kernel tick：1 ms
- 每 tick 重装值：32,500 counts
- 中断：Secure Physical Timer，INTID 29

定时器 ISR 调用 `knl_timer_handler`。重装时读取可能已经为负数的 `CNTP_TVAL` 并补偿中断延迟，避免长期节拍漂移；若补偿后小于 1，则钳制为 1。

### 5.4 MMU、Cache 和共享内存

启动阶段继续使用板级已有的 `DDR_CortexA_MMU.s79` 和 `mmutbl_cfg.c`。移植只把 `mmutbl_cfg.c` 的内核类型依赖从 uC3 头文件改为 `<tk/tkernel.h>`，没有改动 MMU 表内容。

重要区域：

- `0x40000000` 起：CPU0 程序和普通 DDR 数据，按原表使用可缓存属性。
- `0x40B00000-0x40FFFFFF`：保留为 non-cache 区域，供后续 CPU0/CPU1 或设备共享内存使用。
- `0x50000000-0x50003FFF`：16 KiB 一级页表区域。

启动顺序先关闭 Cache/MMU，再失效 Cache/TLB，建立页表，最后由原板级函数启用 MMU/Cache。PFC、GPIO 和 Generic Counter 配置完成后执行同步操作。

### 5.5 链接文件

`sample.icf` 删除 uC3 kernel 专用段，改为 micro T-Kernel 使用的布局：

| 区域 | 地址 | 用途 |
|---|---:|---|
| CODE | `0x40000000-0x400FFFFF` | 向量和只读代码 |
| DATA | `0x40100000-0x40A7FFFF` | 数据、BSS、128 KiB 内核静态系统内存 |
| STACK | `0x40A80000-0x40AFFFFF` | SYS/SVC/IRQ/FIQ/UND/ABT 栈 |
| UNCACHE | `0x40B00000-0x40FFFFFF` | 非缓存共享区 |
| TLB | `0x50000000-0x50003FFF` | MMU 一级页表 |

`.intvec` 固定在 CODE 起始地址；`MTK_SYS_MEM` 放入 DATA；模式栈分别绑定到对应 IAR section。

### 5.6 内核配置

`Kernel/mtkernel/config/config.h` 的首次 bring-up 配置为：

- `CNF_TIMER_PERIOD = 1`：1 ms 系统节拍。
- `USE_STATIC_SYS_MEM = 1`、`SYSTEM_MEM_SIZE = 128 * 1024`：静态分配内核系统内存。
- `USE_FPU = 0`、`ALWAYS_FPU_ATR = 0`：暂不保存/恢复 FPU 上下文。
- 保留 `ADD_PREFIX_MAIN_FUNC = 1`，避免库内部入口与 IAR `main` 冲突。

### 5.7 板级初始化和串口

`knl_startup_hw` 完成以下工作：

1. 保持 CSide/前级启动程序已经配置好的主时钟，不重复改 PLL。
2. 把 GP3_27/GP3_28 配置为 SCIF0_RXD_D/SCIF0_TXD_D。
3. 把 GP6_24/GP6_25 配置为 GPIO 输出，供 LED 测试任务使用。
4. 写 `CNTFID0 = 32500000`，启动 Generic Counter。

T-Monitor 使用 SCIF0 轮询收发，配置为 115200 波特率。该路径不依赖中断，便于内核启动初期输出故障位置。

### 5.8 target 头文件选择链

编译器定义 `_AP_RZG0A_RZG1E_` 后，头文件选择路径是：

```text
<sys/machine.h>
  -> sysdepend/ap_rzg0a_rzg1e/machine.h
     -> sysdepend/cpu/rzg1e/machine.h
        -> sysdepend/cpu/core/armv7a/machine.h
```

board `machine.h` 定义：

```c
#define AP_RZG0A_RZG1E   1
#define CPU_RZG1E        1
#define CPU_CORE_ARMV7A  1
#define CPU_CORE_ACA7    1
#define TARGET_DIR       ap_rzg0a_rzg1e
#define KNL_SYSDEP_PATH  ap_rzg0a_rzg1e
```

CPU `machine.h` 定义 `TARGET_CPU_DIR=rzg1e`。公共 `<sys/sysdef.h>`、`<sys/profile.h>`、`<tk/cpudef.h>`、`<tk/syslib.h>` 再根据 `TARGET_DIR` 自动包含 AP-RZG-0A 头，board 头继续转发到 RZ/G1E CPU 头，CPU 头最后复用 ARMv7-A core。

这层转发让公共 micro T-Kernel 源码不需要加入任何 RZ/G1E 条件编译。

### 5.9 IAR 编译器适配宏

`include/sys/machine.h` 在保留 GNU 分支的同时增加 IAR 分支：

| 公共宏 | IAR 实现 | 使用位置 |
|---|---|---|
| `Inline` | `static inline` | syslib、timer、CPU status |
| `Asm` | `asm` | 公共 barrier 等接口 |
| `Noinit(decl)` | `__no_init decl` | kernel interrupt table、软件时钟 |
| `Section(decl,name)` | `decl @ #name` | `MTK_SYS_MEM` |
| `WEAK_FUNC` | `__weak` | 默认 `usermain` |
| `_VECTOR_ENTRY` | `DCD` | IAR vector 语法 |
| `_WEAK_ENTRY` | `PUBWEAK` | IAR weak 汇编符号 |

`Csym(sym)=sym` 表示 IAR ARM C symbol 不加 GNU/旧工具链式前导下划线。

### 5.10 IAR 汇编转换和异常细节

新建 `mtk_startup.s79` 和 `mtk_context.s79`，没有改 RZA2M/GNU 文件。主要语法转换：

| GNU | IAR |
|---|---|
| `.section/.text` | `SECTION name:CODE/DATA` |
| `.global/.extern` | `PUBLIC/IMPORT` |
| `.word` | `DCD` |
| `.equ` | `EQU` |
| linker stack symbol | `SFB(section)/SFE(section)` |
| ARM mode | `CODE32` |
| ABI alignment | `REQUIRE8/PRESERVE8` |

`irq_entry` 的关键步骤：

1. `SUB lr,lr,#4` 修正 IRQ return address。
2. `SRSDB sp!,#SVC` 把返回 PC/CPSR 保存到 SVC stack。
3. 切到 SVC mode，保存 `r0-r3/r12/lr`。
4. 读 `GICC_IAR`，取低 10-bit INTID。
5. INTID 小于 415时查 `knl_intvec_tbl`。
6. `TA_HLNG` handler 先进入 `knl_hll_inthdr`。
7. spurious 1023 不做 EOI 直接返回；其他非法 INTID 写实际 IAR 值到 EOIR 后进默认 handler。

`svc_entry` 同时支持 ARM 和 Thumb SVC instruction 编码，解析 SVC number 后查 `knl_svcvec_tbl[0..10]`。当前关键项：

- index 7 -> `knl_dispatch_to_schedtsk`
- index 8 -> `knl_dispatch_entry`

FIQ、Undefined、Prefetch Abort、Data Abort 各自切到独立模式栈，再进入 ARMv7-A 公共 C handler。

### 5.11 GIC 初始化的具体寄存器动作

RZ/G1E target 采用：

```c
GICD_BASE = 0xF1001000
GICC_BASE = 0xF1002000
N_INTVEC  = 415
```

寄存器数组长度按 415 向上取整，不沿用 ARMv7-A core 的默认 512-vector 长度。

`knl_init_interrupt()` 顺序：

1. `GICD_CTLR=0`、`GICC_CTLR=0`。
2. 清空 `knl_intvec_tbl[415]` 和 `knl_hll_inthdr_tbl[415]`。
3. 全范围 `ICENABLER=0xFFFFFFFF`，先禁用中断。
4. 全范围 `ICPENDR=0xFFFFFFFF`，清 pending。
5. `IGROUPR=0`，当前 secure bring-up 使用 Group 0。
6. 从 SPI 配置寄存器开始初始化 `ICFGR`。
7. 所有 priority byte 初始化为 `0xF8`。
8. INTID 32 以上的 SPI target 写 `0x01`，只指向 CPU0。
9. `GICC_PMR=0xF8`。
10. `GICC_BPR=2`。
11. `GICC_CTLR=3`，打开 CPU interface。
12. `GICD_CTLR=1`，打开 Distributor。
13. 把 INTID 29 注册为 `TA_HLNG` 的 `knl_timer_handler`。

PPI 29 是 CPU 私有中断，不通过 SPI `ITARGETR` 路由。

`int_rzg1e.c` 另外实现 `EnableInt/DisableInt/SetIntMode/ClearInt/CheckInt/EndOfInt` 和 PMR level API。`INTPRI_BITWIDTH=5`、`INTPRI_SHIFT=3`，内核优先级会转换为 GIC priority byte。

### 5.12 Generic Physical Timer 的具体 CP15 操作

频率换算：

```text
ZS = 260 MHz
Generic Counter = ZS / 8 = 32.5 MHz
CNF_TIMER_PERIOD = 1 ms
reload = 32,500 counts
```

`mtk_context.s79` 中：

| helper | 动作 |
|---|---|
| `rzg1e_gtimer_start` | 写 `CNTFRQ=32500000`、`CNTP_TVAL=32500`、`CNTP_CTL=1` |
| `rzg1e_gtimer_reload` | 读取旧 `CNTP_TVAL`，补偿负延迟后重装 |
| `rzg1e_gtimer_stop` | `CNTP_CTL=2`，mask 并关闭 |
| `rzg1e_gtimer_value` | 读取 `CNTP_TVAL` |
| `rzg1e_gtimer_counter` | 用 `MRRC` 读取 64-bit physical counter |

延迟补偿：

```text
next = nominal_reload
if old_tval < 0:
    next += old_tval
if next < 1:
    next = 1
```

这样 ISR 晚到 N counts 时，下一个周期缩短 N counts，避免每个 tick 都从 ISR 执行时刻重新计算完整 1 ms 造成累计漂移。

`sys_timer.h` 把 helper 接到内核需要的 start/clear/end/terminate/get-nsec hook。`wusec_rzg1e.c` 则读取 64-bit counter 实现 `WaitUsec/WaitNsec`，只用于短 busy wait。

### 5.13 MMU/Cache 的具体保留和修改

复用：

- `Driver/Standard/src/EWARM/DDR_CortexA_MMU.s79`
- `Sample/.../src/mmutbl_cfg.c` 的全部映射项

`mmutbl_cfg.c` 只改一行：

```diff
-#include "kernel.h"
+#include <tk/tkernel.h>
```

原因是移除 uC3 类型依赖，而不是改变内存属性。

MMU 流程：

1. 关闭旧 I/D Cache、branch prediction 和 MMU。
2. 失效 Cache、branch predictor、TLB。
3. 在 `0x50000000` 清零/生成 16 KiB 一级页表和所需二级表。
4. 遍历 `mmu_cfgtbl` 写 section/page descriptor。
5. 配置 TTBCR、TTBR0/1、DACR、context ID。
6. 打开 I-Cache、branch prediction、D-Cache、MMU。
7. `ISB` 后回到 identity-mapped code。

主要映射：

| VA=PA | 大小 | 属性 | 用途 |
|---|---:|---|---|
| `0x40000000` | 1 MiB | `ATR_WBAW` | code/vector |
| `0x40100000` | 10 MiB | `ATR_WBAW` | data/BSS/stack |
| `0x40B00000` | 5 MiB | `ATR_NONC` | non-cache/shared |
| `0x41000000` | 16 MiB | `ATR_WBAW|ATR_SHRD` | 原 shareable DDR |
| `0x50000000` | 1 MiB mapping | `ATR_WBAW` | TLB window |
| `0xE6000000...` | 多段 | `ATR_STRG` | SoC 外设 |
| `0xF1000000` | 1 MiB | `ATR_STRG` | GIC |

后续 CPU0/CPU1 数据应优先放 `0x40B00000` non-cache 区，或使用明确的 Cache maintenance 和 DMB/DSB；`volatile` 本身不能解决跨核 Cache 可见性。

### 5.14 链接布局具体替换

旧 ICF 中的 uC3 专用段：

```text
VINFTBL / VECTTBL
MPLMEM / STKMEM / SYSMEM / SYS
RAM_DATA_region / SYS_DATA_region
```

全部从新 ICF 移除，替换为：

| Region | 地址 | 大小 | 内容 |
|---|---:|---:|---|
| CODE | `0x40000000-0x400FFFFF` | 1 MiB | `.intvec`、readonly |
| DATA | `0x40100000-0x40A7FFFF` | 9.5 MiB | readwrite、BSS、`MTK_SYS_MEM` |
| STACK | `0x40A80000-0x40AFFFFF` | 512 KiB | 各模式栈/C stack |
| UNCACHE | `0x40B00000-0x40FFFFFF` | 5 MiB | `UNCACHE` section |
| TLB | `0x50000000-0x50003FFF` | 16 KiB | `TLB_RAMSEC` |

栈大小：

| 栈 | 大小 |
|---|---:|
| CSTACK/SYS | 32 KiB |
| SVC | 4 KiB |
| IRQ | 4 KiB |
| FIQ | 1 KiB |
| UND | 1 KiB |
| ABT | 1 KiB |

`.intvec` 使用 `place at start of CODE_region`，`TLB_RAMSEC` 使用 `place at start of TLB_region`。这两个固定位置是启动和 MMU 能否工作的硬约束。

### 5.15 静态内核内存和生命周期 hook

`config.h` 打开 `USE_STATIC_SYS_MEM=1`，扩大 `SYSTEM_MEM_SIZE` 到 128 KiB。`sys_start.c` 用 IAR section 宏声明：

```c
Section(EXPORT UW knl_system_mem[...], MTK_SYS_MEM);
```

再设置 `knl_lowmem_top/limit`，供 `knl_init_Imalloc()` 分配 TCB、对象 control block 和 task stack。这样不再依赖 uC3 的 `SYSMEM/STKMEM/MPLMEM`。

板级 hook：

- `cpu_clock.c`：clock 已由 CSide/Loader 初始化，当前 no-op。
- `devinit.c`：init/start/finish 返回 `E_OK`，满足内核生命周期。
- `power_save.c`：`low_pow/off_pow` no-op，首次 bring-up 不执行 `WFI`。
- `knl_restart_hw()`：返回 `E_NOSPT`。
- `knl_shutdown_hw()`：屏蔽中断后停机循环。

### 5.16 PFC、GPIO 和串口的寄存器适配

RZ/G1E PFC 写保护要求先向 `PMMR` 写目标值反码，因此 `pfc_write()` 做：

```c
out_w(PFC_PMMR, ~value);
out_w(address, value);
```

SCIF0 pinmux：

1. 清 `GPSR3[28:27]`。
2. `MOD_SEL3[31:30]=3`，选择 SCIF0 D 组。
3. `IPSR8` 从 bit9 起的 6-bit field 写 `0x12`。
4. 重新置 `GPSR3[28:27]`，启用 peripheral function。
5. 更新 `PUPR3`，最后执行 DSB。

LED：

1. 清 `GPSR6[25:24]`，切 GPIO。
2. 清 `POSNEG[25:24]`。
3. 清 `IOINTSEL[25:24]`。
4. 置 `INOUTSEL[25:24]`，设输出。
5. 置 `OUTDTSEL[25:24]`。

T-Monitor 的 `tm_com.c` 直接提供 `tm_com_init/tm_snd_dat/tm_rcv_dat`：

- `SCIF0_BASE=0xE6E60000`。
- reset TX/RX FIFO，清 FSR/LSR error。
- `SMR=0`、`BRR=17`、`FCR=0x0030`、`SPTR|=0x00C3`。
- `SCR=TE|RE`。
- 发送轮询 `TDFE/TEND`。
- 接收轮询 `RDF/DR`；framing/parity/break/overrun 时 reset RX FIFO 后恢复。

### 5.17 为什么同时关闭 FPU 和 NEON

关闭三个层面：

1. `config.h`：`USE_FPU=0`、`ALWAYS_FPU_ATR=0`。
2. IAR Debug/Release：`FPU2=0`、`NEON=0`。
3. sample task：属性仅 `TA_HLNG`，不带 `TA_FPU`。

只关闭某一层不够：编译器仍可能生成 VFP 指令，或内核仍可能按 FPU task 分配 context。当前 dispatcher 没有保存 FPSCR/D0-D31，所以必须整体关闭。

## 6. IAR 工程修改

`sample_uart.ewp` 已完成：

- 定义 `_AP_RZG0A_RZG1E_`。
- 添加 micro T-Kernel include、kernel、libtk、libtm 路径。
- 加入 RZ/G1E 新目标 C/汇编文件及 micro T-Kernel 公共源码。
- 移除 uC3 静态库、uC3 启动文件、uC3 GIC/GTIMER 源码。
- 保留板级 DDR/MMU 公共文件。
- 关闭 FPU/NEON 代码生成选项。
- 使用新的 `sample.icf`。

### 6.1 Debug/Release 选项对照

| 选项 | 原值 | 新值 |
|---|---|---|
| `CCDefines` | Debug 空，Release 只有 `NDEBUG` | 两个配置均增加 `_AP_RZG0A_RZG1E_` |
| include | uC3 Standard include | mtkernel include/config/knlinc + Driver + sample |
| `IlinkAdditionalLibs` | `uC3cortexahl.a` | 空 |
| `FPU2` | 5 | 0 |
| `NEON` | 1 | 0 |
| entry | `Reset_Handler` | 保持，由新 startup 导出 |
| linker output | `sample_uart.out` | 保持 |
| S-record output | `sample_uart.srec` | 保持 |

新增 include 路径：

```text
Kernel/mtkernel/include
Kernel/mtkernel/config
Kernel/mtkernel/kernel/knlinc
Driver/Standard/inc
Sample/Standard/RZG1E/AP-RZG-0A.UART/src
```

### 6.2 从当前 IAR 构建中移除的 uC3 项

以下路径仍保留在仓库，只是不再由 `sample_uart.ewp` 编译：

```text
Driver/Standard/src/DDR_COM.c
Driver/Standard/src/DDR_CortexA_GIC.c
Driver/Standard/src/EWARM/DDR_CortexA_GIC_sub.c
Driver/Standard/src/DDR_CortexA_GTIMER.c
Driver/Standard/src/EWARM/DDR_CortexA_GTIMER_sub.c
Driver/Standard/src/DDR_RZG1_SCIF.c
Sample/.../src/exception.c
Sample/.../src/GL_kernel_id.c
Sample/.../src/hw_init.c
Sample/.../src/EWARM/krn_mem.c
Sample/.../src/EWARM/prst.s79
Sample/.../src/EWARM/vector.s79
Kernel/Standard/lib/CortexA/EWARM/v8/uC3cortexahl.a
```

其中 GIC/GTIMER/SCIF/异常/启动已由新 target 替代；`GL_kernel_id.c`、`krn_mem.c` 等只适用于 uC3 的对象 ID 和内存模型。

### 6.3 继续复用

```text
Driver/Standard/src/EWARM/DDR_CortexA_MMU.s79
Sample/Standard/RZG1E/AP-RZG-0A.UART/src/mmutbl_cfg.c
```

MMU 是板级硬件初始化，不属于 uC3 内核，因此继续使用；只把配置表的类型 include 改为 T-Kernel。

### 6.4 新工程分组

| IAR group | 文件数 | 内容 |
|---|---:|---|
| Application | 2 | `sample.c`、`mmutbl_cfg.c` |
| micro T-Kernel Core | 34 | sysinit、inittask、task、wait、timer、semaphore、memory、libtk、libtm 等 |
| RZG1E Target | 9 | clock、device、hw、start、GIC、interrupt API、wait、SCIF |
| ARMv7-A Core | 2 | `cpu_cntl.c`、`exc_hdl.c` |
| IAR Startup | 3 | startup、context、原 MMU 汇编 |
| 合计 | 50 | 当前工程全部源码和汇编 |

IAR 中按上述 group 展开后，可以直接区分公共内核、SoC 适配、板级适配和启动汇编，后续排查链接或重复 symbol 更容易。

## 7. 功能测试任务

新的 `sample.c` 同时验证以下 RTOS 功能：

| 测试路径 | 行为 | 正常现象 |
|---|---|---|
| Task + delay | producer 每 100 ms 运行 | `produced` 每秒约增加 10 |
| Semaphore | consumer 永久等待信号量 | `consumed` 跟随 `produced`，差值不超过 1 |
| Cyclic handler | 周期处理器每 100 ms 运行 | `cyclic` 每秒约增加 10 |
| Tick/dispatch | 三个任务和主任务均依赖 delay/调度 | 所有计数持续前进 |
| GPIO | LED 任务每 500 ms 切换 | 两个 LED 交替变化 |
| Critical section | DI/EI 保护计数快照 | 每秒输出一致快照 |

串口预期输出：

```text
[MTK][BOOT] AP-RZG-0A RZ/G1E CPU0
[MTK][TEST] task+delay+semaphore+cyclic+LED
[MTK][PASS] produced=10 consumed=10 cyclic=10 err=0
[MTK][PASS] produced=20 consumed=20 cyclic=20 err=0
```

具体计数可能因启动时刻相差 1；只要每秒继续增长、`consumed <= produced`、差值不超过 1 且 `err=0`，即判定通过。

### 7.1 创建的对象和优先级

| 对象 | 配置 | 验证内容 |
|---|---|---|
| semaphore | 初值 0、最大 1、`TA_TFIFO` | wait queue、signal、任务唤醒 |
| cyclic | 100 ms、phase 100 ms、`TA_STA` | timer event queue 和 handler |
| producer task | priority 5、stack 2048 | task、delay、signal |
| consumer task | priority 6、stack 2048 | forever wait、wake-up |
| LED task | priority 8、stack 2048 | 500 ms delay、GPIO |
| initial/usermain | 每 1000 ms report | tick、长期调度、T-Monitor |

producer 每 100 ms 执行 `produced_count++ -> tk_sig_sem -> tk_dly_tsk(100)`。consumer 永久等待 semaphore，每获得一次 token 便增加 `consumed_count`。两者验证从 task wait 到 ready、再到抢占/恢复的完整路径。

cyclic handler 只做 `cyclic_count++`，不在 task-independent context 中打印或执行长循环，保持 1 ms ISR 路径可控。

LED task 每 500 ms 使用 GPIO6 `OUTDTH/OUTDTL` 切换 GP6_24/25。即使 SCIF0 配置错误，LED 仍能作为 tick/dispatch 的独立硬件 heartbeat。

### 7.2 counter 快照为什么使用 DI/EI

`produced_count/consumed_count/cyclic_count` 可能由不同 task 或 timer handler 更新。`usermain`：

1. `DI(imask)` 保存当前 IRQ mask 并屏蔽 IRQ。
2. 只读取三个 word counter。
3. `EI(imask)` 恢复原 mask。
4. 在临界区外调用 `tm_printf`。

中断关闭窗口不包含串口轮询，避免打印延长 IRQ latency。`volatile` 只保证每次实际读写，不替代该一致快照。

### 7.3 PASS 判据

```text
produced 比上一秒增长
consumed 比上一秒增长
cyclic 比上一秒增长
consumed <= produced
produced - consumed <= 1
test_error == E_OK
```

第一秒可能是 9、10 或 11，重点是持续增长和逻辑关系。任一 RTOS API 返回负错误码都会记录到 `test_error`，后续报告变为 FAIL。

## 8. 编译、下载和板上验证

### 8.1 IAR 编译

1. 用 IAR EWARM 打开 `Sample/Standard/RZG1E/AP-RZG-0A.UART/build/EWARM/sample_uart.eww`。
2. 选择 `Debug` 配置并执行 Rebuild All。
3. 确认没有 uC3 library 或 uC3 startup 的链接引用。
4. 检查链接 map：`.intvec` 应位于 `0x40000000`，`TLB_RAMSEC` 应位于 `0x50000000`。

若使用命令行，可在 IAR 已加入 PATH 后执行：

```powershell
IarBuild.exe Sample\Standard\RZG1E\AP-RZG-0A.UART\build\EWARM\sample_uart.ewp -build Debug -log all
```

### 8.2 CSide 下载

沿用该 AP-RZG-0A 工程原有 CSide 初始化和下载流程：完成 DDR/时钟初始化后，下载 IAR 生成的 CPU0 调试映像并从 `0x40000000` 的向量入口运行。不要另外启动 CPU1 映像。

### 8.3 串口观察

- 接口：板卡 SCIF0。
- 格式：115200、8 data bits、no parity、1 stop bit。
- 上电后先观察两行 `[MTK]` 启动信息，再观察每秒一行 `[PASS]`。
- 同时确认 GP6_24、GP6_25 对应 LED 交替变化。

### 8.4 IAR map 必查项

| 符号/段 | 预期 |
|---|---|
| `.intvec` / `mtk_vector_table` | `0x40000000` |
| `Reset_Handler` | CODE region |
| `knl_system_mem` / `MTK_SYS_MEM` | DATA region |
| `SVC_STACK/IRQ_STACK/...` | `0x40A80000-0x40AFFFFF` |
| `TLB_RAMSEC` | `0x50000000` |
| `sample_uart.out` | 正常生成 |
| `start_uC3` | 不应出现 |
| uC3 static library | 不应出现 |

### 8.5 首次实板建议断点顺序

```text
Reset_Handler
mmu_enabled
?main / main
knl_startup_hw
knl_main
knl_init_interrupt
knl_timer_startup
knl_force_dispatch
init_task_main
usermain
irq_entry
knl_timer_handler
```

不要第一次只在 `usermain` 下断点。若某断点到不了，故障范围就是前一个阶段和当前阶段之间。

### 8.6 建议验收时间

- 先观察 30 秒，确认每秒 PASS 和 LED。
- 再连续运行至少 10 分钟，确认没有 tick 停止、abort 或 semaphore overflow。
- 最后用独立计时参考运行 1 小时，比较 `cyclic_count`，验证 32.5 MHz 假设和 tick 延迟补偿。

## 9. 故障定位

| 现象 | 优先检查 |
|---|---|
| 完全无串口输出 | Reset vector/下载地址、DDR 初始化、SCIF0 pinmux、PCLK 与 BRR |
| 有 BOOT，无周期输出 | Generic Counter 是否启动、PPI 29、GICC/GICD enable、IRQ mask |
| `produced` 不增长 | 任务创建/启动、调度入口、任务栈 |
| `produced` 增长但 `consumed` 不增长 | semaphore 向量、等待队列、IRQ 后调度 |
| `cyclic` 不增长 | tick ISR、cyclic queue、`knl_timer_handler` |
| 计数正常但 LED 不变 | GP6_24/25 pinmux、GPIO direction/output polarity |
| Data Abort/Prefetch Abort | `sample.icf` 与 MMU 表是否一致、TLB 地址、Cache/MMU 启动顺序 |
| 运行一段时间后漂移 | 核对 Generic Counter 实际频率和 `CNTFID0`，检查 `CNTP_TVAL` 补偿 |

### 9.1 到不了 Reset Handler

检查 entry label、`.intvec=0x40000000`、CSide 是否按链接地址加载、DDR 是否已经初始化、CPU0 PC 和 CPSR ARM/Thumb 状态。

### 9.2 Reset 能到，打开 MMU 后跑飞

检查：

- `SFB(TLB_RAMSEC)` 是否为 `0x50000000`。
- `mmu_cfgtbl` 是否以 size=0 结束。
- code/data/stack 是否都有 identity mapping。
- TTBR0/TTBCR/DACR/SCTLR。
- Prefetch Abort 的 LR/IFSR/IFAR。
- Data Abort 的 LR/DFSR/DFAR。

### 9.3 有 T-Monitor 版本，但到不了 usermain

逐项查看：

- `knl_init_Imalloc()` 和 128 KiB 范围。
- `knl_init_interrupt()`。
- `knl_init_object()`。
- `knl_timer_startup()`。
- initial task 的 `tk_cre_tsk/tk_sta_tsk`。
- SVC #7 是否到 `svc_entry`。
- `knl_svcvec_tbl[7]` 是否为 `knl_dispatch_to_schedtsk`。

### 9.4 到 usermain，但一秒报告不出现

优先判断 tick：

- `CNTCR.bit0=1`。
- `CNTFID0=32500000`。
- `CNTP_CTL.ENABLE=1` 且 INT_MASK=0。
- `CNTP_TVAL` 初值约 32500。
- GICD/GICC 已打开。
- PPI 29 已 enable。
- CPSR.I 已清。
- `GICC_IAR` 是否返回 29。

### 9.5 第一次 task switch 就 Data Abort

检查：

- `TCB_CTX=24` 是否匹配当前 `TCB` layout。
- `CTX_SSP=0` 是否匹配 `CTXB`。
- `SStackFrame` 与汇编 push/pop 顺序。
- task stack 的 8-byte alignment。
- IAR structure packing。
- FPU/NEON 是否完全关闭。

### 9.6 producer 增长，consumer 不增长

检查 `tk_sig_sem` 返回值、consumer task 创建/启动、semaphore wait queue 和 IRQ 退出后的 `knl_ctxtsk/knl_schedtsk` 比较。如果 consumer 没有及时取 token，max=1 的 semaphore 可能返回 overflow 并记录到 `test_error`。

### 9.7 cyclic 不增长

检查 `tk_cre_cyc` 返回值、`TA_STA`、cyclic event 是否插入 timer queue，以及 `knl_timer_handler` 是否执行 event callback。cyclic handler 不依赖普通 task 是否被调度，是区分“tick event queue 问题”和“任务问题”的重要指标。

## 10. 全部新增、修改和删除文件

相对 `234308d`：

- 修改 6 个。
- 新增 32 个，包含两份 Document 文件。
- 物理删除 0 个。
- 合计 38 个。

### 10.1 修改的 6 个文件

| 文件 | 改了什么 |
|---|---|
| `Kernel/mtkernel/config/config.h` | target 示例增加 AP-RZG-0A；tick 10 ms 改 1 ms；静态系统内存由关闭改为 128 KiB；FPU/ALWAYS_FPU 关闭 |
| `Kernel/mtkernel/include/sys/machine.h` | 增加 `_AP_RZG0A_RZG1E_` target 入口和 IAR compiler macro |
| `Sample/Standard/RZG1E/AP-RZG-0A.UART/build/EWARM/sample.icf` | 删除 uC3 SYS/STK/MPL/VINFTBL/VECTTBL 布局，改成 MTK code/data/stack/non-cache/TLB |
| `Sample/Standard/RZG1E/AP-RZG-0A.UART/build/EWARM/sample_uart.ewp` | 移除 uC3 库/源文件，加入 micro T-Kernel、RZG1E target 和新 IAR 汇编；关 FPU/NEON |
| `Sample/Standard/RZG1E/AP-RZG-0A.UART/src/mmutbl_cfg.c` | include 从 uC3 `kernel.h` 改为 `<tk/tkernel.h>`，MMU 表内容不变 |
| `Sample/Standard/RZG1E/AP-RZG-0A.UART/src/sample.c` | uC3 串口/mail demo 改为 micro T-Kernel task/semaphore/cyclic/tick/LED 测试 |

### 10.2 新增的 10 个公开 target 头文件

| 文件 | 内容 |
|---|---|
| `Kernel/mtkernel/include/sys/sysdepend/ap_rzg0a_rzg1e/machine.h` | board/CPU/core/path 宏 |
| `Kernel/mtkernel/include/sys/sysdepend/ap_rzg0a_rzg1e/profile.h` | 继承 CPU profile，关闭 low-power |
| `Kernel/mtkernel/include/sys/sysdepend/ap_rzg0a_rzg1e/sysdef.h` | 继承 CPU 地址，定义 `LOWPOW_LIMIT` |
| `Kernel/mtkernel/include/sys/sysdepend/cpu/rzg1e/machine.h` | `TARGET_CPU_DIR=rzg1e`，继承 ARMv7-A |
| `Kernel/mtkernel/include/sys/sysdepend/cpu/rzg1e/profile.h` | IO port、physical timer capability |
| `Kernel/mtkernel/include/sys/sysdepend/cpu/rzg1e/sysdef.h` | GIC/Timer/SCIF/PFC/GPIO/memory 常量 |
| `Kernel/mtkernel/include/tk/sysdepend/ap_rzg0a_rzg1e/cpudef.h` | board -> CPU cpudef |
| `Kernel/mtkernel/include/tk/sysdepend/ap_rzg0a_rzg1e/syslib.h` | board -> CPU syslib |
| `Kernel/mtkernel/include/tk/sysdepend/cpu/rzg1e/cpudef.h` | CPU -> ARMv7-A cpudef |
| `Kernel/mtkernel/include/tk/sysdepend/cpu/rzg1e/syslib.h` | 8/16/32-bit MMIO read/write/and/or |

### 10.3 新增的 9 个 AP-RZG-0A 板级内核文件

| 文件 | 内容 |
|---|---|
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/cpu_clock.c` | 时钟 hook；当前沿用 CSide/Loader |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/cpu_status.h` | 转发到 RZ/G1E CPU status |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/cpu_task.h` | 转发到 RZ/G1E task context |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/devinit.c` | device init/start/finish hook |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/hw_setting.c` | PFC、SCIF0 pin、LED GPIO、Generic Counter、shutdown |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/power_save.c` | 首次 bring-up 不执行 WFI/off power |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/sys_start.c` | IAR `main`、静态内存、`knl_main` 入口 |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/sys_timer.h` | 转发 CPU timer |
| `Kernel/mtkernel/kernel/sysdepend/ap_rzg0a_rzg1e/sysdepend.h` | 转发 CPU sysdepend，声明 clock hook |

### 10.4 新增的 7 个 RZ/G1E CPU/异常文件

| 文件 | 内容 |
|---|---|
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/EWARM/mtk_startup.s79` | vector、CPU0 reset、CPU1 park、MMU/Cache、模式栈、IAR Runtime |
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/EWARM/mtk_context.s79` | IRQ/SVC/FIQ/abort、HLL handler、dispatcher、IRQ mask、CP15 Timer |
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/cpu_status.h` | critical section、context 判断、SVC dispatch |
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/cpu_task.h` | 转发 ARMv7-A task frame |
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/interrupt.c` | GIC init、kernel vector table、PPI 29 handler |
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/sys_timer.h` | Generic Timer -> kernel timer hook |
| `Kernel/mtkernel/kernel/sysdepend/cpu/rzg1e/sysdepend.h` | 转发 ARMv7-A sysdepend |

### 10.5 新增的 3 个库适配文件

| 文件 | 内容 |
|---|---|
| `Kernel/mtkernel/lib/libtk/sysdepend/cpu/rzg1e/int_rzg1e.c` | interrupt controller API -> GIC |
| `Kernel/mtkernel/lib/libtk/sysdepend/cpu/rzg1e/wusec_rzg1e.c` | physical counter -> WaitUsec/WaitNsec |
| `Kernel/mtkernel/lib/libtm/sysdepend/ap_rzg0a_rzg1e/tm_com.c` | SCIF0 polling T-Monitor |

### 10.6 新增的测试和文档文件

| 文件 | 内容 |
|---|---|
| `Sample/Standard/RZG1E/AP-RZG-0A.UART/src/sample_uc3.c` | 原 uC3 sample 备份，不加入新构建 |
| `Document/AP-RZG-0A_mtkernel3_CPU0_porting.md` | 本详细说明 |
| `Document/AP-RZG-0A_mtkernel3_file_changes.txt` | 精确变更路径清单 |

### 10.7 删除项的准确含义

Git 物理删除：无。

“从 IAR 工程删除”仅表示 `.ewp` 不再编译旧 uC3 startup/GIC/GTIMER/SCIF 等文件。它们仍在仓库中，可用于回退和硬件行为对照。这样满足“保留 uC3 源码、不影响无关功能”的约束。

## 11. 分阶段提交

本次移植按四个阶段提交，便于逐段查看：

1. `b041a72 feat(rzg1e): add mtkernel target skeleton`
   - 目标宏、板级/CPU 目录、配置和头文件骨架。
2. `1b3cfb8 feat(rzg1e): port mtkernel CPU0 runtime`
   - IAR 启动/异常、调度、GIC、Timer、MMU/Cache、链接和工程接入。
3. `f7347dc test(rzg1e): add mtkernel CPU0 self-test`
   - 新 RTOS 测试任务，保留旧 uC3 示例。
4. 文档提交
   - 本说明和精确文件变更清单。

## 12. 已完成的静态验证与限制

已完成：

- 全部 IAR 工程文件路径存在性检查，`.ewp` XML 可解析。
- 47 个工程 C 文件使用 ARM Cortex-A7 交叉编译器完成预处理/语法检查，无错误。
- 两个 IAR 汇编文件经过指令级机械转换后使用 ARM GNU assembler 检查，未发现 ARM 指令错误。
- `git diff --check` 通过。
- 未修改任何 RZA2M 目标源文件。

当前机器上的 IAR 安装目录只有配置/头文件，未发现 `IarBuild.exe`、`iccarm.exe`、`iasmarm.exe`，因此这里不能声称已经完成真实 IAR 构建或板上运行。最终验收必须按第 8 节在完整 IAR 和 AP-RZG-0A 实板上执行。

## 13. 后续建议

首次实板通过后再依次进行：

1. 用示波器或长时间计数确认 1 ms tick 精度。
2. 增加 abort 寄存器输出和栈水位监测，便于长期运行诊断。
3. 在独立阶段加入 FPU/NEON context save/restore 并增加浮点任务压力测试。
4. CPU1 另建启动映像和共享内存协议；先明确 cache 属性、barrier 和中断路由，再解除 `WFE` 停放。
