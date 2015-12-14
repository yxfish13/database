#include "query.h"
using namespace std;
int main(){
	PF_Manager pfm;
    RM_Manager rmm(pfm);
    SM_Manager smm(rmm);
	QL_manager QL(rmm,smm);
	QL.Use((const char *)("orderDB"));
	int cnt=0;
	AttrInfo attributes[100];
	attributes[cnt].attrName=string("id");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("name");
	attributes[cnt].attrType=STRING;
	attributes[cnt].attrLength=100;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("state");
	attributes[cnt].attrType=STRING;
	attributes[cnt].attrLength=2;
	attributes[cnt++].notNull=0;
	QL.CreateTable((const char *)("publisher"), cnt, attributes, (const char *)("id"));
	QL.Showtb();
	QL.Desc((const char *)("publisher"));
	QL.DropTable("publisher");
	QL.Showtb();
	QL.Desc((const char *)("publisher"));
	//QL.CloseDB("orderDB");
	//QL.DropDB("orderDB");
	//system("ls");
	return 0;
}