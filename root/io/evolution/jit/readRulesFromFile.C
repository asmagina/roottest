void readRulesFromFile(TFile* f, std::vector<std::string> &vect)
{
   TList* list = f->GetStreamerInfoList();
   if (!list) {
      return;
   }

   list->SetOwner(kFALSE);

   TStreamerInfo *info;

   TObjLink *lnk = list->FirstLink();
   while (lnk) {
      info = (TStreamerInfo*)lnk->GetObject();
      if (info == 0) {
         lnk = lnk->Next();
         continue;
      }
      
      if (info->IsA() != TStreamerInfo::Class()) {
         TObject *obj = (TObject*)info;
         if (strcmp(obj->GetName(),"listOfRules")==0) {          
            TList *listOfRules = (TList*)obj;
            TObjLink *rulelnk = listOfRules->FirstLink();
            while (rulelnk) {
               TObjString *rule = (TObjString*)rulelnk->GetObject();
               vect.push_back(std::string(rule->String().Data()));
               rulelnk = rulelnk->Next();
            }
         }
         info->SetBit(kCanDelete);
      }
      lnk = lnk->Next();
   }

   list->Clear(); 
   delete list;
}
