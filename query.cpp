#include "query.h"
#include <iostream>
#include <cstring>
//using namespace std;

QL_manager::QL_manager(RM_Manager& rmm1,SM_Manager &smm1):rmm(rmm1),smm(smm1){
	flag=false;
}
void QL_manager::CreateDB(const char *dbName){
	smm.CreateDB(string(dbName));
}

void QL_manager::DropDB (const char *dbName){
	smm.DropDB(string (dbName));
}
void QL_manager::Use (const char *dbName){
	if (flag){
		smm.CloseDb();
	}
	smm.OpenDb( dbName);
	flag=true;
}
void QL_manager::Showtb(){
	smm.ShowTable();
}
void QL_manager::CreateTable(const char *relName,                // Create relation
		  int        attrCount,
		  AttrInfo   *attributes,const char * key){
	//cout<<"adf"<<endl;
	smm.CreateTable (relName,                // Create relation
		  attrCount,
		  attributes);
}
void QL_manager::DropTable(const char *relName){
	smm.DropTable(relName);
}
void QL_manager::Desc(const char *relName){
	smm.Print(relName);
}
void QL_manager::Delete (const char *relName, int nConditions, const Condition conditions[]){
	RM_FileHandle delete1_fh;
	smm.GetFh(relName,delete1_fh);
	int offset[100],length[100];
	AttrType attrType[100];
	for (int i=0;i<nConditions;i++)
		smm.GetAttrInfo(relName,conditions[i].lhsAttr.attrName,offset[i],length[i],attrType[i]);
	RM_FileScan delete_scanner(delete1_fh, attrType[0], length[0], offset[0], EQ_OP, 
                                (void*)conditions[0].rhsValue.data, length[0]);
  	RC ret;
  	RM_Record record;
  	RID rid;
  	while( (ret=delete_scanner.GetNextRec(record)) != NOT_FOUND ){
  		bool flag=true;
  		if (flag){
    		record.GetRid(rid);
   	 		delete1_fh.DeleteRec(rid);
   	 	}
  	}
	rmm.CloseFile(delete1_fh);
}
void QL_manager::Select (int nSelAttrs, const RelAttr selAttrs1[], int   nRelations, const char * const relations[], int   nConditions, const Condition conditions[]){
	
	//return;
	const RelAttr * selAttrs=selAttrs1;
	RelAttr sel_Temp[1000];
	if (selAttrs[0].attrName[0]=='*') nSelAttrs=0;
 
	if (nSelAttrs==0){
		for (int i=0;i<nRelations;i++){
			RM_FileHandle * attributefh_addr;
			smm.GetattributeHandle(attributefh_addr);
			RM_FileScan attribute_scanner(*attributefh_addr, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                                (void*)relations[i], strlen(relations[i]));
			  RC ret;
			  RM_Record record;
			  Byte *mem;
			  while( (ret=attribute_scanner.GetNextRec(record)) != NOT_FOUND ){
			  //  record.GetRid(rid);
			  //  attribute_fh.DeleteRec(rid);
			    record.GetData(mem);
			    sel_Temp[nSelAttrs].relName=(char*)relations[i];
			    sel_Temp[nSelAttrs].attrName=(char *)(mem+RELNAME_LENGTH);
			    nSelAttrs++;
			    //cout<<GenString((char *)(mem+RELNAME_LENGTH),ATTRNAME_LENGTH)<<"("<<*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+8)<<")"<<*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+4)<<endl;
			  }
		}
		selAttrs=sel_Temp;
	}
	//const RelAttr * selAttrs=selAttrs1;
	if(nRelations==1){
			int offset[100],length[100];
			AttrType attrType[100];
			for (int i=0;i<nConditions;i++){
				smm.GetAttrInfo(relations[0],conditions[i].lhsAttr.attrName,offset[i*2+0],length[i*2],attrType[i*2+0]);
				if (conditions[i].bRhsIsAttr)
					smm.GetAttrInfo(relations[0],conditions[i].rhsAttr.attrName,offset[i*2+1],length[i*2+1],attrType[i*2+1]);
			}
			for (int i = nConditions*2 ; i < nConditions*2 + nSelAttrs ; i++ )
				smm.GetAttrInfo(relations[0],selAttrs[i-nConditions*2].attrName,offset[i],length[i],attrType[i]);
			RM_FileHandle S1_fh;
			smm.GetFh(relations[0],S1_fh);
			RM_FileScan S1_scanner(S1_fh, attrType[nConditions], length[nConditions], offset[nConditions], EQ_OP, 
		                                NULL, 0);
		  	RC ret;
  			RM_Record record;
  			while( (ret=S1_scanner.GetNextRec_NoComp(record)) != NOT_FOUND ){
	  			record.GetData(pdata[0]);
	   	 		bool flag=true;
	   	 		for (int i=0;i<nConditions;i++){
	   	 			Byte pdata1[500];
					Byte pdata2[500];
					Byte *data1=pdata1;
					Byte *data2=pdata2;
					GetValue(nRelations,relations,offset[i*2+0],length[i*2],attrType[i*2+0],relations[0],data1);
					if (conditions[i].bRhsIsAttr)
						GetValue(nRelations,relations,offset[i*2+1],length[i*2+1],attrType[i*2+1],relations[0],data2);
					else
					{
						attrType[i*2+1]=conditions[i].rhsValue.type;
						length[i*2+1]=length[i*2];
						data2=(Byte*) conditions[i].rhsValue.data;
					}
					flag = flag && _Validate(data1,data2,attrType[i*2],attrType[i*2+1],conditions[i].op,length[i*2],length[i*2+1]);
					// if (attrType[i*2]!=attrType[i*2+1]){
					// 	flag=false;
					// }else{
					// 	if (attrType[i*2]==INT){
					// 		if((*(int*)data1)!=(*(int*)data2)) flag=false;
						
					// 	}
					// 	if (attrType[i*2]==FLOAT){
					// 		if((*(float*)data1)!=(*(float*)data2)) flag=false;
					// 	}
					// 	if (attrType[i*2]==STRING){
					// 		if(GenString((char*)data1,length[i*2])!=GenString((char*)data2,length[i*2+1])) flag=false;
					// 	}
					// }
	   	 		}
	   	 		if (flag){
	   	 			Byte *data;
	   	 			for (int i=0;i<nSelAttrs;i++){
	   	 				GetValue(nRelations,relations,offset[i+nConditions*2],length[i+nConditions*2],attrType[i+nConditions*2],relations[0],data);
	   	 				if (i) cout<<",";
	   	 				else cout<<"(";
	   	 				if (attrType[i+nConditions*2]==INT) cout<<*(int *)data;
	   	 				if (attrType[i+nConditions*2]==FLOAT) cout<<*(float *)data;
	   	 				if (attrType[i+nConditions*2]==STRING) for (int j=0;j<length[i+nConditions*2];j++) cout<<(char)data[j];
	   	 			}
	   	 			cout<<")"<<endl;
	   	 		}
		  	}
			rmm.CloseFile(S1_fh);
	}else{
			int offset[100],length[100];
			AttrType attrType[100];
			for (int i=0;i<nConditions;i++){
				smm.GetAttrInfo(conditions[i].lhsAttr.relName,conditions[i].lhsAttr.attrName,offset[i*2+0],length[i*2],attrType[i*2+0]);
				if (conditions[i].bRhsIsAttr)
					smm.GetAttrInfo(conditions[i].rhsAttr.relName,conditions[i].rhsAttr.attrName,offset[i*2+1],length[i*2+1],attrType[i*2+1]);
			}
			for (int i = nConditions*2 ; i < nConditions*2 + nSelAttrs ; i++ )
				smm.GetAttrInfo(selAttrs[i-nConditions*2].relName,selAttrs[i-nConditions*2].attrName,offset[i],length[i],attrType[i]);
			SelectDFS(nSelAttrs, selAttrs, nRelations, relations, nConditions,conditions,0,offset,length,attrType);
			// RM_FileHandle S1_fh,S2_fh;
			// //cout<<"in"<<endl;
			// //cout<<relations[0]<<endl;
			// //cout<<relations[1]<<endl;
			// smm.GetFh(relations[0],S1_fh);
			// //cout<<"out"<<endl;
			// RM_FileScan S1_scanner(S1_fh, attrType[nConditions], length[nConditions], offset[nConditions], EQ_OP, 
		 //                                NULL, 0);
			// RC ret,ret2;
  	// 		RM_Record record,record2;
  	// 		while( (ret=S1_scanner.GetNextRec_NoComp(record)) != NOT_FOUND ){
	  // 			record.GetData(pdata[0]);
	  // 			//cout<<"1"<<endl;
	  // 			smm.GetFh(relations[1],S2_fh);
	  // 			//cout<<"2"<<endl;
			// 	RM_FileScan S2_scanner(S2_fh, attrType[nConditions], length[nConditions], offset[nConditions], EQ_OP, 
		 //                                NULL, 0);
			// 	while( (ret2=S2_scanner.GetNextRec_NoComp(record2)) != NOT_FOUND ){
			// 		record2.GetData(pdata[1]);
			// 		//cout<<"3"<<endl;
		 //   	 		bool flag=true;
		 //   	 		for (int i=0;i<nConditions;i++){
		 //   	 			Byte pdata1[500];
			// 			Byte pdata2[500];
			// 			Byte *data1=pdata1;
			// 			Byte *data2=pdata2;
			// 			GetValue(nRelations,relations,offset[i*2+0],length[i*2],attrType[i*2+0],conditions[i].lhsAttr.relName,data1);
			// 			if (conditions[i].bRhsIsAttr)
			// 				GetValue(nRelations,relations,offset[i*2+1],length[i*2+1],attrType[i*2+1],conditions[i].rhsAttr.relName,data2);
			// 			else
			// 			{
			// 				attrType[i*2+1]=conditions[i].rhsValue.type;
			// 				length[i*2+1]=length[i*2];
			// 				data2=(Byte*) conditions[i].rhsValue.data;
			// 			}
			// 			if (attrType[i*2]!=attrType[i*2+1]){
			// 				flag=false;
			// 			}else{
			// 				if (attrType[i*2]==INT){
			// 					if((*(int*)data1)!=(*(int*)data2)) flag=false;
			// 				}
			// 				if (attrType[i*2]==FLOAT){
			// 					if((*(float*)data1)!=(*(float*)data2)) flag=false;
			// 				}
			// 				if (attrType[i*2]==STRING){
			// 					if(GenString((char*)data1,length[i*2])!=GenString((char*)data2,length[i*2+1])) flag=false;
			// 				}
			// 			}
		 //   	 		}
		 //   	 		if (flag){
		 //   	 			Byte *data;
		 //   	 			for (int i=0;i<nSelAttrs;i++){
		 //   	 				GetValue(nRelations,relations,offset[i+nConditions*2],length[i+nConditions*2],attrType[i+nConditions*2],selAttrs[i].relName,data);
		 //   	 				if (i) cout<<",";
		 //   	 				else cout<<"(";
		 //   	 				if (attrType[i+nConditions*2]==INT) cout<<*(int *)data;
		 //   	 				if (attrType[i+nConditions*2]==FLOAT) cout<<*(float *)data;
		 //   	 				if (attrType[i+nConditions*2]==STRING) for (int j=0;j<length[i+nConditions*2];j++) cout<<(char)data[j];
		 //   	 			}
		 //   	 			cout<<")"<<endl;
		 //   	 		}
	  //  	 		}
	  //  	 		rmm.CloseFile(S2_fh);
		 //  	}
			// rmm.CloseFile(S1_fh);
	}
//	return OK;
}
RC QL_manager::GetValue(int   nRelations, const char * const relations[],int offset,int length,AttrType attribute,const char *relName,Byte *&data){
	for (int i=0;i<nRelations;i++){
		if (string(relations[i])==string(relName)){
			data = pdata[i]+offset;
		//	if (attribute==0)
		//	cout<<"in get value :"<<*(int*)data<<" "<<*(int*)pdata[i]<<endl;
		}
	}
	return OK;
}
void QL_manager::Insert (const char *relName, int nValues, const Value values[]){
	RM_FileHandle Insert_fh;
	smm.GetFh(relName,Insert_fh);
	int offset[100],length[100];
	smm.Getoffset(relName,offset,length);
	Byte pData[500];
  	Byte *nowp=pData;
	for (int i=0;i<nValues;i++){
		nowp=pData;
		if (values[i].type==INT)
			memcpy(nowp+offset[i], values[i].data , 4);
		if (values[i].type==FLOAT)
			memcpy(nowp+offset[i], values[i].data , 4);
		if (values[i].type==STRING){
			for (int j=0,ls=strlen((char*)values[i].data);j<length[i];j++){
				//memcpy(pData+offset[i], values[i].data , length[i]);
				if (j<ls)
					nowp[offset[i]+j]=((char*)values[i].data)[j];
				else
					nowp[offset[i]+j]=0;
			}
		}
	}
	RID rid;
	//cout<<*(int*)pData<<endl;
	Insert_fh.InsertRec(pData, rid);
	rmm.CloseFile(Insert_fh);
}
void QL_manager::Update(const char *relName, const RelAttr &updAttr, const int isValue, const RelAttr &rhsRelAttr, const Value &rhsValue, int nConditions, const Condition conditions[]){
	RM_FileHandle delete1_fh;
	smm.GetFh(relName,delete1_fh);
	int offset[100],length[100];
	AttrType attrType[100];
	for (int i=0;i<nConditions;i++)
		smm.GetAttrInfo(relName,conditions[i].lhsAttr.attrName,offset[i],length[i],attrType[i]);
	RM_FileScan delete_scanner(delete1_fh, attrType[0], length[0], offset[0], EQ_OP, 
                                (void*)conditions[0].rhsValue.data, length[0]);
  	RC ret;
  	RM_Record record,rec;
  	int offset_up,length_up;
			AttrType attrType_up;
  	smm.GetAttrInfo(relName,updAttr.attrName,offset_up,length_up,attrType_up);
  	while( (ret=delete_scanner.GetNextRec(record)) != NOT_FOUND ){
  		bool flag=true;
  		Byte* mem;
  		RID rid;
  		record.GetData(mem);
  		record.GetRid(rid);
  		if (flag){
  			if (attrType_up==INT)
				memcpy(mem+offset_up, rhsValue.data , 4);
			if (attrType_up==FLOAT)
				memcpy(mem+offset_up, rhsValue.data , 4);
			if (attrType_up==STRING){
				for (int j=0,ls=strlen((char*)rhsValue.data);j<length_up;j++){
					//memcpy(pData+offset[i], values[i].data , length[i]);
					if (j<ls)
						mem[offset_up+j]=((char*)rhsValue.data)[j];
					else
						mem[offset_up+j]=0;
				}
			}
  			rec=RM_Record(mem, delete1_fh.GetRecordSize(), rid);
   	 		delete1_fh.UpdateRec(rec);
   	 	}
  	}
	rmm.CloseFile(delete1_fh);
}
QL_manager::~QL_manager(){
	if (flag){
		smm.CloseDb();
	}
}


