#include <stdio.h>

int main() {
    int x, yyy;
    int xcubed, xsquared, xfourth;

    x = yyy = 0;
    //xsquared = xcubed = xfourth = 0;

    while(x <= 10) {
	yyy = yyy  + 4 * x*x*x*x - 3 * x*x*x + 2 * x*x - x + 1;
	//xfourth = xfourth + 4 * xcubed + 6 * xsquared + 4*x + 1;
	//xcubed = xcubed + 3 * xsquared + 3 * x + 1;
	//xsquared = xsquared + 2*x + 1;
	x = x + 1;
        
    }


    //printf("\n final yyy %d\n", yyy);
    assert(yyy == 9);
    //assert(20 * yyy == x * (34 + x * (-45 + x * (70 + x * (-55 + 16 * x)))));

    return 0;
}
