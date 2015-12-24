#include "parser_internal.h"
#include "string.h"

int mk_attr_infos(NODE *list, int max, AttrInfo attrInfos[], char *primaryKey);
void mk_rel_attr(NODE *node, RelAttr &relAttr);
void parse_type(char *type_string, AttrType *type);
int mk_rel_attrs(NODE *node, int max, RelAttr relattrs[]);
int mk_relations(NODE *list, int max, char *relations[]);
int mk_conditions(NODE *list, int max, Condition conditons[]);
void mk_value(NODE *node, Value &value);
int mk_values(NODE *list, int max, Value valus[]);

#include "query.h"

extern QL_manager *pQlm;

int interp(NODE *n)
{
	switch(n -> kind){
               case N_CREATEDB:
               {
                      pQlm->CreateDB(n->u.CREATEDB.dbname);
                      break;
               }
               case N_DROPDB:
               {
                      pQlm->DropDB(n->u.DROPDB.dbname);
                      break;
               }
               case N_USEDB:
               {
                      pQlm->Use(n->u.USEDB.dbname);
                      break;
               }
               case N_CREATETABLE:
               {
                      int nattrs;
                      AttrInfo attrInfos[10];
                      char primaryKey[30];

                      nattrs = mk_attr_infos(n->u.CREATETABLE.attrlist,10,attrInfos,primaryKey);

                      pQlm->CreateTable(n->u.CREATETABLE.relname,nattrs,attrInfos,primaryKey);

                      break;
               }
               case N_DROPTABLE:
               {
                      pQlm->DropTable(n->u.DROPTABLE.relname);
                      break;
               }
               case N_SHOWTABLES:
               {
                      pQlm->Showtb();
                      break;
               }
               case N_DESC:
               {
                      pQlm->Desc(n->u.DESC.relname);
                      break;
               }
		case  N_QUERY:
		{
			int nRelAttrs = 0;
			RelAttr relAttrs[10];
			int nRelations = 0;
			char *relations[10];
			int nConditions = 0;
			Condition conditions[10];

			nRelAttrs = mk_rel_attrs(n->u.QUERY.relattrlist, 10, relAttrs);
			nRelations = mk_relations(n->u.QUERY.rellist, 10, relations);
			nConditions = mk_conditions(n->u.QUERY.conditionlist, 10, conditions);

			pQlm->Select(nRelAttrs, relAttrs, nRelations, relations, nConditions, conditions);
			break;
		}
              case N_INSERT:
              {
                        int nValues = 0;
                        Value values[10];

                        nValues = mk_values(n->u.INSERT.valuelist, 10, values);

                        pQlm->Insert(n->u.INSERT.relname, nValues, values);
                        break;
              }
              case N_DELETE:
              {
                        int nConditions = 0;
                        Condition conditions[10];

                        nConditions = mk_conditions(n->u.DELETE.conditionlist,10,conditions);

                        pQlm->Delete(n->u.DELETE.relname,nConditions,conditions);
                        break;
              }
              case N_UPDATE:
              {
                        RelAttr relAttr;

                        Value rhsValue;
                        RelAttr rhsRelAttr;
                        int isValue;

                        int nConditions = 0;
                        Condition conditions[10];

                        mk_rel_attr(n->u.UPDATE.relattr,relAttr);

                        struct node *rhs = n->u.UPDATE.relorvalue;
                        if(rhs->u.RELATTR_OR_VALUE.relattr) {
                                mk_rel_attr(rhs->u.RELATTR_OR_VALUE.relattr,rhsRelAttr);
                                isValue = 0;
                        }
                        else{
                                mk_value(rhs->u.RELATTR_OR_VALUE.value, rhsValue);
                                isValue = 1;
                        }

                        nConditions = mk_conditions(n->u.UPDATE.conditionlist,10,conditions);
                        pQlm->Update(n->u.UPDATE.relname, relAttr,isValue, rhsRelAttr, rhsValue, nConditions, conditions);
                        break;
              }
	}
} 

