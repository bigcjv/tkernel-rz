/* AP-RZG-0A micro T-Kernel CPU0 functional test. */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define TEST_TASK_STACK_SIZE  2048
#define TEST_PRODUCER_PERIOD  100
#define TEST_REPORT_PERIOD    1000

#define GPIO6_OUTDTH          (RZG1E_GPIO6_BASE + 0x0044UL)
#define GPIO6_OUTDTL          (RZG1E_GPIO6_BASE + 0x0048UL)
#define LED1_MASK             (1UL << 24)
#define LED2_MASK             (1UL << 25)

LOCAL volatile UW produced_count;
LOCAL volatile UW consumed_count;
LOCAL volatile UW cyclic_count;
LOCAL volatile W test_error;
LOCAL ID test_semid;

LOCAL void record_error(ER ercd)
{
	if (ercd < E_OK) {
		test_error = ercd;
	}
}

LOCAL void producer_task(INT stacd, void *exinf)
{
	ER ercd;

	(void)stacd;
	(void)exinf;
	for (;;) {
		produced_count++;
		ercd = tk_sig_sem(test_semid, 1);
		record_error(ercd);
		ercd = tk_dly_tsk(TEST_PRODUCER_PERIOD);
		record_error(ercd);
	}
}

LOCAL void consumer_task(INT stacd, void *exinf)
{
	ER ercd;

	(void)stacd;
	(void)exinf;
	for (;;) {
		ercd = tk_wai_sem(test_semid, 1, TMO_FEVR);
		if (ercd >= E_OK) {
			consumed_count++;
		} else {
			record_error(ercd);
		}
	}
}

LOCAL void led_task(INT stacd, void *exinf)
{
	ER ercd;

	(void)stacd;
	(void)exinf;
	for (;;) {
		out_w(GPIO6_OUTDTL, ~LED1_MASK);
		out_w(GPIO6_OUTDTH, LED2_MASK);
		ercd = tk_dly_tsk(500);
		record_error(ercd);
		out_w(GPIO6_OUTDTH, LED1_MASK);
		out_w(GPIO6_OUTDTL, ~LED2_MASK);
		ercd = tk_dly_tsk(500);
		record_error(ercd);
	}
}

LOCAL void cyclic_handler(void *exinf)
{
	(void)exinf;
	cyclic_count++;
}

LOCAL ID create_task(FP entry, PRI priority)
{
	T_CTSK ctsk = {
		.exinf = NULL,
		.tskatr = TA_HLNG,
		.task = entry,
		.itskpri = priority,
		.stksz = TEST_TASK_STACK_SIZE,
		.bufptr = NULL
	};
	ID taskid;
	ER ercd;

	taskid = tk_cre_tsk(&ctsk);
	if (taskid < E_OK) return taskid;
	ercd = tk_sta_tsk(taskid, 0);
	if (ercd < E_OK) return ercd;
	return taskid;
}

EXPORT INT usermain(void)
{
	const T_CSEM csem = {
		.exinf = NULL,
		.sematr = TA_TFIFO,
		.isemcnt = 0,
		.maxsem = 1
	};
	const T_CCYC ccyc = {
		.exinf = NULL,
		.cycatr = TA_HLNG | TA_STA,
		.cychdr = (FP)cyclic_handler,
		.cyctim = TEST_PRODUCER_PERIOD,
		.cycphs = TEST_PRODUCER_PERIOD
	};
	UW previous_produced = 0;
	UW previous_consumed = 0;
	UW previous_cyclic = 0;
	ER ercd;
	ID object_id;

	tm_printf((const UB *)"\n[MTK][BOOT] AP-RZG-0A RZ/G1E CPU0\n");
	tm_printf((const UB *)"[MTK][TEST] task+delay+semaphore+cyclic+LED\n");

	test_semid = tk_cre_sem(&csem);
	if (test_semid < E_OK) {
		tm_printf((const UB *)"[MTK][FAIL] tk_cre_sem=%d\n", test_semid);
		for (;;) { ; }
	}
	tm_printf((const UB *)"[MTK][STEP] semaphore created: id=%d\n", test_semid);

	object_id = tk_cre_cyc(&ccyc);
	if (object_id < E_OK) {
		tm_printf((const UB *)"[MTK][FAIL] tk_cre_cyc=%d\n", object_id);
		for (;;) { ; }
	}
	tm_printf((const UB *)"[MTK][STEP] cyclic created: id=%d\n", object_id);

	object_id = create_task((FP)producer_task, 5);
	if (object_id < E_OK) record_error(object_id);
	else tm_printf((const UB *)"[MTK][STEP] producer started: id=%d\n", object_id);
	object_id = create_task((FP)consumer_task, 6);
	if (object_id < E_OK) record_error(object_id);
	else tm_printf((const UB *)"[MTK][STEP] consumer started: id=%d\n", object_id);
	object_id = create_task((FP)led_task, 8);
	if (object_id < E_OK) record_error(object_id);
	else tm_printf((const UB *)"[MTK][STEP] LED task started: id=%d\n", object_id);

	for (;;) {
		UW produced;
		UW consumed;
		UW cyclic;
		UINT imask;
		BOOL progress_ok;

		ercd = tk_dly_tsk(TEST_REPORT_PERIOD);
		record_error(ercd);

		DI(imask);
		produced = produced_count;
		consumed = consumed_count;
		cyclic = cyclic_count;
		EI(imask);

		progress_ok = (produced > previous_produced)
			&& (consumed > previous_consumed)
			&& (cyclic > previous_cyclic)
			&& (consumed <= produced)
			&& ((produced - consumed) <= 1U)
			&& (test_error == E_OK);

		tm_printf((const UB *)"[MTK][%s] produced=%lu consumed=%lu cyclic=%lu err=%ld\n",
			progress_ok ? "PASS" : "FAIL",
			produced, consumed, cyclic, test_error);

		previous_produced = produced;
		previous_consumed = consumed;
		previous_cyclic = cyclic;
	}
}
