#include "luamgr.h"

using namespace std;
int main(int argc, char const *argv[])
{
	luaMgr.init();
	luaMgr.start();
	luaMgr.clear();
	/* code */
	return 0;
}