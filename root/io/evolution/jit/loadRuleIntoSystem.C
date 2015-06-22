#include "cling/Interpreter/Interpreter.h"

void createRuleWrapper(const ROOT::TSchemaRule* ruleobj, std::string& res)
{
   //
   // init source
   //  
   if (ruleobj->GetSource()) {
       res += "struct ";
       res += ruleobj->GetSourceClass();
       res += " {\n";
   
       for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += "int &"; //! temporary fix for type extraction
         res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String(); 
         res += ";\n";
      }

      res += ruleobj->GetSourceClass();
      res += "(";
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += "int &"; //! temporary fix for type extraction
         res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String(); 
         res += "_tmp";
         
         if (i != ruleobj->GetSource()->GetEntries() - 1)
            res += ",";
      }
      
      res += ") : ";
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String(); 
         res += "(";
         res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String(); 
         res += "_tmp)";         
         if (i != ruleobj->GetSource()->GetEntries() - 1)
            res += ", ";
      }
      
      res += "\n";
      res += "{}\n";     

      res += "};\n";

      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += "static Long_t offset_Onfile_";
         res += ruleobj->GetSourceClass();
         res += " = oldObj->GetClass()->GetDataMemberOffset(\"";
         res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
         res += "\");\n";
      }
  
      res += "char* onfile_add = (char*)oldObj->GetObject();\n";
      res += ruleobj->GetSourceClass();
      res += " onfile(";
   
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += "*( int*)(";  //! temporary fix for type extraction
         res += "(onfile_add+offset_Onfile_";
         res += ruleobj->GetSourceClass();
         res += "_";
         res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
         res += ");\n";
      }  
   
      res += ");\n";
   } 

   //
   // init target
   //
   res += "static TClassRef cls(\"";
   res += ruleobj->GetTargetClass();
   res += "\");\n";

   for (int i = 0; i < ruleobj->GetTarget()->GetEntries(); i++) {
      res += "static Long_t offset_";
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += " = cls->GetDataMemberOffset(\"";
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += "\");\n";
   }

   for (int i = 0; i < ruleobj->GetTarget()->GetEntries(); i++) {
      res += "typedef ";
      res += "int "; //! temporary fix for type extraction
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += "_t;\n";
   }

   for (int i = 0; i < ruleobj->GetTarget()->GetEntries(); i++) {
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += "_t& ";
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += " = *(";
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res +=  "_t *)(target+offset_";
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += ");\n";
   }

   res += ruleobj->GetTargetClass();
   res += "* newObj = (";
   res += ruleobj->GetSourceClass();
   res += "*)target;\n";

   //
   // user's code
   //
   res += ruleobj->GetCode();
}

bool loadRuleIntoSystem(const char* rule)
{
   ROOT::TSchemaRule *ruleobj = new ROOT::TSchemaRule();
   if (! ruleobj->SetFromRule( rule ) ) {
      std::cerr << "Rule parsing error." << std::endl;
      delete ruleobj;
      return false;
   }

   R__LOCKGUARD(gInterpreterMutex);

   //
   // compile function
   //
   TClass *cl = TClass::GetClass( ruleobj->GetTargetClass() );

   if (!cl) {
    // Create an empty emulated class for now.
      cl = gInterpreter->GenerateTClass(ruleobj->GetTargetClass(), /* emulation = */ kTRUE, /*silent = */ kTRUE);
   }

   std::string wrapper;
   createRuleWrapper(ruleobj, wrapper);

   std::string name;
   cling::runtime::gCling->createUniqueName(name);

   wrapper = Form("extern \"C\" void %s(char* target, TVirtualObject* obj) {\n %s \n };",
                  name.c_str(), wrapper.c_str());
   
   ROOT::TSchemaRule::ReadFuncPtr_t fp = (ROOT::TSchemaRule::ReadFuncPtr_t)
                                         cling::runtime::gCling->compileFunction(name.c_str(), wrapper.c_str()); 

   std::string test_name;
   cling::runtime::gCling->createUniqueName(test_name);
   
   cling::runtime::gCling->enableRawInput(true);
   
   gROOT->ProcessLine(Form("void %s() {\n", test_name.c_str()));
   gROOT->ProcessLine("TBufferFile buf(TBuffer::kWrite);\n");
   gROOT->ProcessLine(Form("buf.WriteClassBuffer(TClass::GetClass(\"%s\"), new %s());", 
                                        ruleobj->GetTargetClass(), ruleobj->GetTargetClass()));
   gROOT->ProcessLine(Form("TVirtualObject obj(TClass::GetClass(\"%s\"));",
                                        ruleobj->GetSourceClass()));
   gROOT->ProcessLine(Form("%s(buf.Buffer(), &obj);", name.c_str()));
   gROOT->ProcessLine("} ");

   cling::runtime::gCling->enableRawInput(false);

   cling::Interpreter::CompilationResult result = 
      cling::runtime::gCling->process(Form("%s()", test_name.c_str()));

   cling::runtime::gCling->unload(2);

   if (result != cling::Interpreter::CompilationResult::kSuccess)
   {
      std::cerr << "Compilation error." << std::endl;
      cling::runtime::gCling->unload(1);
      delete ruleobj;
      return false;   
   }

   ROOT::TSchemaRuleSet* rset = cl->GetSchemaRules( kTRUE );

   //
   // check identical rules
   //
   TObjArrayIter it(rset->GetRules());
   ROOT::TSchemaRule *r;

   TString s1, s2;
   ruleobj->AsString(s1);
   while( TObject* obj = it.Next() ) {
      r = (ROOT::TSchemaRule *) obj;
      r->AsString(s2);
  
      if (s1 == s2) {
         std::cout << "Rule already exists." << std::endl;
         delete ruleobj;
         return false;
      }   
   }

   //
   // add to TSchemaRuleSet
   //
   TString errmsg;
   if( !rset->AddRule( ruleobj, ROOT::TSchemaRuleSet::kCheckConflict, &errmsg ) ) {
      ::Warning( "TClass::AddRule", "The rule for class: \"%s\": version, \"%s\" and data members: \"%s\" has been skipped because it conflicts with one of the other rules (%s).",
                ruleobj->GetTargetClass(), ruleobj->GetVersion(), ruleobj->GetTargetString(), errmsg.Data() );
      delete ruleobj;
      return false;
   }

   //
   // set resulting wrapping function
   //
   ruleobj->SetReadFunctionPointer(fp);
 
   return true;
}
