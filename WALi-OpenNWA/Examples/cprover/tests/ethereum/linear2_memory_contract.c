int gas;
int memory[];
int localmem[];
int test(int addr0, int addr1, int call_dsize, int call_v, int calldata0) {
	gas = 0;
	int r10, r11, r12, r15, r4, r5, r7, r9;
	r10 = r11 = r12 = r15 = r4 = r5 = r7 = r9 = 0;
	label1:
		gas = gas + (30);
		// UseMemory operations currently ignored
		localmem[64] = 96;
		r7 = call_dsize < 4 ? 1 : 0;
		if (r7 != 0) {
			goto label2;
		} else {
			goto label3;
		}
	label2:
		gas = gas + (7);
		return 0;
	label3:
		gas = gas + (45);
		r5 = calldata0;
		r10 = r5 / addr0;
		r12 = (addr1 & r10);
		r15 = 638722032 == r12 ? 1 : 0;
		if (r15 != 0) {
			goto label4;
		} else {
			goto label13;
		}
	label4:
		gas = gas + (19);
		r5 = call_v == 0 ? 1 : 0;
		if (r5 != 0) {
			goto label5;
		} else {
			goto label12;
		}
	label5:
		gas = gas + (41);
		r4 = 0;
		r5 = 0;
		goto label6;
	label6:
		gas = gas + (76);
		r9 = memory[0];
		r11 = r4 < r9 ? 1 : 0;
		r12 = r11 == 0 ? 1 : 0;
		if (r12 != 0) {
			goto label7;
		} else {
			goto label11;
		}
	label7:
		gas = gas + (15);
		gas = gas + (100 == 0 ? 5000 : 20000);
		// Refund gas ignored
		memory[0] = 100;
		goto label8;
	label8:
		gas = gas + (76);
		r9 = memory[0];
		r11 = r5 < r9 ? 1 : 0;
		r12 = r11 == 0 ? 1 : 0;
		if (r12 != 0) {
			goto label9;
		} else {
			goto label10;
		}
	label9:
		gas = gas + (80);
		r10 = r4 + r5;
		// UseMemory operations currently ignored
		localmem[96] = r10;
		return 0;
	label10:
		gas = gas + (25);
		r10 = 1 + r5;
		r5 = r10;
		goto label8;
	label11:
		gas = gas + (25);
		r10 = 1 + r4;
		r4 = r10;
		goto label6;
	label12:
		gas = gas + (6);
		return 0;
	label13:
		gas = gas + (7);
		return 0;

}

void main(int addr0, int addr1, int call_dsize, int call_v, int calldata0) {
	test(addr0, addr1, call_dsize, call_v, calldata0);
	__VERIFIER_print_hull(gas);
	return;
}
