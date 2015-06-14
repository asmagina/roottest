#include "readRulesFromFile.C"

void runReadRulesFromFile()
{
   const char* filename = "test.root";
   TFile *f = TFile::Open(filename);
   if (!f) {
      std::cerr << "Cannot open file " << filename << std::endl;
      return;
   }

   std::vector<std::string> vect;
   readRulesFromFile(f, vect); 

   if (vect.empty()) {
      std::cout << "No list with rules." << std::endl;
      return;
   }

   std::cout << "List of rules: " << std::endl;
   for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); ++it)
      std::cout << *it << std::endl;

   f->Close();
}
