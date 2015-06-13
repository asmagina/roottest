#include "readRulesFromFile.C"

void runReadRulesFromFile()
{
   const char* filename = "test.root";
   TFile *f = TFile::Open();
   if (!f) {
      std::cerr << "Cannot open file " << filename << std::endl;
      return;
   }

   TList* list = 0;
   readRulesFromFile(f, list); 

   if (!list) {
      std::cout << "Empty list with rules." << std::endl;
      return;
   }

   list->Print();

   f.Close();
}