void QL_manager::SelectDFS(int nSelAttrs, const RelAttr selAttrs[], int   nRelations, const char * const relations[], int   nConditions, const Condition conditions[],int deep,int *offset,int *length,AttrType *attrType){
	if (deep==nRelations){
		bool flag=true;
		   	 		for (int i=0;i<nConditions;i++){
		   	 			Byte pdata1[500];
						Byte pdata2[500];
						Byte *data1=pdata1;
						Byte *data2=pdata2;
						GetValue(nRelations,relations,offset[i*2+0],length[i*2],attrType[i*2+0],conditions[i].lhsAttr.relName,data1);
						if (conditions[i].bRhsIsAttr)
							GetValue(nRelations,relations,offset[i*2+1],length[i*2+1],attrType[i*2+1],conditions[i].rhsAttr.relName,data2);
						else
						{
							attrType[i*2+1]=conditions[i].rhsValue.type;
							length[i*2+1]=length[i*2];
							data2=(Byte*) conditions[i].rhsValue.data;
						}
						flag = flag && _Validate(data1,data2,attrType[i*2],attrType[i*2+1],conditions[i].op,length[i*2],length[i*2+1]);
		   	 		}
		   	 		if (flag){
		   	 			Byte *data;
		   	 			for (int i=0;i<nSelAttrs;i++){
		   	 				GetValue(nRelations,relations,offset[i+nConditions*2],length[i+nConditions*2],attrType[i+nConditions*2],selAttrs[i].relName,data);
		   	 				if (i) cout<<",";
		   	 				else cout<<"(";
		   	 				if (attrType[i+nConditions*2]==INT) cout<<*(int *)data;
		   	 				if (attrType[i+nConditions*2]==FLOAT) cout<<*(float *)data;
		   	 				if (attrType[i+nConditions*2]==STRING) for (int j=0;j<length[i+nConditions*2];j++) cout<<(char)data[j];
		   	 			}
		   	 			cout<<")"<<endl;
		   	 		}
		return;
	}
	RM_FileHandle S1_fh;
	smm.GetFh(relations[deep],S1_fh);
	RM_FileScan S1_scanner(S1_fh, attrType[nConditions], length[nConditions], offset[nConditions], EQ_OP, 
		                                NULL, 0);
	RC ret;
  	RM_Record record;
  	while( (ret=S1_scanner.GetNextRec_NoComp(record)) != NOT_FOUND ){
	  	record.GetData(pdata[deep]);
	  	SelectDFS(nSelAttrs, selAttrs, nRelations, relations, nConditions,conditions,deep+1,offset,length,attrType);
	}
	rmm.CloseFile(S1_fh);
}


