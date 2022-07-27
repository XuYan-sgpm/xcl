#include <stdio.h>
#include <xcl/lang/CThreadLocal.h>

int main() {
	CThreadLocal local=Local_make();
	/*Local_setInt32(&local,34);
	int val;
	Local_getInt32(&local,&val);
	printf("%d\n",val);*/
	return 0;
}