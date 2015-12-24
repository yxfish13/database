#ifndef SYSTEM_MANAGE_H
#define SYSTEM_MANAGE_H

#include "pages_file.h"
#include "record_manager.h"
#include <iostream>
#define SYS_SEP "/"
#define SYSTEM_CATALOG_NAME "__SYSTEM_CATALOG.bin"
#define ATTRIBUTE_CATALOG_NAME "__ATTRIBUTE_CATALOG.bin"

const int SYSTEM_TUPLE_LENGTH = RELNAME_LENGTH + 4 + 4 + 4;
const int ATTRIBUTE_TUPLE_LENGTH = RELNAME_LENGTH + ATTRNAME_LENGTH + 4 + 4 + 4 + 4 + 4 ;
using namespace std;
void SM_PrintError(RC rc);

//Put data into pData and forward pData by the length of the data
void PutIntData(Byte*& pData, const int x);
void PutFloatData(Byte*& pData, const float x);
void PutStringData(Byte*& pData, const char *s, const int len);

// Used by SM_Manager::CreateTable
struct AttrInfo {
  std::string     attrName;           // Attribute name
  AttrType attrType;            // Type of attribute
  int      attrLength;          // Length of attribute
  int notNull;
};

// in system catalog: relName, tupleLength, attrCount, indexCount
// in attribute catalog: relName, attrName, offset, attrType, attrLenth, indexNo, Nullable

class SM_Manager {
  
  RM_Manager &rmm;

  std::string db_dir;
  RM_FileHandle system_fh;
  RM_FileHandle attribute_fh;
  string GenString(char *a, int n){
    int i=0; 
    for(; i<n; i++)
      if (a[i]==0)
        break;
    return string(a,i);
  }
 public:
  SM_Manager  (RM_Manager &rmm);  // Constructor
  ~SM_Manager ();                                  // Destructor
  RC CreateDB    (const std::string &s);
  RC OpenDb      (const char *dbName);                // Open database
  RC CloseDb     ();                                  // Close database
  RC DropDB       (const std::string &s);             // En sure it is closed
  RC CreateTable (const char *relName,                // Create relation
		  int        attrCount,
		  AttrInfo   *attributes);
  RC DropTable   (const char *relName);               // Destroy relation

  /*
  RC CreateIndex (const char *relName,                // Create index
		  const char *attrName);
  RC DropIndex   (const char *relName,                // Destroy index
		  const char *attrName);
  */

  /*
    RC Load        (const char *relName,                // Load utility
		  const char *fileName);
  */
  RC Help        ();                                  // Help for database
  RC Help        (const char *relName);               // Help for relation
  RC Print       (const char *relName);               // Print relation
  RC ShowTable   ();               // Print table
  RC GetFh(const char *relName,RM_FileHandle& fh);
  RC GetAttrInfo(const char *relName,const char *AttrName,int &offset,int &length,AttrType &attrtype);
  RC Getoffset(const char *relName,int *offset,int *length);
  void GetsystemHandle(RM_FileHandle *&system_fh1){system_fh1=&system_fh;}
  void GetattributeHandle(RM_FileHandle *&attribute_fh1){attribute_fh1=&attribute_fh;}
};
#endif
