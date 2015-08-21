#include "Event_5.h"

#include "TClass.h"
#include "TStopwatch.h"

#include <iostream>
#include <vector>

using namespace std;

void PerformanceTest3() 
{
   Int_t ntest = 5000;

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
  
   // Rules differ by source class and versions
   watch.Start();
   for (Int_t i = 0; i < ntest; i++) {
      TClass::AddRule( Form("type=read sourceClass=\"%s\" "
                            "targetClass=\"Event\" "
                            "source=\"\" "
                            "include=\"<TMath.h>;\" "
                            "target=\"fAnotherNewData\" version=\"[%d]\" "
                            "code=\"{newObj->SetAnotherNewData(0);}\"", 
                            sourceclass[i % sourceclass.size()].c_str(), i), kFALSE );
   }
   cout << watch.CpuTime() << endl;
}
