#include "TClass.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStreamerInfo.h"
#include <iostream>

#include "Event_3.h"

using namespace std;

void ReadNestedObjects() 
{
   TFile* f = TFile::Open("Event_2.root");
   if (!f) {
      cerr << "Cannot open file Event_2.root" << endl;
      return;
   }

   TClass::AddRule("#pragma read sourceClass=\"Track\" version=\"[2]\" targetClass=\"Track\" "
                   "source=\"\" target=\"fNewData;\" "
                   "code=\"{ newObj->fNewData = oldObj->fRandom * 100; "
                   "}\" ", kFALSE); 

   cout << "[ TEST ]" << endl;
   bool b = TClass::AddRule("#pragma read sourceClass=\"Event\" version=\"[1]\" targetClass=\"Event\" "
                            "source=\"Track fTrack;\" target=\"fTrack;\" "
                            "code=\"{ if( oldObj->fTrack->GetVersion() == 2 ) "
                            "{ "
                            "newObj->fFlag += oldObj->fTrack->GetMember<double>( \"fNpoint\" )"
                            "oldObj->fTrack->Load( \"newObj->fTrack\" ); "
                            "} "
                            "}\" ", kFALSE); 

   if (b)
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
