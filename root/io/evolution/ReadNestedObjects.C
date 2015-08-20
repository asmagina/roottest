#include "TClass.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStreamerInfo.h"
#include <iostream>

#include "Event_6.h"

using namespace std;

void ReadNestedObjects() 
{
   TFile* f = TFile::Open("Event_2.root");
   if (!f) {
      cerr << "Cannot open file Event_2.root" << endl;
      return;
   }

   TClass::AddRule("#pragma read sourceClass=\"EventHeader\" version=\"[1]\" targetClass=\"EventHeader\" "
                   "include=\"<iostream>;\" "
                   "source=\"Int_t fDate;\" target=\"fDate\" "
                   "code=\"{ fDate.SetSec(onfile.fDate + 100000); fDate.SetNanoSec(100); "
                   "}\" ", kFALSE); 

   if ( TClass::AddRule("#pragma read sourceClass=\"Event\" version=\"[-3]\" targetClass=\"Event\" "
                        "include=\"<iostream>;\" "
                        "source=\"EventHeader fEvtHdr;\" target=\"fEvtHdr, fFlag\" "
                        "code=\"{if (onfile.fEvtHdr->GetClassVersion() == 1) {  "
                        "fFlag = onfile.fEvtHdr->GetMember<Int_t>(onfile.fEvtHdr->GetId(\"fDate\")) % 2; "
                        "onfile.fEvtHdr->Load(&fEvtHdr, \"EventHeader\"); } }\" ", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule was successfully load into system ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be successfully load into system ] " << endl;

   Event* event = new Event();
   TTree* t = (TTree*)f->Get("T");
   t->SetBranchAddress("event", &event);
   t->GetEntry(0);
   event->ls();

   f->Close();
}
