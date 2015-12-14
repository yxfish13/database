#ifndef PARSER_H
#define PARSER_H

#include "system_manage.h"
#include <iostream>
#include <sstream>
//#include  "main.h"

//struct AttrInfo{
//                char     *attrName;   /* attribute name       */
//               AttrType attrType;    /* type of attribute    */
//                int      attrLength;  /* length of attribute  */
//                int     empty; 
//};

struct RelAttr{
	char *relName;
	char *attrName;
};

struct Value{
   	 AttrType type;         
    	void     *data;      
};

struct Condition{
	RelAttr lhsAttr;
	CompOp op;
	int      bRhsIsAttr; 
    	RelAttr  rhsAttr;    
    	Value    rhsValue;
};

class QL_manager;

void runParser(QL_manager &qlm);
void MyParse(std::istream &Input, SM_Manager &smm);

#endif
