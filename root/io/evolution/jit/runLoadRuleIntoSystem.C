#include "readRulesFromFile.C"
#include "loadRuleIntoSystem.C"

void runLoadRuleIntoSystem()
{
   const char* filename = "test.root";
   TFile *f = TFile::Open(filename);
   if (!f) {
      std::cerr << "Cannot open file " << filename << std::endl;
      return;
   }

   std::vector<std::string> vect;
   readRulesFromFile(f, vect); 

   for (int i = 0; i < vect.size(); i++) {
      std::cout << "Adding rule " << vect[i] << " ..." << std::endl;
      loadRuleIntoSystem(vect[i].c_str());
   }
}
