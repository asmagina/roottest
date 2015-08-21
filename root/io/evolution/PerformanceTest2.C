#include "Event_5.h"

#include "TClass.h"
#include "TStopwatch.h"

#include <iostream>
#include <vector>

using namespace std;

void PerformanceTest2() 
{
   Int_t ntest = 5000;

   vector<string> target;
   target.push_back("fAnotherNewData");
   target.push_back("fFlag");
   target.push_back("fNseg");
   target.push_back("fTemperature");
   target.push_back("fNvertex");
   target.push_back("fEventName");
   target.push_back("fType");
   target.push_back("fMeasures");
   target.push_back("fMatrix");
   target.push_back("fClosestDistance");

   TStopwatch watch;
  
   // Rules differ by targets and versions
   watch.Start();
   for (Int_t i = 0; i < ntest; i++) {
      TClass::AddRule( Form("type=read sourceClass=\"Event\" "
                           "targetClass=\"Event\" "
                           "source=\"\" "
                           "include=\"<TMath.h>;\" "
                           "target=\"%s\" version=\"[%d]\" "
                           "code=\"{newObj->SetAnotherNewData(0);}\"", target[i % target.size()].c_str(), i), kFALSE );
   }
   cout << watch.CpuTime() << endl;
}
