#include "readFile.C"

void runReadFile()
{
   std::cout << "Reading test1.root\n" << std::endl;
   readFile("test.root", "AliAODForwardMult");

//   std::cout << "\n\n--------------------------------------------------\n\n" << std::endl;
}
