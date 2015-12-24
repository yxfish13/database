#include "system_manage.h"
#include <iostream>
//using namespace std;


void SM_PrintError(RC rc){

}

void PutIntData(Byte *& pData, const int x){
  memcpy(pData, &x, 4);
  pData+=4;
}

void PutFloatData(Byte *& pData, const float x){
  memcpy(pData, &x, 4);
  pData+=4;
}

void PutStringData(Byte *& pData, const char *s, const int len){
  int s_len=strlen(s);
  for(int i=0; i<len; i++){
    if (i>=s_len)
      *pData=0;
    else
      *pData=s[i];
    pData++;
  }
}

//----------------------------SM_MANAGER----------------------------------

SM_Manager::SM_Manager(RM_Manager &rmm2)
  :rmm(rmm2){
  
}

SM_Manager::~SM_Manager(){

}

RC SM_Manager::CreateDB(const string &db_name){
  string db_dir(db_name+SYS_SEP);
  system((string("mkdir ")+db_name).c_str());
  rmm.CreateFile( (db_dir+SYSTEM_CATALOG_NAME).c_str(),  SYSTEM_TUPLE_LENGTH);
  rmm.CreateFile( (db_dir+ATTRIBUTE_CATALOG_NAME).c_str(), ATTRIBUTE_TUPLE_LENGTH);
  return OK;
}

RC SM_Manager::OpenDb(const char *db_name){
  db_dir=string(db_name)+SYS_SEP;

  RC ret=rmm.OpenFile((db_dir+SYSTEM_CATALOG_NAME).c_str(), system_fh);
  if (ret!=OK){
    SM_PrintError(ret);
    return SYSTEM_CATALOG_OPEN_ERROR;
  }

  ret=rmm.OpenFile((db_dir+ATTRIBUTE_CATALOG_NAME).c_str(), attribute_fh);
  if (ret!=OK){
    SM_PrintError(ret);
    return ATTRIBUTE_CATALOG_OPEN_ERROR;
  }
  
  return OK;
}

RC SM_Manager::CloseDb(){
  
  RC ret=rmm.CloseFile(system_fh);
  if (ret!=OK){
    SM_PrintError(ret);
    return SYSTEM_CATALOG_CLOSE_ERROR;
  }

  ret=rmm.CloseFile(attribute_fh);
  if (ret!=OK){
    SM_PrintError(ret);
    return ATTRIBUTE_CATALOG_CLOSE_ERROR;
  }

  return OK;
}
RC SM_Manager::DropDB       (const std::string &s){
  system((string("mv ")+db_dir+string(" dust/")+db_dir).c_str());
  return OK;
}

RC SM_Manager::CreateTable(const char *rel_name, int attr_count, AttrInfo *attr_info){
  
  //for system catalog
  int tuple_length=0;
  int index_count=0;
  for(int i=0; i<attr_count; i++){
    tuple_length += attr_info[i].attrLength;
    if (!attr_info[i].notNull)
      tuple_length++;
  }

  Byte pData[500];
  Byte *nowp=pData;
  PutStringData(nowp, rel_name, RELNAME_LENGTH);
  PutIntData(nowp, tuple_length);
  PutIntData(nowp, attr_count);
  PutIntData(nowp, index_count);

  RID rid;
  system_fh.InsertRec(pData, rid);
  //------------------- 


  //for attribute catalog
  for(int i=0, now_offset=0; i<attr_count; i++){
    RID rid;
    nowp=pData;
    PutStringData(nowp, rel_name, RELNAME_LENGTH);
    PutStringData(nowp, attr_info[i].attrName.c_str(), ATTRNAME_LENGTH);

    PutIntData(nowp, now_offset);
    now_offset+=attr_info[i].attrLength;
    if (!attr_info[i].notNull)
      now_offset++;

    PutIntData(nowp, (int)attr_info[i].attrType);
    PutIntData(nowp, attr_info[i].attrLength);
    PutIntData(nowp, -1); //for index
    PutIntData(nowp, attr_info[i].notNull);
    attribute_fh.InsertRec(pData, rid);
  }
  attribute_fh.ForcePages();
  system_fh.ForcePages();
  //-------------------

  //for table file
  RC ret=rmm.CreateFile((db_dir+string(rel_name)).c_str(), tuple_length);
  if (ret!=OK){
    return TABLE_CREATE_ERROR;
  }
  //------------------

  return OK;
}

