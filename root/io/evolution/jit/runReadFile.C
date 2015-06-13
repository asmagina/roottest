#include "readOldFileWithJIT.C"

void runReadFile()
{
   std::cout << "Reading test1.root\n" << std::endl;
   readOldFileWithJIT("test.root", "AliAODForwardMult");

//   std::cout << "\n\n--------------------------------------------------\n\n" << std::endl;
}