int mk_attr_infos(NODE *list, int max, AttrInfo attrInfos[], char *primaryKey)
{
        int i;
        AttrType type;
        NODE *attr;

        for(i = 0; list != NULL; ++i, list = list -> u.LIST.next) {

            if(list->u.LIST.next == NULL)
            {
                    attr = list->u.LIST.curr;
                    memcpy(primaryKey , attr->u.ATTRTYPE.attrname, 30);
                    break;
            }

            attr = list->u.LIST.curr;
            parse_type(attr -> u.ATTRTYPE.type, &type);

            attrInfos[i].attrName = attr -> u.ATTRTYPE.attrname;
            attrInfos[i].attrType = type;
            attrInfos[i].notNull = attr->u.ATTRTYPE.empty;

            if(attr->u.ATTRTYPE.value == 0)
            {
                    switch(type){
                            case INT:
                                    attrInfos[i].attrLength = sizeof(int);
                                    break;
                            case FLOAT:
                                    attrInfos[i].attrLength = sizeof(float);
                                    break;
                    }
            }
            else 
                    attrInfos[i].attrLength = attr->u.ATTRTYPE.value;
        }

        return i;
}

void parse_type(char *type_string, AttrType *type)
{
        char c;

        sscanf(type_string, "%c", &c);

        switch(c){
                case 'i':
                        *type = INT;
                        break;
                case 'f':
                case 'r':
                        *type = FLOAT;
                        break;
                case 'v':
                        *type = STRING;
                        return;
                default:
                        return;
        }
}

int mk_rel_attrs(NODE *list, int max, RelAttr relAttrs[])
{
	int i;

   	for(i = 0; list != NULL; ++i, list = list -> u.LIST.next){
		mk_rel_attr(list->u.LIST.curr, relAttrs[i]);
   	}

   	return i;
}

void mk_rel_attr(NODE *node, RelAttr &relAttr)
{
	relAttr.relName = node->u.RELATTR.relname;
	relAttr.attrName = node->u.RELATTR.attrname;
}

int mk_relations(NODE *list, int max, char *relations[])
{
   	int i;
   	NODE *current;

  	 for(i = 0; list != NULL; ++i, list = list -> u.LIST.next){
		current = list -> u.LIST.curr;
      		relations[i] = current->u.RELATION.relname;
   	}

   	return i;
}

int mk_conditions(NODE *list, int max, Condition conditions[])
{
   	int i;
   	NODE *current;

   	for(i = 0; list != NULL; ++i, list = list -> u.LIST.next){
		current = list -> u.LIST.curr;
      		conditions[i].lhsAttr.relName = current->u.CONDITION.lhsRelattr->u.RELATTR.relname;
      		conditions[i].lhsAttr.attrName = current->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
      		conditions[i].op = current->u.CONDITION.op;
      		if (current->u.CONDITION.rhsRelattr) {
         			conditions[i].bRhsIsAttr = 1;
         			conditions[i].rhsAttr.relName = current->u.CONDITION.rhsRelattr->u.RELATTR.relname;
        			conditions[i].rhsAttr.attrName =  current->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
      		}
      		else {
        			conditions[i].bRhsIsAttr = 0;
         			mk_value(current->u.CONDITION.rhsValue, conditions[i].rhsValue);
     		 }
   	}

   	return i;
}

void mk_value(NODE *node, Value &value)
{
   	value.type = node->u.VALUE.type;
   	switch (value.type) {
      	case INT:
         		value.data = (void *)&node->u.VALUE.ival;
         		break;
      	case FLOAT:
         		value.data = (void *)&node->u.VALUE.rval;
         		break;
      	case STRING:
         		value.data = (void *)node->u.VALUE.sval;
        		break;
   	}
}

int mk_values(NODE *list, int max, Value values[])
{
        int i;
        for(i = 0;list != NULL;++i,list = list->u.LIST.next){
                mk_value(list->u.LIST.curr, values[i]);
        }

        return i;
}