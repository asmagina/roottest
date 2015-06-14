bool loadRuleIntoSystem(const char* rule)
{
   ROOT::TSchemaRule *ruleobj = new ROOT::TSchemaRule();
   if (! ruleobj->SetFromRule( rule ) ) {
      delete ruleobj;
      return false;
   }

   R__LOCKGUARD(gInterpreterMutex);

   // check compilation
   // create wrapper

   if (! createRuleWrapper(ruleobj, )) {
      delete ruleobj;
      return false;
   }

   TClass *cl = TClass::GetClass( ruleobj->GetTargetClass() );
     
   if (!cl) {
      // Create an empty emulated class for now.
      cl = gInterpreter->GenerateTClass(ruleobj->GetTargetClass(), /* emulation = */ kTRUE, /*silent = */ kTRUE);
   }

   ROOT::TSchemaRuleSet* rset = cl->GetSchemaRules( kTRUE );

   TString errmsg;
   if( !rset->AddRule( ruleobj, ROOT::TSchemaRuleSet::kCheckConflict, &errmsg ) ) {
      ::Warning( "TClass::AddRule", "The rule for class: \"%s\": version, \"%s\" and data members: \"%s\" has been skipped because it conflicts with one of the other rules (%s).",
                ruleobj->GetTargetClass(), ruleobj->GetVersion(), ruleobj->GetTargetString(), errmsg.Data() );
      delete ruleobj;
      return false;
   }

   return true;
}
