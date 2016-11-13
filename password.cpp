//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "password.h"
//---------------------------------------------------------------------------
void __fastcall AddMasterPassword(TTable *Table, String Pswd)
{
    CRTblDesc TblDesc;
    hDBIDb hDb;
    // Make sure that the table is opened and is exclusive
    if ((Table->Active == False) || (Table->Exclusive == False))
        throw EDatabaseError("Table must be opened in exclusive mode to add passwords");
    // Initialize the table descriptor
    memset(&TblDesc, 0, sizeof(CRTblDesc));
    // Place the table name in descriptor
    strcpy(TblDesc.szTblName, Table->TableName.c_str());
    // Place the table type in descriptor
    strcpy(TblDesc.szTblType, szPARADOX);
    // Master Password, Password
    strcpy(TblDesc.szPassword, Pswd.c_str());
    // Set bProtected to True
    TblDesc.bProtected = True;
    // Get the database handle from the cursor handle }
    Check(DbiGetObjFromObj((hDBIObj)Table->Handle, objDATABASE, (hDBIObj)hDb));
    // Close the table }
    Table->Close();
    // Add the master password to the Paradox table
    Check(DbiDoRestructure(hDb, 1, &TblDesc, NULL, NULL, NULL, FALSE));
    // Add the new password to the session
    Session->AddPassword(Pswd);
    // Re-Open the table
    Table->Open();
}

/* In order to add an auxilary password, the master password MUST also be
  added at the same time; even if the master password is already on the
  table!  A privelage type of prvINSDEL gives full rights without the
  ability to restructure or delete.  */
void __fastcall AddAuxPassword(TTable *Table, String Mstrpswd, String Pswd, PRVType Rights)
{
    // Specific information about the table structure, indexes, etc.
    CRTblDesc TblDesc;
    // Security descriptor
    SECDesc SDesc;
    // Uses as a handle to the database
    hDBIDb hDb;
    // crAdd
    CROpType crType;

    // Add the master password to the session
    Session->AddPassword(Mstrpswd);
    // Make sure that the table is opened and is exclusive
    if ((Table->Active == False) || (Table->Exclusive == False))
        throw EDatabaseError("Table must be opened in exclusive mode to add passwords");
    // Initialize the table and security descriptor
    memset(&TblDesc, 0, sizeof(CRTblDesc));
    memset(&SDesc, 0, sizeof(SDesc));
    crType = crADD;
    // Table privileges
    SDesc.eprvTable = Rights;
    // Family rights
    SDesc.iFamRights = NOFAMRIGHTS;
    for (int W = 1; W <= Table->FieldCount; W++)
        SDesc.aprvFld[W - 1] = prvFULL;
    // Aux Password name
    strcpy(SDesc.szPassword, Pswd.c_str());

    // Place the table name in descriptor
    strcpy(TblDesc.szTblName, Table->TableName.c_str());
    // Place the table type in descriptor
    strcpy(TblDesc.szTblType, szPARADOX);
    // Number of security definitions
    TblDesc.iSecRecCount = 1;
    // This should be crAdd
    TblDesc.pecrSecOp = &crType;
    // Attach the security descriptor to the Table Descriptor
    TblDesc.psecDesc = &SDesc;
    // Copy in the master password
    strcpy(TblDesc.szPassword, Mstrpswd.c_str());
    // Set bProtected to True
    TblDesc.bProtected = True;
    // Get the database handle from the cursor handle
    Check(DbiGetObjFromObj((hDBIObj)Table->Handle, objDATABASE, (hDBIObj)hDb));
    // Close the table
    Table->Close();
    // Add the auxilary password to the Paradox table
    Check(DbiDoRestructure(hDb, 1, &TblDesc, NULL, NULL, NULL, FALSE));
    // Re-Open the table
    Table->Open();
}


void __fastcall RemoveMasterPassword(TTable *Table)
{
    CRTblDesc TblDesc;
    hDBIDb hDb;

    // Make sure that the table is opened and is exclusive
    if ((Table->Active == False) || (Table->Exclusive == False))
        throw EDatabaseError("Table must be opened in exclusive mode to add passwords");
    // Initialize the table descriptor
    memset(&TblDesc, 0, sizeof(CRTblDesc));
    // Place the table name in descriptor
    strcpy(TblDesc.szTblName, Table->TableName.c_str());
    // Place the table type in descriptor
    strcpy(TblDesc.szTblType, szPARADOX);
    // Set bProtected to False
    TblDesc.bProtected = False;
    // Get the database handle from the cursor handle
    Check(DbiGetObjFromObj((hDBIObj)Table->Handle, objDATABASE, (hDBIObj)hDb));
    // Close the table
    Table->Close();
    // Add the master password to the Paradox table
    Check(DbiDoRestructure(hDb, 1, &TblDesc, NULL, NULL, NULL, FALSE));
    // Re-Open the table
    Table->Open();
}

void __fastcall RemoveAuxPassword(TTable *Table, String Mstrpswd, String Pswd)
{
    // Specific information about the table structure, indexes, etc.
    CRTblDesc TblDesc;
    // Security descriptor
    SECDesc SDesc;
    // Uses as a handle to the database
    hDBIDb hDb;
    // crAdd
    CROpType crType;

    // Make sure that the table is opened and is exclusive
    if ((Table->Active == False) || (Table->Exclusive == False))
        throw EDatabaseError("Table must be opened in exclusive mode to add passwords");
    // Initialize the table and security descriptor
    memset(&TblDesc, 0, sizeof(CRTblDesc));
    memset(&SDesc, 0, sizeof(SECDesc));
    crType = crDROP;
    // Descriptor number that specifies the password to remove
    SDesc.iSecNum = GetSecNoFromName(Table->DBHandle, Table->TableName, Pswd);
    if (SDesc.iSecNum == 0)
    {
        ShowMessage("Could not find auxilary password.");
        return;
    }
    // Place the table name in descriptor
    strcpy(TblDesc.szTblName, Table->TableName.c_str());
    // Place the table type in descriptor
    strcpy(TblDesc.szTblType, szPARADOX);
    // Number of security definitions
    TblDesc.iSecRecCount = 1;
    // This should be crDrop
    TblDesc.pecrSecOp = &crType;
    // Attach the security descriptor to the Table Descriptor
    TblDesc.psecDesc = &SDesc;
    // Copy in the master password
    strcpy(TblDesc.szPassword, Mstrpswd.c_str());
    // Set bProtected to True
    TblDesc.bProtected = True;
    // Get the database handle from the cursor handle
    Check(DbiGetObjFromObj((hDBIObj)Table->Handle, objDATABASE, (hDBIObj)hDb));
    // Close the table
    Table->Close();
    // Add the auxilary password to the Paradox table
    Check(DbiDoRestructure(hDb, 1, &TblDesc, NULL, NULL, NULL, FALSE));
    // Re-Open the table
    Table->Open();
}


short __fastcall GetSecNoFromName(hDBIDb hDb, String TblName, String Pswd)
{
    hDBICur hCur = NULL;
    SECDesc Sec;

    Check(DbiOpenSecurityList(hDb, TblName.c_str(), NULL, hCur));
    Check(DbiSetToBegin(hCur));
    while (DbiGetNextRecord(hCur, dbiNOLOCK, &Sec, NULL) == DBIERR_NONE)
    {
        if (CompareText(Sec.szPassword, Pswd) == 0)
        {
            // Set the Security Number to the result
            if (hCur != NULL)
                Check(DbiCloseCursor(hCur));
            return Sec.iSecNum;
        }
    }
    if (hCur != NULL)
        Check(DbiCloseCursor(hCur));
    return 0;
}
