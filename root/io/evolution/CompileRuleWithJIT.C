#include "TClass.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TROOT.h"
#include "TSchemaRuleSet.h"
#include "TTree.h"
#include <iostream>

#include "Event_5.h"

#include "TStreamerInfo.h"

using namespace std;

void CompileRuleWithJIT() 
{
   TFile* f = TFile::Open("Event_2.root");
   if (!f) {
      cerr << "Cannot open file Event_2.root" << endl;
      return;
   }

   // provides parsing error
   cout << "[ TEST 1 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"\" "
                         "source=\"Event\" "
                         "target=\"fNewData\" version=\"[21-34]\" "
                         "code=\"{newObj->fNewData = 23; }\"", kFALSE) ) 
      cout << "[ SUCCEED ] [ As expected, rule provides parsing error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide parsing error ]" << endl;

   // wrong targets
   cout << "[ TEST 2 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Eventt\" "
                         "source=\"\" "
                         "target=\"fNewData\" version=\"[-1]\" "
                         "code=\"{newObj->fNewData = 20; }\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule provides consistency check error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide consistency check error ]" << endl;
      
   // wrong targets
   cout << "[ TEST 3 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"\" "
                         "target=\"fNewNewData\" version=\"[-1]\" "
                         "code=\"{newObj->fNewNewData = 90; }\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule provides consistency check error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide consistency check error ]" << endl;

   // compilation error
   cout << "[ TEST 4 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"\" "
                         "target=\"fAnotherNewData\" version=\"[-1]\" "
                         "code=\"{newObj->SetAnotherNewData(ttTMath::Abs(-90))}\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule provides compilation error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide compilation error ]" << endl;

   // rule is ok
   cout << "[ TEST 5 ]" << endl;
   if ( TClass::AddRule("type=read sourceClass=\"Event\" "
                        "targetClass=\"Event\" "
                        "source=\"\" "
                        "include=\"<TMath.h>;\" "
                        "target=\"fAnotherNewData\" version=\"[-1]\" "
                        "code=\"{newObj->SetAnotherNewData(0);}\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule is ok ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be ok ]" << endl;

   // rule is ok
   cout << "[ TEST 6 ]" << endl;
   if ( TClass::AddRule("type=read sourceClass=\"Event\" "
                        "targetClass=\"Event\" "
                        "source=\"Float_t fNewData;\" "
                        "target=\"fAnotherNewData\" version=\"[2]\" "
                        "code=\"{newObj->SetAnotherNewData(onfile.fNewData + 150);}\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule is ok ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be ok ]" << endl;

   // rule is in conflict
   cout << "[ TEST 7 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"\" "
                         "include=\"<TMath.h>;\" "
                         "target=\"fAnotherNewData\" version=\"[2]\" "
                         "code=\"{newObj->SetAnotherNewData(10);}\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule is in conflict ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be in conflict ]" << endl;

   // rule is ok
   cout << "[ TEST 8 ]" << endl;
   if ( TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"UInt_t fFlag;\" "
                         "target=\"fFlag\" checksum=\"[4227322039]\" "
                         "code=\"{fFlag = onfile.fFlag + 1000;}\"", kFALSE) )
      cout << "[ SUCCEED ] [ As expected, rule is ok ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be ok ]" << endl;

   // rule is in conflict
   cout << "[ TEST 9 ]" << endl;
   TClass* cl = TClass::GetClass("Event");
   Int_t nrules = cl->GetSchemaRules()->GetRules()->GetEntries(); 

   Bool_t flag = TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"UInt_t fFlag;\" "
                         "target=\"fFlag\" checksum=\"[4227322039]\" "
                         "code=\"{fFlag = onfile.fFlag * 70;}\"", kFALSE);

   if (flag == kTRUE && cl->GetSchemaRules()->GetRules()->GetEntries() ==  nrules)
      cout << "[ SUCCEED ] [ As expected, rule is already exists ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be already loaded in a system ]" << endl;

   Event* event = new Event();
   TTree* t = (TTree*)f->Get("T");
   t->SetBranchAddress("event", &event);
   t->GetEntry(0);
   event->ls();

   f->Close();
}
