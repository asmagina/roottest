//////////////////////////////////////////////////////////
//   This class has been generated by TFile::MakeProject
//     (Tue Jun 14 15:33:00 2011 by ROOT version 5.31/01)
//      from the StreamerInfo in file http://root.cern.ch/files/atlasFlushed.root
//////////////////////////////////////////////////////////


#ifndef EventInfo_p2_h
#define EventInfo_p2_h
class EventInfo_p2;

#include "EventID_p1.h"
#include "EventType_p1.h"
#include "TriggerInfo_p2.h"
#include "Riostream.h"
#include <vector>

class EventInfo_p2 {

public:
// Nested classes declaration.

public:
// Data Members.
   EventID_p1  m_event_ID;    //
   EventType_p1 m_event_type;    //
   TriggerInfo_p2 m_trigger_info;    //
   vector<unsigned int> m_event_flags;     //

   EventInfo_p2();
   EventInfo_p2(const EventInfo_p2 & );
   virtual ~EventInfo_p2();

};
#endif