RC SM_Manager::DropTable(const char *relName){
  string rel_name(relName);
  
  //drop in system catalog
  RM_FileScan system_scanner(system_fh, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                             (void*)relName, strlen(relName));
  RC ret;
  RM_Record record;
  ret=system_scanner.GetNextRec(record);
  if (ret==NOT_FOUND){
    return NOT_FOUND;
  }
  RID rid;
  record.GetRid(rid);
  system_fh.DeleteRec(rid);
  //------------------------

  //drop in attribute catalog
  RM_FileScan attribute_scanner(attribute_fh, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                                (void*)relName, strlen(relName));
  while( (ret=attribute_scanner.GetNextRec(record)) != NOT_FOUND ){
    record.GetRid(rid);
    attribute_fh.DeleteRec(rid);
  }
  system((string("mv -f ")+db_dir+string(relName)+" "+db_dir+"dust/"+string(relName)).c_str());

  attribute_fh.ForcePages();
  system_fh.ForcePages();
  
  //-----------------------
  
  return OK;
}

RC SM_Manager::Help(){
  return OK;
}

RC SM_Manager::Help(const char *rel_name){
  return OK;
}

RC SM_Manager::Print(const char *relName){
  RM_FileScan attribute_scanner(attribute_fh, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                                (void*)relName, strlen(relName));
  RC ret;
  RM_Record record;
  Byte *mem;
  while( (ret=attribute_scanner.GetNextRec(record)) != NOT_FOUND ){
  //  record.GetRid(rid);
  //  attribute_fh.DeleteRec(rid);
    record.GetData(mem);
    cout<<GenString((char *)(mem+RELNAME_LENGTH),ATTRNAME_LENGTH)<<"("<<*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+8)<<")"<<*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+4)<<endl;
  }
  return OK;
}

RC SM_Manager::ShowTable   (){
  RM_FileScan system_scanner(system_fh, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                                NULL, 0);
  //cout<<"1"<<endl;
  RC ret;
  RM_Record record;
  Byte *mem;
  //cout<<"1.1"<<endl;
  while( (ret=system_scanner.GetNextRec_NoComp(record)) != NOT_FOUND ){
  //  record.GetRid(rid);
  //  attribute_fh.DeleteRec(rid);
   // cout<<"2"<<endl;
    record.GetData(mem);
   // cout<<"3"<<endl;
    cout<<GenString((char *)(mem),RELNAME_LENGTH)<<"("<<*(int*)(mem+RELNAME_LENGTH)<<")"<<endl;
    //cout<<"4"<<endl;
  }
  return OK;
}               // Print table
RC SM_Manager::GetFh(const char *relName,RM_FileHandle& fh){
  RC ret=rmm.OpenFile((db_dir+string(relName)).c_str(), fh);
  return ret;
}
RC SM_Manager::GetAttrInfo(const char *relName,const char *AttrName,int &offset,int &length,AttrType &attrtype){
  RM_FileScan attribute_scanner(attribute_fh, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                                (void*)relName, strlen(relName));
  RC ret;
  RM_Record record;
  Byte *mem;
  while( (ret=attribute_scanner.GetNextRec(record)) != NOT_FOUND ){
  //  record.GetRid(rid);
  //  attribute_fh.DeleteRec(rid);
    record.GetData(mem);
    if (GenString((char *)(mem+RELNAME_LENGTH),ATTRNAME_LENGTH)==GenString((char *)AttrName,strlen(AttrName))){
      offset=*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH);
      length=*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+8);
      attrtype=(AttrType)*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+4);
    }
  }
  return OK;
}
RC SM_Manager::Getoffset(const char *relName,int *offset,int *length){
  RM_FileScan attribute_scanner(attribute_fh, STRING, RELNAME_LENGTH, 0, EQ_OP, 
                                (void*)relName, strlen(relName));
  RC ret;
  RM_Record record;
  Byte *mem;
  int cnt=0;
  while( (ret=attribute_scanner.GetNextRec(record)) != NOT_FOUND ){
  //  record.GetRid(rid);
  //  attribute_fh.DeleteRec(rid);
    record.GetData(mem);
    //if (genString((char *)(mem+RELNAME_LENGTH),ATTRNAME_LENGTH)==genString(AttrName,strlen(AttrName))){
      offset[cnt]=*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH);
      length[cnt]=*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+8);
      cnt++;
    //  attrtype=*(int*)(mem+RELNAME_LENGTH+ATTRNAME_LENGTH+4);
    //}
  }
  return OK;
}

//----------------------------SM_MANAGER----------------------------------
