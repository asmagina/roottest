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

   // read nested object with transformation of its field
   cout << "[ TEST 1 ]" << endl;
   TClass::AddRule("#pragma read sourceClass=\"EventHeader\" version=\"[1]\" targetClass=\"EventHeader\" "
                   "source=\"Int_t fDate\" target=\"fDate\" "
                   "code=\"{ fDate.SetSec(onfile.fDate + 100000); fDate.SetNanoSec(100); "
                   "}\" ", kFALSE); 
   if ( TClass::AddRule("#pragma read sourceClass=\"Event\" version=\"[-3]\" targetClass=\"Event\" "
                        "source=\"EventHeader fEvtHdr\" target=\"fEvtHdr\" "
                        "code=\"{ if (onfile.fEvtHdr->GetClassVersion() == 1) {"
                        " fFlag = onfile.fEvtHeader->GetMember<Int_t>(fDate_id) % 2; "
                        " onfile.fEvtHdr->Load( fEvtHdr ); } }\" ", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule was successfully load into system ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be successfully load into system ] " << endl;

   // read a collection with transformation of its elements
   cout << "[ TEST 2 ]" << endl;
   TClass::AddRule("#pragma read sourceClass=\"Track\" version=\"[-2]\" targetClass=\"Track\" "
                   "source=\"\" target=\"fNewData\" "
                   "code=\"{ fNewData = 100500; }\" ", kFALSE); 

   if ( TClass::AddRule("#pragma read sourceClass=\"Event\" version=\"[-3]\" targetClass=\"Event\" "
                        "source=\"TClonesArray* fTracks;\" target=\"fTracks\" "
                        "code=\"{ for (UInt_t i = 0; i < onfile.fTracks->GetSize(); i++) {"
                        "onfile.fTracks->At(i)->Load(fTracks[i]); "
                        "}}\" ", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule was successfully load into system ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be successfully load into system ] " << endl;

/*   Event* event = new Event();
   TTree* t = (TTree*)f->Get("T");
   t->SetBranchAddress("event", &event);
   t->GetEntry(0);
   event->ls();
*/
   f->Close();
}
