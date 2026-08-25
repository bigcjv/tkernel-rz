# AP-RZG-0A（RZ/G1E）CPU0 移植 micro T-Kernel 3 说明

## 1. 目标与范围

本次工作把 `Sample/Standard/RZG1E/AP-RZG-0A.UART` 的 CPU0 运行环境从 uC3 切换为 `Kernel/mtkernel` 中的 micro T-Kernel 3 源码，继续使用 IAR EWARM 编译和 CSide 下载。

当前移植范围如下：

- 只启动 CPU0；CPU1 在启动汇编中进入 `WFE` 等待，不参与内核运行。
- 新增独立的 `ap_rzg0a_rzg1e` 板级目标和 `rzg1e` CPU 目标，不修改 RZA2M 的源文件。
- CPU0 完成异常向量、任务上下文切换、GIC、1 ms 系统节拍、MMU/Cache、SCIF0 T-Monitor 输出和测试任务接入。
- 第一阶段关闭 FPU 上下文切换。
- uC3 代码和旧示例保留，旧 `sample.c` 保存为 `sample_uc3.c`，但不再加入当前 IAR 构建。

本次没有处理 CPU1 启动、SMP、FPU/NEON 上下文、设备驱动框架扩展和低功耗优化。

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

## 3. 启动和运行流程

CPU0 的启动链路为：

```text
Reset_Handler
  -> 判断 MPIDR，只允许 CPU0 继续
  -> 关闭 watchdog、MMU、I/D Cache 和分支预测
  -> 失效 Cache、TLB，设置 VBAR 和 ACTLR.SMP
  -> 调用原板级 IAR MMU 初始化，建立页表并打开 MMU/Cache
  -> 初始化各 CPU 模式栈
  -> 进入 IAR C Runtime (?main)
  -> main -> knl_startup_hw -> knl_main
  -> 初始化 GIC、Generic Physical Timer 和内核对象
  -> 调用 usermain，运行测试任务
```

CPU1 在读取 MPIDR 后直接进入 `secondary_cpu_park`，循环执行 `WFE`。这样不会误入 CPU0 的栈、IAR Runtime 或内核全局状态。

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

`machine.h` 新增 `_AP_RZG0A_RZG1E_` 选择项，展开为 `AP_RZG0A_RZG1E`、`CPU_RZG1E`、`CPU_ARMV7A`、`CPU_CORTEXA7`，同时提供 IAR 所需的内联、段、弱符号和汇编声明宏。

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

## 6. IAR 工程修改

`sample_uart.ewp` 已完成：

- 定义 `_AP_RZG0A_RZG1E_`。
- 添加 micro T-Kernel include、kernel、libtk、libtm 路径。
- 加入 RZ/G1E 新目标 C/汇编文件及 micro T-Kernel 公共源码。
- 移除 uC3 静态库、uC3 启动文件、uC3 GIC/GTIMER 源码。
- 保留板级 DDR/MMU 公共文件。
- 关闭 FPU/NEON 代码生成选项。
- 使用新的 `sample.icf`。

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

## 10. 分阶段提交

本次移植按四个阶段提交，便于逐段查看：

1. `b041a72 feat(rzg1e): add mtkernel target skeleton`
   - 目标宏、板级/CPU 目录、配置和头文件骨架。
2. `1b3cfb8 feat(rzg1e): port mtkernel CPU0 runtime`
   - IAR 启动/异常、调度、GIC、Timer、MMU/Cache、链接和工程接入。
3. `f7347dc test(rzg1e): add mtkernel CPU0 self-test`
   - 新 RTOS 测试任务，保留旧 uC3 示例。
4. 文档提交
   - 本说明和精确文件变更清单。

## 11. 已完成的静态验证与限制

已完成：

- 全部 IAR 工程文件路径存在性检查，`.ewp` XML 可解析。
- 47 个工程 C 文件使用 ARM Cortex-A7 交叉编译器完成预处理/语法检查，无错误。
- 两个 IAR 汇编文件经过指令级机械转换后使用 ARM GNU assembler 检查，未发现 ARM 指令错误。
- `git diff --check` 通过。
- 未修改任何 RZA2M 目标源文件。

当前机器上的 IAR 安装目录只有配置/头文件，未发现 `IarBuild.exe`、`iccarm.exe`、`iasmarm.exe`，因此这里不能声称已经完成真实 IAR 构建或板上运行。最终验收必须按第 8 节在完整 IAR 和 AP-RZG-0A 实板上执行。

## 12. 后续建议

首次实板通过后再依次进行：

1. 用示波器或长时间计数确认 1 ms tick 精度。
2. 增加 abort 寄存器输出和栈水位监测，便于长期运行诊断。
3. 在独立阶段加入 FPU/NEON context save/restore 并增加浮点任务压力测试。
4. CPU1 另建启动映像和共享内存协议；先明确 cache 属性、barrier 和中断路由，再解除 `WFE` 停放。
