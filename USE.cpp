#include <iostream>
#include "parser.h"
#include "query.h"

using namespace std;

PF_Manager pfm;
RM_Manager rmm(pfm);
SM_Manager smm(rmm);
QL_manager qlm(rmm,smm);

int main()
{
	runParser(qlm);

	return 0;
}