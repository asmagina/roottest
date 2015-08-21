#include "Event_5.h"

#include "TClass.h"
#include "TStopwatch.h"

#include <iostream>
#include <vector>

using namespace std;

void PerformanceTest4() 
{
   Int_t ntest = 3000;

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

   vector<string> sourceclass;
   sourceclass.push_back("adsada");
   sourceclass.push_back("adsadeqweqa");
   sourceclass.push_back("adsadwwa");
   sourceclass.push_back("adsaeqwewqda");
   sourceclass.push_back("adsadddda");
   sourceclass.push_back("adsdfsada");
   sourceclass.push_back("adsadfsa");
   sourceclass.push_back("adsadfsfa");
   sourceclass.push_back("aada");
   sourceclass.push_back("adassss");

   TStopwatch watch;
  
   // Rules differ by source class, targets and versions
   watch.Start();
   for (Int_t i = 0; i < ntest; i++) {
      TClass::AddRule( Form("type=read sourceClass=\"%s\" "
                            "targetClass=\"Event\" "
                            "source=\"\" "
                            "include=\"<TMath.h>;\" "
                            "target=\"%s\" version=\"[%d]\" "
                            "code=\"{newObj->SetAnotherNewData(0);}\"",
                            sourceclass[i % sourceclass.size()].c_str(), target[i % target.size()].c_str(), i), kFALSE );
   }
   cout << watch.CpuTime() << endl;
}
