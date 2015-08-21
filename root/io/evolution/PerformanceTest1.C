#include "Event_5.h"

#include "TClass.h"
#include "TStopwatch.h"

#include <iostream>

using namespace std;

void PerformanceTest1() 
{
   Int_t ntest = 3000;
   TStopwatch watch;
  
   // Rules differ only by version
   watch.Start();
   for (Int_t i = 0; i < ntest*10; i += 10) {
      TClass::AddRule( Form("type=read sourceClass=\"Event\" "
                            "targetClass=\"Event\" "
                            "source=\"\" "
                            "include=\"<TMath.h>;\" "
                            "target=\"fAnotherNewData\" version=\"[%d-%d]\" "
                            "code=\"{newObj->SetAnotherNewData(0);}\"", i, i+9), kFALSE );
   }
   cout << watch.CpuTime() << endl;
}
