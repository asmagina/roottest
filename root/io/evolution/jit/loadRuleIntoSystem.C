bool createRuleWrapper(const ROOT::TSchemaRule* ruleobj, int n, std::string& res)
{
   res  = "static void read_";
   res += ruleobj->GetTargetClass();
   res += Form("_%d", n);
   res += "( char* target, TVirtualObject *oldObj )\n";
   res += "{\n";

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
      res +=  "_t *)(target + offset_";
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
   res += "\n};\n";

   return true; 
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

   //
   // NOTE: works only for read rules
   //
   std::string wrapper;
   std::string wrapper_name = "read_";
               wrapper_name += ruleobj->GetTargetClass();
               wrapper_name += Form("_%d", rset->GetRules()->GetEntries());
   createRuleWrapper(ruleobj, rset->GetRules()->GetEntries(), wrapper);

   gROOT->ProcessLine(".rawInput");
   gROOT->ProcessLine(wrapper.c_str());
   gROOT->ProcessLine(".rawInput");
 
//   cling::runtime::gCling->declare("char* target = \"ehqeiqheoiq\";");
   cling::runtime::gCling->declare(Form("TVirtualObject obj(TClass::GetClass(\"%s\"));", // !!
                                        ruleobj->GetSourceClass()));             //

   if (cling::runtime::gCling->process(Form("%s(target, &obj)", wrapper_name.c_str())) != 
      cling::Interpreter::CompilationResult::kSuccess) 
   {
      std::cerr << "Compilation error." << std::endl;
      delete ruleobj;
      return false;   
   }

   cling::Value v;
   cling::runtime::gCling->evaluate(wrapper_name.c_str(), v);
//   ruleobj->SetReadFunctionPointer(v.getPtr());

   TString errmsg;
   if( !rset->AddRule( ruleobj, ROOT::TSchemaRuleSet::kCheckConflict, &errmsg ) ) {
      ::Warning( "TClass::AddRule", "The rule for class: \"%s\": version, \"%s\" and data members: \"%s\" has been skipped because it conflicts with one of the other rules (%s).",
                ruleobj->GetTargetClass(), ruleobj->GetVersion(), ruleobj->GetTargetString(), errmsg.Data() );
      delete ruleobj;
      return false;
   }

   return true;
}
