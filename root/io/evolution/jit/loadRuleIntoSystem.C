#include "cling/Interpreter/Interpreter.h"

void extractSourceInfo(const::ROOT::TSchemaRule* ruleobj, TObjArray* types, TObjArray* dims)
{
   TString s;
   ruleobj->AsString(s);
      
   ROOT::MembersMap_t rule_values;
   std::string error_string;
   ROOT::ParseRule( s.Data(), rule_values, error_string) ;
       
   ROOT::MembersMap_t ::const_iterator it1;
   it1 = rule_values.find( "source" );

   std::list<std::pair<ROOT::TSchemaType,std::string> >           elems;
   std::list<std::pair<ROOT::TSchemaType,std::string> >::iterator it;
      
   ROOT::TSchemaRuleProcessor::SplitDeclaration( it1->second.c_str(), elems );
       
   for( it = elems.begin(); it != elems.end(); ++it ) {
      TObjString* type = new TObjString(it->first.fType.c_str());
      TObjString* dim  = new TObjString(it->first.fDimensions.c_str());
      types->Add(type);
      dims->Add(dim);
   }
}

void createRuleWrapper(const ROOT::TSchemaRule* ruleobj, std::string& res)
{
   //
   // init source
   //  
   if (ruleobj->GetSource()) {
      
      //
      // extract source types
      //
      TObjArray* types = new TObjArray();
      TObjArray* dims =  new TObjArray();

      extractSourceInfo(ruleobj, types, dims);

      res += "struct ";
      res += ruleobj->GetSourceClass();
      res += "_OnFile {\n";
   
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += ((TObjString*)((*types)[i]))->String();
         res += " &";
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String(); 
         res += ((TObjString*)((*dims)[i]))->String();
         res += ";\n";
      }

      res += ruleobj->GetSourceClass();
      res += "_OnFile";
      res += "(";
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += ((TObjString*)((*types)[i]))->String();
         res += " &";
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String(); 
         res += "_tmp";
         res += ((TObjString*)((*dims)[i]))->String();
         
         if (i != ruleobj->GetSource()->GetEntries() - 1)
            res += ",";
      }
      
      res += ") : ";
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String(); 
         res += "(";
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String(); 
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
         res += "_";
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String();
         res += " = oldObj->GetClass()->GetDataMemberOffset(\"";
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String();
         res += "\");\n";
      }
  
      res += "char* onfile_add = (char*)oldObj->GetObject();\n";
      res += ruleobj->GetSourceClass();
      res += "_OnFile onfile(";
   
      for (int i = 0; i < ruleobj->GetSource()->GetEntries(); i++) {
         res += "*( ";
         res += ((TObjString*)((*types)[i]))->String();
         res +="*)(";
         res += "onfile_add+offset_Onfile_";
         res += ruleobj->GetSourceClass();
         res += "_";
         res += ((TObjString*)((*ruleobj->GetSource())[i]))->String();
         res += ")";
         if (i != ruleobj->GetSource()->GetEntries() - 1)
            res += ", \n";
      }  
   
      res += ");\n";

      delete types;
      delete dims;
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

   TDataMember* dm;
   for (int i = 0; i < ruleobj->GetTarget()->GetEntries(); i++) {
      dm = TClass::GetClass(ruleobj->GetTargetClass())->
           GetDataMember(((TObjString*)((*ruleobj->GetTarget())[i]))->String());
      
      res += "typedef ";      
      res += dm->GetTypeName();
      res += " ";
      res += ((TObjString*)((*ruleobj->GetTarget())[i]))->String();
      res += "_t";
      if (dm->GetArrayDim()) {
         res += "[";
         res += dm->GetArrayDim();
         res += "]";
      }
      res += ";\n";
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

   TClass *cl = TClass::GetClass( ruleobj->GetTargetClass() );

   if (!cl) {
    // Create an empty emulated class for now.
      cl = gInterpreter->GenerateTClass(ruleobj->GetTargetClass(), /* emulation = */ kTRUE, /*silent = */ kTRUE);
   }

   ROOT::TSchemaRuleSet* rset = cl->GetSchemaRules( kTRUE );

   TObjArrayIter it(rset->GetRules());
   ROOT::TSchemaRule *r;
   TString s1;
   s1 = rule;

   while( TObject* obj = it.Next() ) {
      r = (ROOT::TSchemaRule *) obj;
      TString s2;
      r->AsString(s2);
  
      if (s1 == s2) {
         std::cout << "Rule already exists." << std::endl;
         return false;
      }   
   }

   //
   // compile function
   //
   std::string wrapper;
   createRuleWrapper(ruleobj, wrapper);

   std::string name;
   cling::runtime::gCling->createUniqueName(name);

   wrapper = Form("extern \"C\" void %s(char* target, TVirtualObject* oldObj) {\n %s \n };",
                  name.c_str(), wrapper.c_str());
   
   ROOT::TSchemaRule::ReadFuncPtr_t fp = (ROOT::TSchemaRule::ReadFuncPtr_t)
                                         cling::runtime::gCling->compileFunction(name.c_str(), wrapper.c_str()); 

   if (!fp) 
   {
      std::cerr << "Compilation error." << std::endl;
      cling::runtime::gCling->unload(1);
      delete ruleobj;
      return false;   
   }

   //
   // add to TSchemaRuleSet
   //
   TString errmsg;
   if( !rset->AddRule( ruleobj, ROOT::TSchemaRuleSet::kCheckConflict, &errmsg ) ) {
      ::Warning( "TClass::AddRule", "The rule for class: \"%s\": version, \"%s\" and data members: \"%s\" has been skipped because it conflicts with one of the other rules (%s).",
                ruleobj->GetTargetClass(), ruleobj->GetVersion(), ruleobj->GetTargetString(), errmsg.Data() );

      cling::runtime::gCling->unload(1);
      delete ruleobj;
      return false;
   }

   //
   // set resulting wrapping function
   //
   ruleobj->SetReadFunctionPointer(fp);

   return true;
}
