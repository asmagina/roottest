#include "readFile.C"

void runReadFile(const char* filename, const char* classname)
{
   std::cout << "Reading test1.root\n" << std::endl;
   readFile("test.root", "TTree");
}
