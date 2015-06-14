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

   std::vector<std::string> vect;
   readRulesFromFile(f, vect);

   for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); ++it)
      loadRuleIntoSystem(it->c_str());
   
   std::cout << "[" << classname << "] Streamer info after:" << std::endl;
   gROOT->GetClass(classname)->GetStreamerInfos()->ls();

   f->Close();
}
