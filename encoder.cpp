//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "encoder.h"
#include "fileprotect.h"
#include "about.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
    : TForm(Owner)
{
    csp = CStringProtect(8192,8192*4,0,480,960);
    SummaryTable=NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Button1Click(TObject *Sender)
{
    if(Edit1->Text == "" || Edit2->Text =="" || ListBox1->Count<=0) {
        StatusBar1->Panels->Items[0]->Text = "Error : Please input all fields";
        return;
    }
    Button1->Enabled = false;
    ListBox2->Clear();
    ProgressBar2->Min=0;
    ProgressBar2->Max=ListBox1->Count;
    ProgressBar2->Position=0;
    ProgressBar2->Step=1;

    AnsiString dbSummaryTableName = "summary";
    SummaryTable = new TTable(0);
    SummaryTable->Active = false;
    SummaryTable->Exclusive = true;
    SummaryTable->DatabaseName = IncludeTrailingPathDelimiter(Edit2->Text)+Edit1->Text;
    if(!DirectoryExists(SummaryTable->DatabaseName)) {
        if(!CreateDir(SummaryTable->DatabaseName)) {
            StatusBar1->Panels->Items[0]->Text = "Problem creating directory : " + SummaryTable->DatabaseName;
            Timer1->Enabled = false;
            return;
        }
    }
    SummaryTable->TableName = dbSummaryTableName;
    SummaryTable->TableType = ttParadox;
    SummaryTable->FieldDefs->Clear();

    TFieldDef *NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "AccountNumber";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "RefNumber";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "BillDate";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "SummaryLabel";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "LeftValue";
    NewField->Size=30;

    NewField = SummaryTable->FieldDefs->AddFieldDef(); // define first field
    NewField->DataType = ftString;
    NewField->Name = "RightValue";
    NewField->Size=30;
    if(!SummaryTable->Exists) {
        SummaryTable->CreateTable();
    } else {
        TQuery *q = new TQuery(0);
        q->DatabaseName = SummaryTable->DatabaseName;
        q->SQL->Clear();
        q->SQL->Add("drop index "+dbSummaryTableName+".iacnum");
        try {
            q->ExecSQL();
        } catch(...) {
        }
        q->SQL->Clear();
        q->SQL->Add("drop index "+dbSummaryTableName+".irefnum");
        try {
            q->ExecSQL();
        } catch(...) {
        }
        q->SQL->Clear();
        q->SQL->Add("drop index "+dbSummaryTableName+".isummary");
        try {
            q->ExecSQL();
        } catch(...) {
        }
        delete q;

    }
    SummaryTable->Exclusive=false;
    SummaryTable->Open();

    Timer1->Enabled = true;

}
//---------------------------------------------------------------------------
void __fastcall TForm3::StatusBar1DrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
  if (Panel == StatusBar->Panels->Items[1]) {
    ProgressBar1->Top = Rect.Top;
    ProgressBar1->Left = Rect.Left;
    ProgressBar1->Width = Rect.Right - Rect.Left - 15;
    ProgressBar1->Height = Rect.Bottom - Rect.Top;
  }

  if (Panel == StatusBar->Panels->Items[2]) {
    ProgressBar2->Top = Rect.Top;
    ProgressBar2->Left = Rect.Left;
    ProgressBar2->Width = Rect.Right - Rect.Left - 15;
    ProgressBar2->Height = Rect.Bottom - Rect.Top;
  }

}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button4Click(TObject *Sender)
{
    ProgressBar1->StepIt();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormCreate(TObject *Sender)
{
    ProgressBar1->Parent = StatusBar1;
    long style = GetWindowLong(ProgressBar1->Handle, GWL_EXSTYLE);
    style &= ~WS_EX_STATICEDGE;
    SetWindowLong(ProgressBar1->Handle, GWL_EXSTYLE,style);
    ProgressBar1->Top=0;
    ProgressBar1->Left  = StatusBar1->Panels->Items[0]->Width;
    ProgressBar1->Width = StatusBar1->Panels->Items[1]->Width-4;

    ProgressBar2->Parent = StatusBar1;
    style = GetWindowLong(ProgressBar2->Handle, GWL_EXSTYLE);
    style &= ~WS_EX_STATICEDGE;
    SetWindowLong(ProgressBar2->Handle, GWL_EXSTYLE,style);
    ProgressBar1->Top=0;
    ProgressBar1->Left  = StatusBar1->Panels->Items[1]->Width;
    ProgressBar1->Width = StatusBar1->Panels->Items[2]->Width-4;

    SendMessage(ProgressBar2->Handle, PBM_SETBARCOLOR, 0, clRed);
    running = flag = false;
    cnt=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button3Click(TObject *Sender)
{
    AnsiString Directory;
    if (SelectDirectory("Select target directory", "", Directory)) {
        Edit2->Text = Directory;
    }

}
//---------------------------------------------------------------------------
void __fastcall TForm3::About1Click(TObject *Sender)
{
    AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Exit1Click(TObject *Sender)
{
    Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Add3Click(TObject *Sender)
{
    if(OpenDialog1->Execute()) {
        ListBox1->AddItem(OpenDialog1->FileName, Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Add2Click(TObject *Sender)
{
    ListBox1->DeleteSelected();
}
//---------------------------------------------------------------------------


void __fastcall TForm3::Timer1Timer(TObject *Sender)
{
    if(ListBox1->Count!=0 && Edit2->Text !="") {
        if(!running && !flag) {
            cnt = 0;
            flag = false;
            running=true;

            AnsiString DirectoryName;
            DirectoryName = IncludeTrailingPathDelimiter(IncludeTrailingPathDelimiter(Edit2->Text)+Edit1->Text)+ ChangeFileExt(ExtractFileName(ListBox1->Items->Strings[0]), ".cd");
            if(!DirectoryExists(DirectoryName)) {
                if(!CreateDir(DirectoryName)) {
                    StatusBar1->Panels->Items[0]->Text = "Problem creating directory : " + DirectoryName;
                    Timer1->Enabled = false;
                    return;
                }
            }
            fpdo = new TFileProtectDo(false, ListBox1->Items->Strings[0], DirectoryName, csp, ProgressBar1, StatusBar1->Panels->Items[0], &flag, SummaryTable);
        } else if(running && flag){
            running = false;
            flag=false;
            ListBox2->AddItem(ListBox1->Items->Strings[0], Sender);
            ListBox1->ItemIndex=0;
            ListBox1->DeleteSelected();
            ProgressBar2->StepIt();
        } else {
            cnt+=Timer1->Interval;
            Label7->Caption = cnt;
            Label7->Caption = Label7->Caption +" miliseconds elapsed";

        }
    } else {
        Timer1->Enabled = false;
       TQuery *aquery = new TQuery(0);
       aquery->DatabaseName = SummaryTable->DatabaseName;
       AnsiString dbSummaryTableName = SummaryTable->TableName;

       aquery->SQL->Add("Create Index iacnum on "+dbSummaryTableName+" AccountNumber;");
       aquery->ExecSQL();
       aquery->SQL->Clear();

       aquery->SQL->Add("Create Index irefnum on "+dbSummaryTableName+" RefNumber;");
       aquery->ExecSQL();
       aquery->SQL->Clear();

       aquery->SQL->Add("Create Index isummary on "+dbSummaryTableName+" SummaryLabel;");
       aquery->ExecSQL();
       aquery->SQL->Clear();
       delete aquery;

        if(SummaryTable!=NULL) {
            delete SummaryTable;
            SummaryTable = NULL;
            Button1->Enabled = true;
            StatusBar1->Panels->Items[0]->Text ="Processing Done ...";
        }
    }

}
//---------------------------------------------------------------------------