bool QL_manager::_Validate(const int op1, const int op2,CompOp comp_op)const{
  switch(comp_op){
  case EQ_OP:
    return op1 == op2;
    break;
  case LT_OP:
    return op1 < op2;
    break;
  case GT_OP:
    return op1>op2;
    break;
  case LE_OP:
    return op1<=op2;
    break;
  case GE_OP:
    return op1>=op2;
    break;
  case NE_OP:
    return op1!=op2;
    break;
  default:
    ;
  }
  return 0;
}

bool QL_manager::_Validate(const std::string &op1, const std::string &op2,CompOp comp_op)const{
  switch(comp_op){
  case EQ_OP:
    return op1 == op2;
    break;
  case LT_OP:
    return op1 < op2;
    break;
  case GT_OP:
    return op1>op2;
    break;
  case LE_OP:
    return op1<=op2;
    break;
  case GE_OP:
    return op1>=op2;
    break;
  case NE_OP:
    return op1!=op2;
    break;
  default:
    ;
  }
  return 0;
}

bool QL_manager::_Validate(const float op1, const float op2,CompOp comp_op)const{
  float d=op1-op2;
  switch(comp_op){
  case EQ_OP:
    return d<EPS && d>-EPS;
    break;
  case LT_OP:
    return d<-EPS;
    break;
  case GT_OP:
    return d>EPS;
    break;
  case LE_OP:
    return d<EPS;
    break;
  case GE_OP:
    return d>-EPS;
    break;
  case NE_OP:
    return d>EPS || d<-EPS;
    break;
  default:
    ;
  }
  return 0;
}