#include "TClass.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include <iostream>

#include "Event_5.h"

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
                         "target=\"fNewData; \" version=\"[34-21]\" "
                         "code=\"{newObj->fNewData = 23; }\"") ) 
      cout << "[ SUCCEED ] [ As expected, rule provides parsing error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide parsing error ]" << endl;

   // wrong targets
   cout << "[ TEST 2 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Eventt\" "
                         "source=\"\" "
                         "target=\"fNewData;\" version=\"[-1]\" "
                         "code=\"{newObj->fNewData = 20; }\"") )
      cout << "[ SUCCEED ] [ As expected, rule provides consistency check error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide consistency check error ]" << endl;
      
   // wrong targets
   cout << "[ TEST 3 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"\" "
                         "target=\"fNewData; fNewNewData;\" version=\"[-1]\" "
                         "code=\"{newObj->fNewData =80; newObj->fNewNewData = 90; }\"") )
      cout << "[ SUCCEED ] [ As expected, rule provides consistency check error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide consistency check error ]" << endl;

   // compilation error
   cout << "[ TEST 4 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"\" "
                         "target=\"fNewData;\" version=\"[-1]\" "
                         "code=\"{newObj->fNewData = TMath::Abs(-90);}\"") )
      cout << "[ SUCCEED ] [ As expected, rule provides compilation error ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to provide compilation error ]" << endl;

   // rule is ok
   cout << "[ TEST 5 ]" << endl;
   if ( TClass::AddRule("type=read sourceClass=\"Event\" "
                        "targetClass=\"Event\" "
                        "source=\"\" "
                        "include=\"#include <TMath.h>;\" "
                        "target=\"fNewData;\" version=\"[-1]\" "
                        "code=\"{newObj->fNewData = 0;}\"") )
      cout << "[ SUCCEED ] [ As expected, rule is ok ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be ok ]" << endl;

   // rule is ok
   cout << "[ TEST 6 ]" << endl;
   if ( TClass::AddRule("type=read sourceClass=\"Event\" "
                        "targetClass=\"Event\" "
                        "source=\"Float_t fNewData;\" "
                        "target=\"fAnotherNewData;\" version=\"[2]\" "
                        "code=\"{newObj->fAnotherNewData = oldObj->fNewData + 150;}\"") )
      cout << "[ SUCCEED ] [ As expected, rule is ok ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be ok ]" << endl;

   // rule is ok
   cout << "[ TEST 7 ]" << endl;
   if ( TClass::AddRule("type=read sourceClass=\"EventHeader\" "
                        "targetClass=\"EventHeader\" "
                        "source=\"\" "
                        "target=\"fDetectorN;\" version=\"[-1]\" "
                        "code=\"{newObj->fDetectorN = -10;}\"") )
      cout << "[ SUCCEED ] [ As expected, rule is ok ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be ok ]" << endl;
   
   // rule is in conflict
   cout << "[ TEST 8 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"EventHeader\" "
                         "targetClass=\"EventHeader\" "
                         "source=\"\" "
                         "target=\"fDetectorN;\" version=\"[1-]\" "
                         "code=\"{newObj->fDetectorN = -20;}\"") )
      cout << "[ SUCCEED ] [ As expected, rule is in conflict ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be in conflict ]" << endl;
   
   // rule is in conflict
   cout << "[ TEST 9 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"Event\" "
                         "targetClass=\"Event\" "
                         "source=\"\" "
                         "include=\"#include <TMath.h>;\" "
                         "target=\"fNewData;\" version=\"[-2]\" "
                         "code=\"{newObj->fNewData = 10;}\"") )
      cout << "[ SUCCEED ] [As expected, rule is in conflict ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be in conflict ]" << endl;

   // rule is in conflict
   cout << "[ TEST 10 ]" << endl;
   if ( !TClass::AddRule("type=read sourceClass=\"EventHeader\" "
                         "targetClass=\"Event\" "
                         "source=\"Float_t fRun;\" "
                         "target=\"fAnotherNewData;\" version=\"[-2]\" "
                         "code=\"{newObj->fAnotherNewData = oldObj->fRun;}\"") )
      cout << "[ SUCCEED ] [ As expected, rule is in conflict ]" << endl;
   else 
      cout << "[ FAIL ] [ Rule is expected to be in conflict ]" << endl;

   Event* event = new Event();
   TTree* t = (TTree*)f->Get("T");
   t->SetBranchAddress("event", &event);
   t->GetEntry(0);
//   event->ls();

   f->Close();
}
