#include "parser.h"
#include "system_manage.h"
#include "record_manager.h"

using namespace std;
class QL_manager
{
	Byte *pdata[10];
	RM_Manager &rmm;
	SM_Manager &smm;
	RC GetValue(int   nRelations, const char * const relations[],int offset,int length,AttrType attribute,const char *relName,Byte *data);
	string GenString(char *a, int n){
	  int i=0; 
	  for(; i<n; i++)
	    if (a[i]==0)
	      break;
	  return string(a,i);
	}
	bool flag;
public:
	QL_manager(RM_Manager& rmm1,SM_Manager &smm1);
	~QL_manager();
	void CreateDB(const char *dbName);
	void DropDB(const char *dbName);
	void Use(const char *dbName);
	void Showtb();
	void CreateTable(const char *relName, int attrCount, AttrInfo *attributes, const char *primaryKey);
	void DropTable(const char *relName);
	void Desc(const char *relName);


	void Delete (const char *relName, int nConditions, const Condition conditions[]);
	void Select (int nSelAttrs, const RelAttr selAttrs[], int   nRelations, const char * const relations[], int   nConditions, const Condition conditions[]);   
	void Insert (const char *relName, int nValues, const Value values[]);
	void Update(const char *relName, const RelAttr &updAttr, const int isValue, const RelAttr &rhsRelAttr, const Value &rhsValue, int nConditions, const Condition conditions[]);
};