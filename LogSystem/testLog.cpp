#include "../LogSystem/filelog/loghelp.h"

int main(int argc,char* argv[])
{
	LogHelp::InitLogHelp("F:\\test.txt");
	int a = 10;
	LOG_IF1(a);
	return 0;
}
