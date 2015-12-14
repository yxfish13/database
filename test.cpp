#include "query.h"
using namespace std;

	PF_Manager pfm;
    RM_Manager rmm(pfm);
    SM_Manager smm(rmm);
	QL_manager QL(rmm,smm);
void createTable_test(bool flag){
	int cnt=0;
	AttrInfo attributes[100];
	attributes[cnt].attrName=string("id");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("title");
	attributes[cnt].attrType=STRING;
	attributes[cnt].attrLength=100;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("authors");
	attributes[cnt].attrType=STRING;
	attributes[cnt].attrLength=200;
	attributes[cnt++].notNull=0;
	attributes[cnt].attrName=string("publisher_id");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("copies");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=0;
	attributes[cnt].attrName=string("pages");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=0;
	QL.CreateTable((const char *)("book"), cnt, attributes, (const char *)("id"));
	if (flag){
		QL.Showtb();
		QL.Desc((const char *)("book"));
		cout<<endl;
	}

	cnt=0;
	//AttrInfo attributes[100];
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
	if (flag){
		QL.Showtb();
		QL.Desc((const char *)("publisher"));
		cout<<endl;
	}	

	cnt=0;
	//AttrInfo attributes[100];
	attributes[cnt].attrName=string("id");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("name");
	attributes[cnt].attrType=STRING;
	attributes[cnt].attrLength=25;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("gender");
	attributes[cnt].attrType=STRING;
	attributes[cnt].attrLength=1;
	attributes[cnt++].notNull=0;
	QL.CreateTable((const char *)("customer"), cnt, attributes, (const char *)("id"));
	if (flag){
		QL.Showtb();
		QL.Desc((const char *)("customer"));
		cout<<endl;
	}

	cnt=0;
	//AttrInfo attributes[100];
	attributes[cnt].attrName=string("customer_id");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("book_id");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=1;
	attributes[cnt].attrName=string("quantity");
	attributes[cnt].attrType=INT;
	attributes[cnt].attrLength=10;
	attributes[cnt++].notNull=0;
	QL.CreateTable((const char *)("orders"), cnt, attributes, (const char *)("id"));
	if (flag){
		QL.Showtb();
		QL.Desc((const char *)("orders"));
		cout<<endl;
	}
}
void dropTable_test(bool flag){
	QL.DropTable("publisher");
	if(flag){
		QL.Showtb();
		QL.Desc((const char *)("publisher"));
		QL.Desc((const char *)("customer"));
		QL.Desc((const char *)("orders"));
		QL.Desc((const char *)("book"));
		cout<<endl;
	}	
	QL.DropTable("customer");
	if(flag){
		QL.Showtb();
		QL.Desc((const char *)("publisher"));
		QL.Desc((const char *)("customer"));
		QL.Desc((const char *)("orders"));
		QL.Desc((const char *)("book"));
		cout<<endl;
	}
	QL.DropTable("orders");
	if(flag){
		QL.Showtb();
		QL.Desc((const char *)("publisher"));
		QL.Desc((const char *)("customer"));
		QL.Desc((const char *)("orders"));
		QL.Desc((const char *)("book"));
		cout<<endl;
	}
	QL.DropTable("book");
	if(flag){
		QL.Showtb();
		QL.Desc((const char *)("publisher"));
		QL.Desc((const char *)("customer"));
		QL.Desc((const char *)("orders"));
		QL.Desc((const char *)("book"));
		cout<<endl;
	}
}
int main(){
	QL.Use((const char *)("orderDB"));
//	createTable_test(false);
	

	//QL.CloseDB("orderDB");
	//QL.DropDB("orderDB");
	//system("ls");
	return 0;
}