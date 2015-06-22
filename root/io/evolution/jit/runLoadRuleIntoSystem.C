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

   vect.push_back("type=read sourceClass=\"AliAODForwardMult\" targetClass=\"AliAODForwardMult\" source=\"UInt_t fTriggers; Float_t fIpZ; Float_t fCentrality; UShort_t fNClusters;\" target=\"fHeader\" version=\"[-5]\" code=\"{newObj->CreateHeader(onfile.fTriggers,onfile.fIpZ,onfile.fCentrality,onfile.fNClusters);}\"");

//   for (int i = 0; i < vect.size(); i++) {
//      std::cout << "Adding rule " << vect[i] << " ..." << std::endl;
      loadRuleIntoSystem(vect[2].c_str());
//   }
}
