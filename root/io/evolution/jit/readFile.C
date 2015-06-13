#include "readRulesFromFile.C"
#include "loadRuleIntoSystem.C"

void readFile(const char* filename, const char* classname)
{
   TFile *f = TFile::Open(filename);
   if (!f) {
      std::cerr << "Cannot open file " << filename << std::endl;
      return;
   }

   if (!gROOT->GetClass(classname)) {
      std::cerr << "Cannot find class " << classname << " in file " << filename << std::endl;
      return;  
   }

   std::cout << "[" << classname << "] StreamerInfo info before: " << std::endl;
   gROOT->GetClass(classname)->GetStreamerInfos()->ls();

   TList* list = 0;
   readRulesFromFile(f, list);

   TIter next(list);
   while(TObjString* rule = next())
      loadRuleIntoSystem(rule->String().Data());
   
   std::cout << "[" << classname << "] Streamer info after:" << std::endl;
   gROOT->GetClass("Event")->GetStreamerInfos()->ls();

   f.Close();
}
