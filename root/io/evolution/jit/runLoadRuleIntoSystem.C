#include "readRulesFromFile.C"
#include "loadRuleIntoSystem.C"

void runLoadRuleIntoSystem()
{
   gSystem->Load("../pragma_read/libTestv2");
   const char* filename = "test.root";
   TFile *f = TFile::Open(filename);
   if (!f) {
      std::cerr << "Cannot open file " << filename << std::endl;
      return;
   }

   std::vector<std::string> vect;
   readRulesFromFile(f, vect); 

   vect.push_back("type=read sourceClass=\"AliAODForwardMult\" targetClass=\"AliAODForwardMult\" source=\"UInt_t fTriggers; Float_t fIpZ; Float_t fCentrality; UShort_t fNClusters;\" target=\"fHeader\" version=\"[-5]\" code=\"{newObj->CreateHeader(onfile.fTriggers,onfile.fIpZ,onfile.fCentrality,onfile.fNClusters);}\"");

   vect.push_back("type=read sourceClass=\"AliAODForwardMult\" targetClass=\"AliAODForwardMult\" source=\"UInt_t fTriggers; Float_t fIpZ; Float_t fCentrality; UShort_t fNClusters;\" target=\"fHeader\" version=\"[-5]\" code=\"{std::cout << \"ololo\" << std::endl;}\"");

   for (int i = 0; i < vect.size(); i++) {
      std::cout << "Adding rule " << vect[i] << " ..." << std::endl;
      loadRuleIntoSystem(vect[i].c_str());
   }
}
