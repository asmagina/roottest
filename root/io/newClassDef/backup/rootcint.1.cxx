// @(#)root/utils:$Id$
// Author: Fons Rademakers   13/07/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// rootcint                                                             //
//                                                                      //
// This program generates the Streamer(), TBuffer &operator>>() and     //
// ShowMembers() methods for ROOT classes, i.e. classes using the       //
// ClassDef and ClassImp macros.                                        //
// In addition rootcint can also generate the CINT dictionaries         //
// needed in order to get access to ones classes via the interpreter.   //
//                                                                      //
// Rootcint can be used like:                                           //
//                                                                      //
//   rootcint TAttAxis.h[+][-][!] ... [LinkDef.h] > AxisDict.cxx        //
//                                                                      //
// or                                                                   //
//                                                                      //
//  rootcint [-f] axisDict.cxx [-c] TAttAxis.h[+][-][!] ... [LinkDef.h] //
//                                                                      //
// The difference between the two is that in the first case only the    //
// Streamer() and ShowMembers() methods are generated while in the      //
// latter case a complete compileable file is generated (including      //
// the include statements). The first method also allows the            //
// output to be appended to an already existing file (using >>).        //
// The optional - behind the header file name tells rootcint to not     //
// generate the Streamer() method. A custom method must be provided     //
// by the user in that case. For the + and ! options see below.         //
// When using option -c also the interpreter method interface stubs     //
// will be written to the output file (AxisDict.cxx in the above case). //
// By default the output file will not be overwritten if it exists.     //
// Use the -f (force) option to overwite the output file. The output    //
// file must have one of the following extensions: .cxx, .C, .cpp,      //
// .cc, .cp.                                                            //
//                                                                      //
// Before specifying the first header file one can also add include     //
// file directories to be searched and preprocessor defines, like:      //
//   -I$MYPROJECT/include -DDebug=1                                     //
//                                                                      //
// The (optional) file LinkDef.h looks like:                            //
//                                                                      //
// #ifdef __CINT__                                                      //
//                                                                      //
// #pragma link off all globals;                                        //
// #pragma link off all classes;                                        //
// #pragma link off all functions;                                      //
//                                                                      //
// #pragma link C++ class TAxis;                                        //
// #pragma link C++ class TAttAxis-;                                    //
// #pragma link C++ class TArrayC-!;                                    //
// #pragma link C++ class AliEvent+;                                    //
//                                                                      //
// #pragma link C++ function StrDup;                                    //
// #pragma link C++ function operator+(const TString&,const TString&);  //
//                                                                      //
// #pragma link C++ global gROOT;                                       //
// #pragma link C++ global gEnv;                                        //
//                                                                      //
// #pragma link C++ enum EMessageTypes;                                 //
//                                                                      //
// #endif                                                               //
//                                                                      //
// This file tells rootcint for which classes the method interface      //
// stubs should be generated. A trailing - in the class name tells      //
// rootcint to not generate the Streamer() method. This is necessary    //
// for those classes that need a customized Streamer() method.          //
// A trailing ! in the class name tells rootcint to not generate the    //
// operator>>(TBuffer &b, MyClass *&obj) function. This is necessary to //
// be able to write pointers to objects of classes not inheriting from  //
// TObject. See for an example the source of the TArrayF class.         //
// A trailing + in the class name tells rootcint to generate an         //
// automatic Streamer(), i.e. a streamer that let ROOT do automatic     //
// schema evolution. The + option is mutually exclusive with            //
// the - option. For new classes the + option is the                    //
// preferred option. For legacy reasons it is not yet the default.      //
// When the linkdef file is not specified a default version exporting   //
// the classes with the names equal to the include files minus the .h   //
// is generated.                                                        //
//                                                                      //
// *** IMPORTANT ***                                                    //
// 1) LinkDef.h must be the last argument on the rootcint command line. //
// 2) Note that the LinkDef file name MUST contain the string:          //
//    LinkDef.h, Linkdef.h or linkdef.h, i.e. NA49_LinkDef.h is fine    //
//    just like, linkdef1.h. Linkdef.h is case sensitive.               //
//                                                                      //
// ----------- historical ---------                                     //
//                                                                      //
// Note that the file rootcint.C is constructed in such a way that it   //
// can also be interpreted by CINT. The above two statements become in  //
// that case:                                                           //
//                                                                      //
// cint -I$ROOTSYS/include +V TAttAxis.h TAxis.h LinkDef.h rootcint.C \ //
//                            TAttAxis.h TAxis.h > AxisGen.C            //
//                                                                      //
// or                                                                   //
//                                                                      //
// cint -I$ROOTSYS/include +V TAttAxis.h TAxis.h LinkDef.h rootcint.C \ //
//                            AxisGen.C TAttAxis.h TAxis.h              //
//                                                                      //
// The +V and -I$ROOTSYS/include options are added to the list of       //
// arguments in the compiled version of rootcint.                       //
//////////////////////////////////////////////////////////////////////////

#ifndef __CINT__

#ifdef HAVE_CONFIG
#include "config.h"
#endif

#include "Api.h"

extern "C" {
   void  G__setothermain(int othermain);
   void  G__setglobalcomp(int globalcomp);
   int   G__main(int argc, char **argv);
   void  G__exit(int rtn);
   struct G__includepath *G__getipathentry();
}

const char *help =
"\n"
"This program generates the Streamer(), TBuffer &operator>>() and\n"
"ShowMembers() methods for ROOT classes, i.e. classes using the\n"
"ClassDef and ClassImp macros.\n"
"In addition rootcint can also generate the CINT dictionaries\n"
"needed in order to get access to ones classes via the interpreter.\n"
"\n"
"Rootcint can be used like:\n"
"\n"
"  rootcint TAttAxis.h[{+,-}][!] ... [LinkDef.h] > AxisDict.cxx\n"
"\n"
"or\n"
"\n"
"  rootcint [-f] AxisDict.cxx [-c] TAttAxis.h[{+,-}][!] ... [LinkDef.h]\n"
"\n"
"The difference between the two is that in the first case only the\n"
"Streamer() and ShowMembers() methods are generated while in the\n"
"latter case a complete compileable file is generated (including\n"
"the include statements). The first method also allows the\n"
"output to be appended to an already existing file (using >>).\n"
"The optional - behind the header file name tells rootcint\n"
"to not generate the Streamer() method. A custom method must be\n"
"provided by the user in that case. For the + and ! options see below.\n"
"When using option -c also the interpreter method interface stubs\n"
"will be written to the output file (AxisDict.cxx in the above case).\n"
"By default the output file will not be overwritten if it exists.\n"
"Use the -f (force) option to overwite the output file. The output\n"
"file must have one of the following extensions: .cxx, .C, .cpp,\n"
".cc, .cp.\n"
"\n"
"Before specifying the first header file one can also add include\n"
"file directories to be searched and preprocessor defines, like:\n"
"   -I$../include -DDebug\n"
"\n"
"The (optional) file LinkDef.h looks like:\n"
"\n"
"#ifdef __CINT__\n"
"\n"
"#pragma link off all globals;\n"
"#pragma link off all classes;\n"
"#pragma link off all functions;\n"
"\n"
"#pragma link C++ class TAxis;\n"
"#pragma link C++ class TAttAxis-;\n"
"#pragma link C++ class TArrayC-!;\n"
"#pragma link C++ class AliEvent+;\n"
"\n"
"#pragma link C++ function StrDup;\n"
"#pragma link C++ function operator+(const TString&,const TString&);\n"
"\n"
"#pragma link C++ global gROOT;\n"
"#pragma link C++ global gEnv;\n"
"\n"
"#pragma link C++ enum EMessageTypes;\n"
"\n"
"#endif\n"
"\n"
"This file tells rootcint for which classes the method interface\n"
"stubs should be generated. A trailing - in the class name tells\n"
"rootcint to not generate the Streamer() method. This is necessary\n"
"for those classes that need a customized Streamer() method.\n"
"A trailing ! in the class name tells rootcint to not generate the\n"
"operator>>(TBuffer &b, MyClass *&obj) method. This is necessary to\n"
"be able to write pointers to objects of classes not inheriting from\n"
"TObject. See for an example the source of the TArrayF class.\n"
"A trailing + in the class name tells rootcint to generate an\n"
"automatic Streamer(), i.e. a streamer that let ROOT do automatic\n"
"schema evolution. The + option is mutually exclusive with\n"
"the - option. For new classes the + option is the\n"
"preferred option. For legacy reasons it is not yet the default.\n"
"When this linkdef file is not specified a default version exporting\n"
"the classes with the names equal to the include files minus the .h\n"
"is generated.\n"
"\n"
"*** IMPORTANT ***\n"
"1) LinkDef.h must be the last argument on the rootcint command line.\n"
"2) Note that the LinkDef file name MUST contain the string:\n"
"   LinkDef.h, Linkdef.h or linkdef.h, i.e. NA49_LinkDef.h is fine,\n"
"   just like linkdef1.h. Linkdef.h is case sensitive.\n";

#else
#include <ertti.h>
#endif

#ifdef _WIN32
#ifdef system
#undef system
#endif
#include <process.h>
#endif

#ifdef __MWERKS__
#include <console.h>
#endif

#include <time.h>
#include <string>
using std::string;

const char *autoldtmpl = "G__auto%dLinkDef.h";
char autold[64];

enum ESTLType {kNone, kVector, kList, kDeque, kMap, kMultimap, kSet, kMultiset};

FILE *fp;


//______________________________________________________________________________
int GetClassVersion(G__ClassInfo &cl)
{
   // Return the version number of the class or -1
   // if the function Class_Version does not exist.

   if (!cl.HasMethod("Class_Version")) return -1;

   const char *function = "::Class_Version()";
   const char *classname = cl.Fullname();
   char *name = new char[strlen(classname)+strlen(function)+1];
   sprintf(name, "%s%s", classname, function);
   int version = (int)G__int(G__calc(name));
   delete [] name;
   return version;
}

//______________________________________________________________________________
int IsSTLContainer(G__DataMemberInfo &m)
{
   // Is this an STL container?

   const char *s = m.Type()->TmpltName();
   if (!s) return kNone;
   char type[512];
   strcpy(type, s);

   if (!strcmp(type, "vector"))   return kVector;
   if (!strcmp(type, "list"))     return kList;
   if (!strcmp(type, "deque"))    return kDeque;
   if (!strcmp(type, "map"))      return kMap;
   if (!strcmp(type, "multimap")) return kMultimap;
   if (!strcmp(type, "set"))      return kSet;
   if (!strcmp(type, "multiset")) return kMultiset;
   return kNone;
}

//______________________________________________________________________________
int IsSTLContainer(G__BaseClassInfo &m)
{
   // Is this an STL container?

   const char *s = m.Name();
   if (!s) return kNone;
   char type[512];
   strcpy(type, s);

   if (!strncmp(type, "vector",6))   return kVector;
   if (!strncmp(type, "list",4))     return kList;
   if (!strncmp(type, "deque",5))    return kDeque;
   if (!strncmp(type, "map",3))      return kMap;
   if (!strncmp(type, "multimap",8)) return kMultimap;
   if (!strncmp(type, "set",3))      return kSet;
   if (!strncmp(type, "multiset",8)) return kMultiset;
   return kNone;
}

//______________________________________________________________________________
int IsStreamable(G__DataMemberInfo &m)
{
   // Is this member a Streamable object?

   if ((m.Property() & G__BIT_ISSTATIC) ||
         strncmp(m.Title(), "!", 1) == 0        ||
         strcmp(m.Name(), "G__virtualinfo") == 0) return 0;
   if (((m.Type())->Property() & G__BIT_ISFUNDAMENTAL) ||
       ((m.Type())->Property() & G__BIT_ISENUM)) return 0;
   if (IsSTLContainer(m)) return 1;
   if (!strcmp(m.Type()->Name(), "string") || !strcmp(m.Type()->Name(), "string*")) return 1;
   if ((m.Type())->HasMethod("Streamer")) {
      if (!(m.Type())->HasMethod("Class_Version")) return 1;
      int version = GetClassVersion(*m.Type());
      if (version > 0) return 1;
   }
   return 0;
}

//______________________________________________________________________________
G__TypeInfo &TemplateArg(G__DataMemberInfo &m, int count = 0)
{
   // Returns template argument. When count = 0 return first argument,
   // 1 second, etc.

   static G__TypeInfo ti;
   char arg[512], *s;

   strcpy(arg, m.Type()->TmpltArg());
   s = strtok(arg, ",");
   for (int i = 0; i < count; i++)
      s = strtok(0, ",");

   ti.Init(s);

   return ti;
}

//______________________________________________________________________________
G__TypeInfo &TemplateArg(G__BaseClassInfo &m, int count = 0)
{
   // Returns template argument. When count = 0 return first argument,
   // 1 second, etc.

   static G__TypeInfo ti;
   char arg[512], *s;

   strcpy(arg, m.Name());
   s = strtok(arg, "<");
   s = strtok(0, ",");
   for (int i = 0; i < count; i++)
      s = strtok(0, ",");

   ti.Init(s);

   return ti;
}

//______________________________________________________________________________
void WriteStringOperators(FILE *fd)
{
   // Write static ANSI C++ string to TBuffer operators.

   fprintf(fd, "//_______________________________________");
   fprintf(fd, "_______________________________________\n");
   fprintf(fd, "static TBuffer &operator>>(TBuffer &b, string &s)\n{\n");
   fprintf(fd, "   // Reading string object.\n\n");
   fprintf(fd, "   Assert(b.IsReading());\n");
   fprintf(fd, "   char ch;\n");
   fprintf(fd, "   do {\n");
   fprintf(fd, "      b >> ch;\n");
   fprintf(fd, "      if (ch) s.append(1, ch);\n");
   fprintf(fd, "   } while (ch != 0);\n");
   fprintf(fd, "   return b;\n");
   fprintf(fd, "}\n");
   fprintf(fd, "//_______________________________________");
   fprintf(fd, "_______________________________________\n");
   fprintf(fd, "static TBuffer &operator<<(TBuffer &b, string s)\n{\n");
   fprintf(fd, "   // Writing string object.\n\n");
   fprintf(fd, "   Assert(b.IsWriting());\n");
   fprintf(fd, "   b.WriteString(s.c_str());\n");
   fprintf(fd, "   return b;\n");
   fprintf(fd, "}\n");
}

//______________________________________________________________________________
int STLStringStreamer(G__DataMemberInfo &m, int rwmode)
{
   // Create Streamer code for a standard string object. Returns 1 if data
   // member was a standard string and if Streamer code has been created,
   // 0 otherwise.

   if (!strcmp(m.Type()->Name(), "string") ||
       !strcmp(m.Type()->Name(), "string*")) {
      if (rwmode == 0) {
         // create read mode
         if ((m.Property() & G__BIT_ISPOINTER) &&
             (m.Property() & G__BIT_ISARRAY)) {

         } else if (m.Property() & G__BIT_ISARRAY) {

         } else {
            fprintf(fp, "      { TString R__str; R__str.Streamer(R__b); ");
            if (m.Property() & G__BIT_ISPOINTER)
               fprintf(fp, "if (*%s) delete *%s; (*%s = new string(R__str.Data())); }\n", m.Name(), m.Name(), m.Name());
            else
               fprintf(fp, "%s = R__str.Data(); }\n", m.Name());
         }
      } else {
         // create write mode
         if (m.Property() & G__BIT_ISPOINTER)
            fprintf(fp, "      { TString R__str; if (*%s) R__str = (*%s)->c_str(); R__str.Streamer(R__b);}\n", m.Name(), m.Name());
         else
            fprintf(fp, "      { TString R__str = %s.c_str(); R__str.Streamer(R__b);}\n", m.Name());
      }
      return 1;
   }
   return 0;
}

//______________________________________________________________________________
int STLContainerArrayStreamer(G__DataMemberInfo &m, int rwmode)
{
   // Create Streamer code for an STL container array. Returns 1 if data member
   // was an STL container and if Streamer code has been created, 0 otherwise.

   const char *stlc = m.Type()->TmpltName();
   int len = 1;
   for (int dim = 0; dim < m.ArrayDim(); dim++) len *= m.MaxIndex(dim);

   if (!strcmp(stlc, "vector") || !strcmp(stlc, "list") ||
       !strcmp(stlc, "deque")) {
      if (rwmode == 0) {
         // create read code
         fprintf(fp, "      {\n");
         fprintf(fp, "         for (Int_t R__l = 0; R__l < %d; R__l++) {\n",len);
         const char *s = TemplateArg(m).Name();
         if (!strncmp(s, "const ", 6)) s += 6;
         fprintf(fp, "            int R__i, R__n;\n");
         fprintf(fp, "            R__b >> R__n;\n");
         if (m.Property() & G__BIT_ISPOINTER) {
            fprintf(fp, "            %s[R__l] = new %s<%s >;\n", m.Name(), stlc, s);
            if (!strcmp(stlc, "vector")) {
               fprintf(fp, "            %s[R__l]->reserve(R__n);\n", m.Name());
            }
         } else {
            fprintf(fp, "            %s[R__l].clear();\n", m.Name());
            if (!strcmp(stlc, "vector")) {
               fprintf(fp, "            %s[R__l].reserve(R__n);\n", m.Name());
            }
         }
         fprintf(fp, "            for (R__i = 0; R__i < R__n; R__i++) {\n");
         fprintf(fp, "               %s R__t;\n", s);
         if ((TemplateArg(m).Property() & G__BIT_ISPOINTER) ||
             (TemplateArg(m).Property() & G__BIT_ISFUNDAMENTAL) ||
             (TemplateArg(m).Property() & G__BIT_ISENUM)) {
            if (TemplateArg(m).Property() & G__BIT_ISENUM)
               fprintf(fp, "               R__b >> (Int_t&)R__t;\n");
            else
               fprintf(fp, "               R__b >> R__t;\n");
         } else {
            if (TemplateArg(m).HasMethod("Streamer")) {
               fprintf(fp, "               R__t.Streamer(R__b);\n");
            } else {
              if (strcmp(s,"string") == 0) {
                 fprintf(fp,"               TString R__str;\n");
                 fprintf(fp,"               R__str.Streamer(R__b);\n");
                 fprintf(fp,"               R__t = R__str.Data();\n");
              } else {
                 fprintf(stderr, "*** Datamember %s::%s: template arg %s has no Streamer()"
                          " method (need manual intervention)\n",
                          m.MemberOf()->Name(), m.Name(), TemplateArg(m).Name());
                 fprintf(fp, "               //R__t.Streamer(R__b);\n");
              }
            }
        }
         if (m.Property() & G__BIT_ISPOINTER)
            fprintf(fp, "               %s[R__l]->push_back(R__t);\n", m.Name());
         else
            fprintf(fp, "               %s[R__l].push_back(R__t);\n", m.Name());
         fprintf(fp, "            }\n");
         fprintf(fp, "         }\n");
         fprintf(fp, "      }\n");
      } else {
         // create write code
         fprintf(fp, "      {\n");
         fprintf(fp, "         for (Int_t R__l = 0; R__l < %d; R__l++) {\n",len);
         if (m.Property() & G__BIT_ISPOINTER)
            fprintf(fp, "            R__b << int(%s[R__l]->size());\n", m.Name());
         else
            fprintf(fp, "            R__b << int(%s[R__l].size());\n", m.Name());
         fprintf(fp, "            %s<%s >::iterator R__k;\n", stlc, TemplateArg(m).Name());
         if (m.Property() & G__BIT_ISPOINTER)
            fprintf(fp, "            for (R__k = %s[R__l]->begin(); R__k != %s[R__l]->end(); ++R__k)\n",
                    m.Name(), m.Name());
         else
            fprintf(fp, "            for (R__k = %s[R__l].begin(); R__k != %s[R__l].end(); ++R__k)\n",
                    m.Name(), m.Name());
         if ((TemplateArg(m).Property() & G__BIT_ISPOINTER) ||
             (TemplateArg(m).Property() & G__BIT_ISFUNDAMENTAL) ||
             (TemplateArg(m).Property() & G__BIT_ISENUM)) {
            if (TemplateArg(m).Property() & G__BIT_ISENUM)
               fprintf(fp, "               R__b << (Int_t)*R__k;\n");
            else
               fprintf(fp, "               R__b << *R__k;\n");
         } else {
            if (TemplateArg(m).HasMethod("Streamer"))
               fprintf(fp, "               (*R__k).Streamer(R__b);\n");
            else
               fprintf(fp, "               //(*R__k).Streamer(R__b);\n");
         }
         fprintf(fp, "         }\n");
         fprintf(fp, "      }\n");
      }
   }
   return 1;
}

//______________________________________________________________________________
int STLContainerStreamer(G__DataMemberInfo &m, int rwmode)
{
   // Create Streamer code for an STL container. Returns 1 if data member
   // was an STL container and if Streamer code has been created, 0 otherwise.

   int stltype = IsSTLContainer(m);
   if (m.Type()->IsTmplt() && stltype) {
      if (m.Property() & G__BIT_ISARRAY) return STLContainerArrayStreamer(m,rwmode);

      if (rwmode == 0) {
         // create read code
         fprintf(fp, "      {\n");
         char tmparg[512];
         strcpy(tmparg,m.Type()->Name());
         int lenarg = strlen(tmparg);
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         const char *s = TemplateArg(m).Name();
         if (!strncmp(s, "const ", 6)) s += 6;
         if (m.Property() & G__BIT_ISPOINTER) {
            fprintf(fp, "         *%s = new %s;\n", m.Name(), tmparg);
         } else {
            fprintf(fp, "         %s.clear();\n", m.Name());
         }
         fprintf(fp, "         int R__i, R__n;\n");
         fprintf(fp, "         R__b >> R__n;\n");
         fprintf(fp, "         for (R__i = 0; R__i < R__n; R__i++) {\n");
         fprintf(fp, "            %s R__t;\n", s);
         if ((TemplateArg(m).Property() & G__BIT_ISPOINTER) ||
             (TemplateArg(m).Property() & G__BIT_ISFUNDAMENTAL) ||
             (TemplateArg(m).Property() & G__BIT_ISENUM)) {
            if (TemplateArg(m).Property() & G__BIT_ISENUM)
               fprintf(fp, "            R__b >> (Int_t&)R__t;\n");
            else {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            R__b >> R__t;\n");
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                     fprintf(fp, "            R__b >> R__t2;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str;\n");
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                        fprintf(fp, "            string R__t2 = R__str.Data();\n");
                     } else {
                        fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__t2.Streamer(R__b);\n");
                     }
                 }
               } else if (stltype == kSet || stltype == kMultiset) {
                  fprintf(fp, "            R__b >> R__t;\n");
               } else {
                  if (strcmp(s,"string*") == 0) {
                     fprintf(fp, "            TString R__str;\n");
                     fprintf(fp, "            R__str.Streamer(R__b);\n");
                     fprintf(fp, "            R__t = new string(R__str.Data());\n");
                  } else {
                     fprintf(fp, "            R__b >> R__t;\n");
                  }
               }
             }
          } else {
            if (TemplateArg(m).HasMethod("Streamer")) {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            R__t.Streamer(R__b);\n");
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                     fprintf(fp, "            R__b >> R__t2;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str;\n");
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                        fprintf(fp, "            string R__t2 = R__str.Data();\n");
                     } else {
                        fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__t2.Streamer(R__b);\n");
                     }
                  }
               } else {
                  fprintf(fp, "            R__t.Streamer(R__b);\n");
               }
            } else {
              if (strcmp(s,"string") == 0) {
                 fprintf(fp,"            TString R__str;\n");
                 fprintf(fp,"            R__str.Streamer(R__b);\n");
                 fprintf(fp,"            R__t = R__str.Data();\n");
              } else {
                 fprintf(stderr, "*** Datamember %s::%s: template arg %s has no Streamer()"
                         " method (need manual intervention)\n",
                         m.MemberOf()->Name(), m.Name(), TemplateArg(m).Name());
                 fprintf(fp, "            //R__t.Streamer(R__b);\n");
              }
            }
         }
         if (m.Property() & G__BIT_ISPOINTER) {
            if (stltype == kMap || stltype == kMultimap) {
               fprintf(fp, "            (*%s)->insert(make_pair(R__t,R__t2));\n", m.Name());
            } else if (stltype == kSet || stltype == kMultiset) {
               fprintf(fp, "            (*%s)->insert(R__t);\n", m.Name());
            } else {
               fprintf(fp, "            (*%s)->push_back(R__t);\n", m.Name());
            }
         } else {
            if (stltype == kMap || stltype == kMultimap) {
               fprintf(fp, "            %s.insert(make_pair(R__t,R__t2));\n", m.Name());
            } else if (stltype == kSet || stltype == kMultiset) {
               fprintf(fp, "            %s.insert(R__t);\n", m.Name());
            } else {
               fprintf(fp, "            %s.push_back(R__t);\n", m.Name());
            }
         }
         fprintf(fp, "         }\n");
         fprintf(fp, "      }\n");
      } else {
         // create write code
         fprintf(fp, "      {\n");
         if (m.Property() & G__BIT_ISPOINTER)
            fprintf(fp, "         R__b << int((*%s)->size());\n", m.Name());
         else
            fprintf(fp, "         R__b << int(%s.size());\n", m.Name());
         char tmparg[512];
         strcpy(tmparg,m.Type()->Fullname());
         int lenarg = strlen(tmparg);
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         fprintf(fp, "         %s::iterator R__k;\n", tmparg);
         if (m.Property() & G__BIT_ISPOINTER)
            fprintf(fp, "         for (R__k = (*%s)->begin(); R__k != (*%s)->end(); ++R__k) {\n",
                    m.Name(), m.Name());
         else
            fprintf(fp, "         for (R__k = %s.begin(); R__k != %s.end(); ++R__k) {\n",
                    m.Name(), m.Name());
         if ((TemplateArg(m).Property() & G__BIT_ISPOINTER) ||
             (TemplateArg(m).Property() & G__BIT_ISFUNDAMENTAL) ||
             (TemplateArg(m).Property() & G__BIT_ISENUM)) {
            if (TemplateArg(m).Property() & G__BIT_ISENUM)
               fprintf(fp, "            R__b << (Int_t)*R__k;\n");
            else {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            R__b << (*R__k).first;\n");
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            R__b << (*R__k).second;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str = ((%s&)((*R__k).second)).c_str();\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                     } else {
                        fprintf(fp, "            ((%s&)((*R__k).second)).Streamer(R__b);\n",TemplateArg(m,1).Name());
                     }
                  }
               } else if (stltype == kSet || stltype == kMultiset) {
                  fprintf(fp, "            R__b << *R__k;\n");
               } else {
                  if (strcmp(TemplateArg(m).Name(),"string*") == 0) {
                     fprintf(fp,"            TString R__str = (*R__k)->c_str();\n");
                     fprintf(fp,"            R__str.Streamer(R__b);\n");
                  } else {
                     if (strcmp(TemplateArg(m).Name(),"(unknown)") == 0) {
                        fprintf(stderr, "Cannot process template argument1 %s\n",tmparg);
                        fprintf(fp, "            //R__b << *R__k;\n");
                     } else {
                        fprintf(fp, "            R__b << *R__k;\n");
                     }
                  }
               }
           }
         } else {
            if (TemplateArg(m).HasMethod("Streamer")) {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            ((%s&)((*R__k).first)).Streamer(R__b);\n",TemplateArg(m).Name());
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            R__b << (*R__k).second;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str = ((%s&)((*R__k).second)).c_str();\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                     } else {
                        fprintf(fp, "            ((%s&)((*R__k).second)).Streamer(R__b);\n",TemplateArg(m,1).Name());
                     }
                  }
               } else if (stltype == kSet || stltype == kMultiset) {
                  fprintf(fp, "            (*R__k).Streamer(R__b);\n");
               } else {
                  fprintf(fp, "            (*R__k).Streamer(R__b);\n");
               }
            } else {
               if (strcmp(TemplateArg(m).Name(),"string") == 0) {
                  fprintf(fp,"            TString R__str = (*R__k).c_str();\n");
                  fprintf(fp,"            R__str.Streamer(R__b);\n");
               } else {
                  if (strcmp(TemplateArg(m).Name(),"(unknown)") == 0) {
                    fprintf(stderr, "Cannot process template argument2 %s\n",tmparg);
                    fprintf(fp, "            //(*R__k).Streamer(R__b);\n");
                  } else {
                    fprintf(fp, "            //(*R__k).Streamer(R__b);\n");
                  }
               }
            }
        }
         fprintf(fp, "         }\n");
         fprintf(fp, "      }\n");
      }
      return 1;
   }
   return 0;
}

//______________________________________________________________________________
int STLBaseStreamer(G__BaseClassInfo &m, int rwmode)
{
   // Create Streamer code for an STL base class. Returns 1 if base class
   // was an STL container and if Streamer code has been created, 0 otherwise.

   int stltype = IsSTLContainer(m);
   if (m.IsTmplt() && stltype) {

      if (rwmode == 0) {
         // create read code
         fprintf(fp, "      {\n");
         char tmparg[512];
         strcpy(tmparg,m.Name());
         int lenarg = strlen(tmparg);
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         const char *s = TemplateArg(m).Name();
         if (!strncmp(s, "const ", 6)) s += 6;
         fprintf(fp, "         clear();\n");
         fprintf(fp, "         int R__i, R__n;\n");
         fprintf(fp, "         R__b >> R__n;\n");
         fprintf(fp, "         for (R__i = 0; R__i < R__n; R__i++) {\n");
         fprintf(fp, "            %s R__t;\n", s);
         if ((TemplateArg(m).Property() & G__BIT_ISPOINTER) ||
             (TemplateArg(m).Property() & G__BIT_ISFUNDAMENTAL) ||
             (TemplateArg(m).Property() & G__BIT_ISENUM)) {
            if (TemplateArg(m).Property() & G__BIT_ISENUM)
               fprintf(fp, "            R__b >> (Int_t&)R__t;\n");
            else {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            R__b >> R__t;\n");
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                     fprintf(fp, "            R__b >> R__t2;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str;\n");
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                        fprintf(fp, "            string R__t2 = R__str.Data();\n");
                     } else {
                        fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__t2.Streamer(R__b);\n");
                     }
                 }
               } else if (stltype == kSet || stltype == kMultiset) {
                  fprintf(fp, "            R__b >> R__t;\n");
               } else {
                  if (strcmp(s,"string*") == 0) {
                     fprintf(fp, "            TString R__str;\n");
                     fprintf(fp, "            R__str.Streamer(R__b);\n");
                     fprintf(fp, "            R__t = new string(R__str.Data());\n");
                  } else {
                     fprintf(fp, "            R__b >> R__t;\n");
                  }
               }
             }
          } else {
            if (TemplateArg(m).HasMethod("Streamer")) {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            R__t.Streamer(R__b);\n");
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                     fprintf(fp, "            R__b >> R__t2;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str;\n");
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                        fprintf(fp, "            string R__t2 = R__str.Data();\n");
                     } else {
                        fprintf(fp, "            %s R__t2;\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__t2.Streamer(R__b);\n");
                     }
                  }
               } else {
                  fprintf(fp, "            R__t.Streamer(R__b);\n");
               }
            } else {
              if (strcmp(s,"string") == 0) {
                 fprintf(fp,"            TString R__str;\n");
                 fprintf(fp,"            R__str.Streamer(R__b);\n");
                 fprintf(fp,"            R__t = R__str.Data();\n");
              } else {
                 fprintf(stderr, "*** Baseclass %s: template arg %s has no Streamer()"
                         " method (need manual intervention)\n",
                         m.Name(), TemplateArg(m).Name());
                 fprintf(fp, "            //R__t.Streamer(R__b);\n");
              }
            }
         }
         if (m.Property() & G__BIT_ISPOINTER) {
            if (stltype == kMap || stltype == kMultimap) {
               fprintf(fp, "            insert(make_pair(R__t,R__t2));\n");
            } else if (stltype == kSet || stltype == kMultiset) {
               fprintf(fp, "            insert(R__t);\n");
            } else {
               fprintf(fp, "            push_back(R__t);\n");
            }
         } else {
            if (stltype == kMap || stltype == kMultimap) {
               fprintf(fp, "            insert(make_pair(R__t,R__t2));\n");
            } else if (stltype == kSet || stltype == kMultiset) {
               fprintf(fp, "            insert(R__t);\n");
            } else {
               fprintf(fp, "            push_back(R__t);\n");
            }
         }
         fprintf(fp, "         }\n");
         fprintf(fp, "      }\n");
      } else {
         // create write code
         fprintf(fp, "      {\n");
         fprintf(fp, "         R__b << int(size());\n");
         char tmparg[512];
         strcpy(tmparg,m.Name());
         int lenarg = strlen(tmparg);
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         if (tmparg[lenarg-1] == '*') {tmparg[lenarg-1] = 0; lenarg--;}
         fprintf(fp, "         %s::iterator R__k;\n", tmparg);
         fprintf(fp, "         for (R__k = begin(); R__k != end(); ++R__k) {\n");
         if ((TemplateArg(m).Property() & G__BIT_ISPOINTER) ||
             (TemplateArg(m).Property() & G__BIT_ISFUNDAMENTAL) ||
             (TemplateArg(m).Property() & G__BIT_ISENUM)) {
            if (TemplateArg(m).Property() & G__BIT_ISENUM)
               fprintf(fp, "            R__b << (Int_t)*R__k;\n");
            else {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            R__b << (*R__k).first;\n");
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            R__b << (*R__k).second;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str = ((%s&)((*R__k).second)).c_str();\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                     } else {
                        fprintf(fp, "            ((%s&)((*R__k).second)).Streamer(R__b);\n",TemplateArg(m,1).Name());
                     }
                  }
               } else if (stltype == kSet || stltype == kMultiset) {
                  fprintf(fp, "            R__b << *R__k;\n");
               } else {
                  if (strcmp(TemplateArg(m).Name(),"string*") == 0) {
                     fprintf(fp,"            TString R__str = (*R__k)->c_str();\n");
                     fprintf(fp,"            R__str.Streamer(R__b);\n");
                  } else {
                     if (strcmp(TemplateArg(m).Name(),"(unknown)") == 0) {
                        fprintf(stderr, "Cannot process template argument1 %s\n",tmparg);
                        fprintf(fp, "            //R__b << *R__k;\n");
                     } else {
                        fprintf(fp, "            R__b << *R__k;\n");
                     }
                  }
               }
           }
         } else {
            if (TemplateArg(m).HasMethod("Streamer")) {
               if (stltype == kMap || stltype == kMultimap) {
                  fprintf(fp, "            ((%s&)((*R__k).first)).Streamer(R__b);\n",TemplateArg(m).Name());
                  if ((TemplateArg(m,1).Property() & G__BIT_ISPOINTER) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISFUNDAMENTAL) ||
                  (TemplateArg(m,1).Property() & G__BIT_ISENUM)) {
                     fprintf(fp, "            R__b << (*R__k).second;\n");
                  } else {
                     if (strcmp(TemplateArg(m,1).Name(),"string") == 0) {
                        fprintf(fp, "            TString R__str = ((%s&)((*R__k).second)).c_str();\n",TemplateArg(m,1).Name());
                        fprintf(fp, "            R__str.Streamer(R__b);\n");
                     } else {
                        fprintf(fp, "            ((%s&)((*R__k).second)).Streamer(R__b);\n",TemplateArg(m,1).Name());
                     }
                  }
               } else if (stltype == kSet || stltype == kMultiset) {
                  fprintf(fp, "            (*R__k).Streamer(R__b);\n");
               } else {
                  fprintf(fp, "            (*R__k).Streamer(R__b);\n");
               }
            } else {
               if (strcmp(TemplateArg(m).Name(),"string") == 0) {
                  fprintf(fp,"            TString R__str = (*R__k).c_str();\n");
                  fprintf(fp,"            R__str.Streamer(R__b);\n");
               } else {
                  if (strcmp(TemplateArg(m).Name(),"(unknown)") == 0) {
                    fprintf(stderr, "Cannot process template argument2 %s\n",tmparg);
                    fprintf(fp, "            //(*R__k).Streamer(R__b);\n");
                  } else {
                    fprintf(fp, "            //(*R__k).Streamer(R__b);\n");
                  }
               }
            }
        }
         fprintf(fp, "         }\n");
         fprintf(fp, "      }\n");
      }
      return 1;
   }
   return 0;
}

//______________________________________________________________________________
int PointerToPointer(G__DataMemberInfo &m)
{
   if (strstr(m.Type()->Name(), "**")) return 1;
   return 0;
}


//______________________________________________________________________________
void WriteArrayDimensions(int dim)
{
   for (int i = 0; i < dim-1; i++)
      fprintf(fp, "[0]");
}

//______________________________________________________________________________
void WriteInputOperator(G__ClassInfo &cl)
{
   fprintf(fp, "//_______________________________________");
   fprintf(fp, "_______________________________________\n");

   G__ClassInfo space = cl.EnclosingSpace();
   char space_prefix[256] = "";
   if (space.Property() & G__BIT_ISNAMESPACE)
      sprintf(space_prefix,"%s::",space.Fullname());

   if (cl.IsTmplt()) {
      // Produce specialisation for templates:
      fprintf(fp, "template <> TBuffer &operator>>"
              "(TBuffer &buf, %s *&obj)\n{\n", cl.Fullname());
   } else {
      fprintf(fp, "TBuffer &%soperator>>(TBuffer &buf, %s *&obj)\n{\n",
              space_prefix, cl.Fullname() );
   }
   fprintf(fp, "   // Read a pointer to an object of class %s.\n\n", cl.Fullname());

   if (cl.IsBase("TObject") || !strcmp(cl.Fullname(), "TObject")) {
      fprintf(fp, "   obj = (%s *) buf.ReadObject(%s::Class());\n", cl.Fullname(),
              cl.Fullname());
   } else {
      fprintf(fp, "   ::Error(\"%s::operator>>\", \"objects not inheriting"
                  " from TObject need a specialized operator>>"
                  " function\"); if (obj) { }\n", cl.Fullname());
   }
   fprintf(fp, "   return buf;\n}\n\n");
}


//______________________________________________________________________________
void WriteClassName(G__ClassInfo &cl, int tmplt = 0)
{
   // Write the code to set the class name and the initialization object.

   fprintf(fp, "//_______________________________________");
   fprintf(fp, "_______________________________________\n");
   fprintf(fp, "const char *%s::Class_Name()\n{\n", cl.Fullname());
   fprintf(fp, "   // Return the class name for %s.\n", cl.Fullname());
   fprintf(fp, "   return \"%s\";\n}\n\n", cl.Fullname());
   if (!tmplt) {
      fprintf(fp, "// Static variable to hold initialization object\n");
      fprintf(fp, "static %s::R__Init __gR__Init%s(%d);\n\n",
              cl.Fullname(), G__map_cpp_name((char *)cl.Fullname()),
              cl.RootFlag());
   } else {
      fprintf(fp, "//_______________________________________");
      fprintf(fp, "_______________________________________\n");
      fprintf(fp, "const char* %s::ImplFileName()\n{\n", cl.Fullname());
      fprintf(fp, "   return R__tInit< %s >::GetImplFileName();\n}\n\n",
                  cl.Fullname());

      fprintf(fp, "//_______________________________________");
      fprintf(fp, "_______________________________________\n");
      fprintf(fp, "int   %s::ImplFileLine()\n{\n", cl.Fullname());
      fprintf(fp, "   return R__tInit< %s >::GetImplFileLine();\n}\n\n",
                  cl.Fullname());

      fprintf(fp, "//_______________________________________");
      fprintf(fp, "_______________________________________\n");
      fprintf(fp, "void %s::Dictionary()\n{\n", cl.Fullname());
      fprintf(fp, "   fgIsA = CreateClass(Class_Name(),   Class_Version(),\n");
      fprintf(fp, "                       DeclFileName(), ImplFileName(),\n");
      fprintf(fp, "                       DeclFileLine(), ImplFileLine());\n");
      fprintf(fp, "};\n\n");

      fprintf(fp, "//_______________________________________");
      fprintf(fp, "_______________________________________\n");
      fprintf(fp, "TClass *%s::Class()\n{\n", cl.Fullname());
      fprintf(fp, "   if (!fgIsA) Dictionary(); return fgIsA;\n}\n\n");

      fprintf(fp, "// Static variable to hold initialization object\n");
      fprintf(fp, "static R__tInit< %s > __gR__Init%s(%d);\n\n", cl.Fullname(),
              G__map_cpp_name((char*)cl.Fullname()), cl.RootFlag());
      //      fprintf(fp, "template <> \n");
      fprintf(fp, "template <> TClass *%s::fgIsA = 0;\n", cl.Fullname());
      //      fprintf(fp, "template <> char* %s::fgImplFileName = 0;\n", cl.Fullname());
      //      fprintf(fp, "template <> int   %s::fgImplFileLine = 0;\n", cl.Fullname());
      fprintf(fp, "\n");

#if 0
      int numberOfArgs = 1;
      const char* args = cl.TmpltArg();
      for (unsigned int i=0,nest = 0; i<strlen(args); i++) {
         if (nest==0 && args[i]==',') numberOfArgs++;
         switch (args[i]) {
         case '(':
         case '[':
         case '<':
           nest++;
           break;
         case ')':
         case ']':
         case '>':
           nest--;
           break;
         };
      }
      fprintf(fp, "R__Setter%d< %s, %s > setterFor%s;\n",
              numberOfArgs, cl.TmpltArg(), cl.Fullname(),G__map_cpp_name((char*)cl.Fullname()));
#endif
   }
}

//______________________________________________________________________________
const char *ShortTypeName (const char *typeDesc)
{
   // Return the absolute type of typeDesc.
   // E.g.: typeDesc = "class TNamed**", returns "TNamed".
   // You need to use the result immediately before it is being overwritten.

   static char t[64];
   char *s;
   if (!strstr(typeDesc, "(*)(") && (s = (char*)strchr(typeDesc, ' ')))
      strcpy(t, s+1);
   else
      strcpy(t, typeDesc);

   int l = strlen(t);
   while (l > 0 && t[l-1] == '*')
      t[--l] = 0;

   return t;
}

//______________________________________________________________________________
const char *GrabIndex(G__DataMemberInfo &member, int printError)
{
   // GrabIndex returns a static string (so use it or copy it immediatly, do not
   // call GrabIndex twice in the same expression) containing the size of the
   // array data member.
   // In case of error, or if the size is not specified, GrabIndex returns 0.

   int error;
   char *where = 0;

   const char *index = member.ValidArrayIndex(&error, &where);
   if (index==0 && printError) {
      const char *errorstring;
      switch (error) {
         case G__DataMemberInfo::NOT_INT:
            errorstring = "is not an integer";
            break;
         case G__DataMemberInfo::NOT_DEF:
            errorstring = "has not been defined before the array";
            break;
         case G__DataMemberInfo::IS_PRIVATE:
            errorstring = "is a private member of a parent class";
            break;
         case G__DataMemberInfo::UNKNOWN:
            errorstring = "is not known";
            break;
         default:
            errorstring = "UNKNOWN ERROR!!!!";
      }

      if (where==0) {
         fprintf(stderr,"*** Datamember %s::%s: no size indication!\n",
                 member.MemberOf()->Name(), member.Name());
      } else {
         fprintf(stderr,"*** Datamember %s::%s: size of array (%s) %s!\n",
                 member.MemberOf()->Name(), member.Name(), where, errorstring);
      }
   }
   return index;
}


//______________________________________________________________________________
void WriteStreamer(G__ClassInfo &cl)
{

   fprintf(fp, "//_______________________________________");
   fprintf(fp, "_______________________________________\n");
   fprintf(fp, "void %s::Streamer(TBuffer &R__b)\n{\n", cl.Fullname());
   fprintf(fp, "   // Stream an object of class %s.\n\n", cl.Fullname());

   // In case of VersionID<=0 write dummy streamer only calling
   // its base class Streamer(s). If no base class(es) let Streamer
   // print error message, i.e. this Streamer should never have been called.
   int version = GetClassVersion(cl);
   if (version <= 0) {
      G__BaseClassInfo b(cl);

      int basestreamer = 0;
      while (b.Next())
         if (b.HasMethod("Streamer")) {
            fprintf(fp, "   %s::Streamer(R__b);\n", b.Name());
            basestreamer++;
         }
      if (!basestreamer) {
         fprintf(fp, "   ::Error(\"%s::Streamer\", \"version id <=0 in ClassDef,"
                 " dummy Streamer() called\"); if (R__b.IsReading()) { }\n", cl.Fullname());
      }
      fprintf(fp, "}\n\n");
      return;
   }

   // see if we should generate Streamer with extra byte count code
   int ubc = 0;
   //if ((cl.RootFlag() & G__USEBYTECOUNT)) ubc = 1;
   ubc = 1;   // now we'll always generate byte count streamers

   // loop twice: first time write reading code, second time writing code
   for (int i = 0; i < 2; i++) {

      int decli = 0;

      if (i == 0) {
         if (ubc) fprintf(fp, "   UInt_t R__s, R__c;\n");
         fprintf(fp, "   if (R__b.IsReading()) {\n");
         if (ubc)
            fprintf(fp, "      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }\n");
         else
            fprintf(fp, "      Version_t R__v = R__b.ReadVersion(); if (R__v) { }\n");
      } else {
         if (ubc) fprintf(fp, "      R__b.CheckByteCount(R__s, R__c, %s::IsA());\n", cl.Fullname());
         fprintf(fp, "   } else {\n");
         if (ubc)
            fprintf(fp, "      R__c = R__b.WriteVersion(%s::IsA(), kTRUE);\n",cl.Fullname());
         else
            fprintf(fp, "      R__b.WriteVersion(%s::IsA());\n",cl.Fullname());
      }

      // Stream base class(es) when they have the Streamer() method
      G__BaseClassInfo b(cl);

      while (b.Next()) {
         if (b.HasMethod("Streamer"))
            fprintf(fp, "      %s::Streamer(R__b);\n", b.Name());
      }
      // Stream data members
      G__DataMemberInfo m(cl);

      while (m.Next()) {

         // we skip:
         //  - static members
         //  - members with an ! as first character in the title (comment) field
         //  - the member G__virtualinfo inserted by the CINT RTTI system

         if (!(m.Property() & G__BIT_ISSTATIC) &&
             strncmp(m.Title(), "!", 1)        &&
             strcmp(m.Name(), "G__virtualinfo")) {

            // fundamental type: short, int, long, etc....
            if (((m.Type())->Property() & G__BIT_ISFUNDAMENTAL) ||
                ((m.Type())->Property() & G__BIT_ISENUM)) {
               if (m.Property() & G__BIT_ISARRAY &&
                   m.Property() & G__BIT_ISPOINTER) {
                  int s = 1;
                  for (int dim = 0; dim < m.ArrayDim(); dim++)
                     s *= m.MaxIndex(dim);
                  if (!decli) {
                     fprintf(fp, "      int R__i;\n");
                     decli = 1;
                  }
                  fprintf(fp, "      for (R__i = 0; R__i < %d; R__i++)\n", s);
                 if (i == 0) {
                     fprintf(stderr,"*** Datamember %s::%s: array of pointers to fundamental type (need manual intervention)\n", cl.Fullname(), m.Name());
                     fprintf(fp, "         ;//R__b.ReadArray(%s);\n", m.Name());
                  } else {
                     fprintf(fp, "         ;//R__b.WriteArray(%s, __COUNTER__);\n", m.Name());
                  }
               } else if (m.Property() & G__BIT_ISPOINTER) {
                  const char *indexvar = GrabIndex(m, i==0);
                  if (indexvar==0) {
                     if (i == 0) {
                        fprintf(stderr,"*** Datamember %s::%s: pointer to fundamental type (need manual intervention)\n", cl.Fullname(), m.Name());
                        fprintf(fp, "      //R__b.ReadArray(%s);\n", m.Name());
                     } else {
                        fprintf(fp, "      //R__b.WriteArray(%s, __COUNTER__);\n", m.Name());
                     }
                  } else {
                     if (i == 0) {
                        fprintf(fp, "      delete []%s; \n",m.Name());
                        fprintf(fp, "      %s = new %s[%s]; \n",
                                m.Name(),ShortTypeName(m.Type()->Name()),indexvar);
                        fprintf(fp, "      R__b.ReadFastArray(%s,%s); \n",
                                m.Name(),indexvar);
                     } else {
                        fprintf(fp, "      R__b.WriteFastArray(%s,%s); \n",
                                m.Name(),indexvar);
                     }
                  }
               } else if (m.Property() & G__BIT_ISARRAY) {
                  if (i == 0) {
                     if (m.ArrayDim() > 1) {
                        if ((m.Type())->Property() & G__BIT_ISENUM)
                           fprintf(fp, "      R__b.ReadStaticArray((Int_t*)%s);\n", m.Name());
                        else
                           fprintf(fp, "      R__b.ReadStaticArray((%s*)%s);\n", m.Type()->TrueName(), m.Name());
                     } else {
                        if ((m.Type())->Property() & G__BIT_ISENUM)
                           fprintf(fp, "      R__b.ReadStaticArray((Int_t*)%s);\n", m.Name());
                        else
                           fprintf(fp, "      R__b.ReadStaticArray(%s);\n", m.Name());
                      }
                  } else {
                     int s = 1;
                     for (int dim = 0; dim < m.ArrayDim(); dim++)
                        s *= m.MaxIndex(dim);
                     if (m.ArrayDim() > 1) {
                        if ((m.Type())->Property() & G__BIT_ISENUM)
                           fprintf(fp, "      R__b.WriteArray((Int_t*)%s, %d);\n", m.Name(), s);
                        else
                           fprintf(fp, "      R__b.WriteArray((%s*)%s, %d);\n", m.Type()->TrueName(), m.Name(), s);
                     } else {
                        if ((m.Type())->Property() & G__BIT_ISENUM)
                           fprintf(fp, "      R__b.WriteArray((Int_t*)%s, %d);\n", m.Name(), s);
                        else
                           fprintf(fp, "      R__b.WriteArray(%s, %d);\n", m.Name(), s);
                     }
                  }
               } else if ((m.Type())->Property() & G__BIT_ISENUM) {
                  if (i == 0)
                     fprintf(fp, "      R__b >> (Int_t&)%s;\n", m.Name());
                  else
                     fprintf(fp, "      R__b << (Int_t)%s;\n", m.Name());
               } else {
                  if (i == 0)
                     fprintf(fp, "      R__b >> %s;\n", m.Name());
                  else
                     fprintf(fp, "      R__b << %s;\n", m.Name());
               }
            } else {
               // we have an object...

               // check if object is a standard string
               if (STLStringStreamer(m, i))
                  continue;

               // check if object is an STL container
               if (STLContainerStreamer(m, i))
                  continue;

               // handle any other type of objects
               if (m.Property() & G__BIT_ISARRAY &&
                   m.Property() & G__BIT_ISPOINTER) {
                  int s = 1;
                  for (int dim = 0; dim < m.ArrayDim(); dim++)
                     s *= m.MaxIndex(dim);
                  if (!decli) {
                     fprintf(fp, "      int R__i;\n");
                     decli = 1;
                  }
                  fprintf(fp, "      for (R__i = 0; R__i < %d; R__i++)\n", s);
                  if (i == 0)
                     fprintf(fp, "         R__b >> %s", m.Name());
                  else {
                     if (m.Type()->IsBase("TObject") && m.Type()->IsBase("TArray"))
                        fprintf(fp, "         R__b << (TObject*)%s", m.Name());
                     else
                        fprintf(fp, "         R__b << %s", m.Name());
                  }
                  WriteArrayDimensions(m.ArrayDim());
                  fprintf(fp, "[R__i];\n");
               } else if (m.Property() & G__BIT_ISPOINTER) {
                  // This is always good. However, in case of a pointer
                  // to an object that is guarenteed to be there and not
                  // being referenced by other objects we could use
                  //     xx->Streamer(b);
                  // Optimize this with control statement in title.
                  if (PointerToPointer(m)) {
                     if (i == 0) {
                        fprintf(stderr,"*** Datamember %s::%s: pointer to pointer (need manual intervention)\n", cl.Fullname(), m.Name());
                        fprintf(fp, "      //R__b.ReadArray(%s);\n", m.Name());
                     } else {
                        fprintf(fp, "      //R__b.WriteArray(%s, __COUNTER__);\n", m.Name());
                     }
                  } else {
                     if (strstr(m.Type()->Name(), "TClonesArray")) {
                        fprintf(fp, "      %s->Streamer(R__b);\n", m.Name());
                     } else {
                        if (i == 0)
                           fprintf(fp, "      R__b >> %s;\n", m.Name());
                        else {
                           if (m.Type()->IsBase("TObject") && m.Type()->IsBase("TArray"))
                              fprintf(fp, "      R__b << (TObject*)%s;\n", m.Name());
                           else
                              fprintf(fp, "      R__b << %s;\n", m.Name());
                        }
                     }
                  }
               } else if (m.Property() & G__BIT_ISARRAY) {
                  int s = 1;
                  for (int dim = 0; dim < m.ArrayDim(); dim++)
                     s *= m.MaxIndex(dim);
                  if (!decli) {
                     fprintf(fp, "      int R__i;\n");
                     decli = 1;
                  }
                  fprintf(fp, "      for (R__i = 0; R__i < %d; R__i++)\n", s);
                  fprintf(fp, "         %s", m.Name());
                  WriteArrayDimensions(m.ArrayDim());
                  fprintf(fp, "[R__i].Streamer(R__b);\n");
               } else {
                  if ((m.Type())->HasMethod("Streamer"))
                     fprintf(fp, "      %s.Streamer(R__b);\n", m.Name());
                  else {
                     if (i == 0)
                        fprintf(stderr, "*** Datamember %s::%s: object has no Streamer() method (need manual intervention)\n",
                                cl.Fullname(), m.Name());
                     fprintf(fp, "      //%s.Streamer(R__b);\n", m.Name());
                  }
               }
            }
         }
      }
   }
   if (ubc) fprintf(fp, "      R__b.SetByteCount(R__c, kTRUE);\n");
   fprintf(fp, "   }\n");
   fprintf(fp, "}\n\n");
}

//______________________________________________________________________________
void WriteAutoStreamer(G__ClassInfo &cl)
{
   // Write Streamer() method suitable for automatic schema evolution.

   fprintf(fp, "//_______________________________________");
   fprintf(fp, "_______________________________________\n");
   fprintf(fp, "void %s::Streamer(TBuffer &R__b)\n{\n", cl.Fullname());
   fprintf(fp, "   // Stream an object of class %s.\n\n", cl.Fullname());
   fprintf(fp, "   if (R__b.IsReading()) {\n");
   fprintf(fp, "      %s::Class()->ReadBuffer(R__b, this);\n", cl.Fullname());
   G__BaseClassInfo br(cl);
   while (br.Next())
      if (IsSTLContainer(br)) {
         STLBaseStreamer(br,0);
      }
   fprintf(fp, "   } else {\n");
   fprintf(fp, "      %s::Class()->WriteBuffer(R__b, this);\n", cl.Fullname());
   G__BaseClassInfo bw(cl);
   while (bw.Next())
      if (IsSTLContainer(bw)) {
         STLBaseStreamer(br,1);
      }
   fprintf(fp, "   }\n");
   fprintf(fp, "}\n\n");
}

//______________________________________________________________________________
void WriteStreamerBases(G__ClassInfo &cl)
{
   // Write Streamer() method for base classes of cl (unused)

   fprintf(fp, "//_______________________________________");
   fprintf(fp, "_______________________________________\n");
   fprintf(fp, "void %s_StreamerBases(TBuffer &R__b, void *pointer)\n{\n",  cl.Fullname());
   fprintf(fp, "   // Stream base classes of class %s.\n\n", cl.Fullname());
   fprintf(fp, "   %s *obj = (%s*)pointer;\n", cl.Fullname(), cl.Fullname());
   fprintf(fp, "   if (R__b.IsReading()) {\n");
   G__BaseClassInfo br(cl);
   while (br.Next())
      if (br.HasMethod("Streamer")) {
         fprintf(fp, "      obj->%s::Streamer(R__b);\n", br.Name());
      }
   fprintf(fp, "   } else {\n");
   G__BaseClassInfo bw(cl);
   while (bw.Next())
      if (bw.HasMethod("Streamer")) {
         fprintf(fp, "      obj->%s::Streamer(R__b);\n", bw.Name());
      }
   fprintf(fp, "   }\n");
   fprintf(fp, "}\n\n");
}

//______________________________________________________________________________
void WritePointersSTL(G__ClassInfo &cl)
{
   // Write interface function for STL members

   char a[80];
   char clName[G__LONGLINE];
   sprintf(clName,"%s",G__map_cpp_name((char *)cl.Fullname()));
   int version = GetClassVersion( cl);
   if (version <= 0) return;

   G__DataMemberInfo m(cl);

   while (m.Next()) {
      if ((m.Property() & G__BIT_ISSTATIC)) continue;
      int pCounter = 0;
      if (m.Property() & G__BIT_ISPOINTER) {
         const char *leftb = strchr(m.Title(),'[');
         if (leftb) {
            const char *rightb = strchr(leftb,']');
            if (rightb) {
               pCounter++;
               sprintf(a,m.Type()->Name());
               char *astar = (char*)strchr(a,'*');
               *astar = 0;
               if (strstr(m.Type()->Name(),"**")) pCounter++;
            }
         }
      }

      //member is a string
      if (!strcmp(m.Type()->Name(), "string") || !strcmp(m.Type()->Name(), "string*")) {
         fprintf(fp, "//_______________________________________");
         fprintf(fp, "_______________________________________\n");
         fprintf(fp, "void R__%s_%s(TBuffer &R__b, void *R__p, int)\n",clName,m.Name());
         fprintf(fp, "{\n");
         if (m.Property() & G__BIT_ISPOINTER) {
            //fprintf(fp, "   %s %s = (%s)R__p;\n",m.Type()->Name(),m.Name(),m.Type()->Name());
            fprintf(fp, "   %s* %s = (%s*)R__p;\n",m.Type()->Name(),m.Name(),m.Type()->Name());
         } else {
            fprintf(fp, "   %s &%s = *(%s *)R__p;\n",m.Type()->Name(),m.Name(),m.Type()->Name());
         }
         fprintf(fp, "   if (R__b.IsReading()) {\n");
         STLStringStreamer(m,0);
         fprintf(fp, "   } else {\n");
         STLStringStreamer(m,1);
         fprintf(fp, "   }\n");
         fprintf(fp, "}\n\n");
         continue;
      }

      if (!IsStreamable(m)) continue;
         fprintf(fp, "//_______________________________________");
         fprintf(fp, "_______________________________________\n");
         if (pCounter) {
            fprintf(fp, "void R__%s_%s(TBuffer &R__b, void *R__p, int R__n)\n",clName,m.Name());
         } else {
            fprintf(fp, "void R__%s_%s(TBuffer &R__b, void *R__p, int)\n",clName,m.Name());
         }
         fprintf(fp, "{\n");
         if (m.Property() & G__BIT_ISARRAY) {
            fprintf(fp, "   %s* %s = (%s*)R__p;\n",m.Type()->Fullname(),m.Name(),m.Type()->Fullname());
         } else {
            if (m.Property() & G__BIT_ISPOINTER) {
               if (pCounter) {
                  fprintf(fp, "   %s* %s = (%s*)R__p;\n",m.Type()->Fullname(),m.Name(),m.Type()->Fullname());
               } else {
                  fprintf(fp, "   %s* %s = (%s*)R__p;\n",m.Type()->Fullname(),m.Name(),m.Type()->Fullname());
               }
            } else {
               fprintf(fp, "   %s &%s = *(%s *)R__p;\n",m.Type()->Fullname(),m.Name(),m.Type()->Fullname());
            }
         }
         fprintf(fp, "   if (R__b.IsReading()) {\n");
         if (m.Type()->IsTmplt() && IsSTLContainer(m)) {
            STLContainerStreamer(m,0);
         } else {
            if (m.Property() & G__BIT_ISARRAY) {
               int len = 1;
               for (int dim = 0; dim < m.ArrayDim(); dim++) len *= m.MaxIndex(dim);
               fprintf(fp, "      for (Int_t R__l = 0; R__l < %d; R__l++) {\n",len);
               if (m.Property() & G__BIT_ISPOINTER) {
                  fprintf(fp, "         R__b >> %s[R__l];\n",m.Name());
               } else {
                  fprintf(fp, "         %s[R__l].Streamer(R__b);\n",m.Name());
               }
               fprintf(fp, "      }\n");
            } else {
               if (m.Property() & G__BIT_ISPOINTER) {
                  if (pCounter == 2) {
                     fprintf(fp, "      delete [] *%s;\n",m.Name());
                     fprintf(fp, "      if (!R__n) return;\n");
                     fprintf(fp, "      *%s = new %s*[R__n];\n",m.Name(),a);
                     fprintf(fp, "      %s** R__s = *%s;\n",a,m.Name());
                     fprintf(fp, "      for (Int_t R__l = 0; R__l < R__n; R__l++) {\n");
                     fprintf(fp, "         R__s[R__l] = new %s();\n",a);
                     fprintf(fp, "         R__s[R__l]->Streamer(R__b);\n");
                     fprintf(fp, "      }\n");
                  } else if(pCounter == 1) {
                     fprintf(fp, "      delete [] *%s;\n",m.Name());
                     fprintf(fp, "      if (!R__n) return;\n");
                     fprintf(fp, "      *%s = new %s[R__n];\n",m.Name(),a);
                     fprintf(fp, "      %s* R__s = *%s;\n",a,m.Name());
                     fprintf(fp, "      for (Int_t R__l = 0; R__l < R__n; R__l++) {\n");
                     fprintf(fp, "         R__s[R__l].Streamer(R__b);\n");
                     fprintf(fp, "      }\n");
                  } else {
                     if (strncmp(m.Title(),"->",2) == 0)
                        fprintf(fp, "      (*%s)->Streamer(R__b);\n",m.Name());
                     else
                        fprintf(fp, "      R__b >> *%s;\n",m.Name());
                 }
               } else {
                  fprintf(fp, "      %s.Streamer(R__b);\n",m.Name());
               }
            }
         }
         fprintf(fp, "   } else {\n");
         if (m.Type()->IsTmplt() && IsSTLContainer(m)) {
            STLContainerStreamer(m,1);
         } else {
            if (m.Property() & G__BIT_ISARRAY) {
               int len = 1;
               for (int dim = 0; dim < m.ArrayDim(); dim++) len *= m.MaxIndex(dim);
               fprintf(fp, "      for (Int_t R__l = 0; R__l < %d; R__l++) {\n",len);
               if (m.Property() & G__BIT_ISPOINTER) {
                  if (m.Type()->IsBase("TObject"))
                     fprintf(fp, "         R__b << (TObject*)%s[R__l];\n",m.Name());
                  else
                     fprintf(fp, "         R__b << %s[R__l];\n",m.Name());
               } else {
                  fprintf(fp, "         %s[R__l].Streamer(R__b);\n",m.Name());
               }
               fprintf(fp, "      }\n");
            } else {
               if (m.Property() & G__BIT_ISPOINTER) {
                  if (pCounter == 2) {
                     fprintf(fp, "      %s** R__s = *%s;\n",a,m.Name());
                     fprintf(fp, "      for (Int_t R__l = 0; R__l < R__n; R__l++) {\n");
                     fprintf(fp, "         R__s[R__l]->Streamer(R__b);\n");
                     fprintf(fp, "      }\n");
                  } else if(pCounter == 1) {
                     fprintf(fp, "      %s* R__s = *%s;\n",a,m.Name());
                     fprintf(fp, "      for (Int_t R__l = 0; R__l < R__n; R__l++) {\n");
                     fprintf(fp, "         R__s[R__l].Streamer(R__b);\n");
                     fprintf(fp, "      }\n");
                  } else {
                     if (strncmp(m.Title(),"->",2) == 0)
                        fprintf(fp, "      (*%s)->Streamer(R__b);\n",m.Name());
                     else {
                        if (m.Type()->IsBase("TObject"))
                           fprintf(fp, "      R__b << (TObject*)*%s;\n",m.Name());
                        else
                           fprintf(fp, "      R__b << *%s;\n",m.Name());
                     }
                  }
               } else {
                  fprintf(fp, "      %s.Streamer(R__b);\n",m.Name());
               }
            }
         }
         fprintf(fp, "   }\n");
         fprintf(fp, "}\n\n");
      //}
   }
}

//______________________________________________________________________________
void WriteShowMembers(G__ClassInfo &cl)
{
   fprintf(fp, "//_______________________________________");
   fprintf(fp, "_______________________________________\n");
   fprintf(fp, "void %s::ShowMembers(TMemberInspector &R__insp, char *R__parent)\n{\n", cl.Fullname());
   fprintf(fp, "   // Inspect the data members of an object of class %s.\n\n", cl.Fullname());
#ifdef  WIN32
   // This is to work around a bad msvc C++ bug.
   // This code would work in the general case, but why bother....and
   // we want to remember to eventually remove it ...

   if (strstr(cl.Fullname(),"::")) {
       // there is a namespace involved, trigger MS VC bug workaround
       fprintf(fp, "   typedef %s msvc_bug_workaround;\n", cl.Fullname());
       fprintf(fp, "   TClass *R__cl  = msvc_bug_workaround::IsA();\n");
    } else
       fprintf(fp, "   TClass *R__cl  = %s::IsA();\n", cl.Fullname());
#else   
   fprintf(fp, "   TClass *R__cl  = %s::IsA();\n", cl.Fullname());
#endif
   fprintf(fp, "   Int_t   R__ncp = strlen(R__parent);\n");
   fprintf(fp, "   if (R__ncp || R__cl || R__insp.IsA()) { }\n");

   // Inspect data members
   G__DataMemberInfo m(cl);
   char cdim[12], cvar[64];
   char clName[G__LONGLINE];
   sprintf(clName,"%s",G__map_cpp_name((char *)cl.Fullname()));
   int version = GetClassVersion(cl);
   int clflag = 1;
   if (version <= 0 || cl.RootFlag() == 0) clflag = 0;

   while (m.Next()) {

      // we skip:
      //  - static members
      //  - the member G__virtualinfo inserted by the CINT RTTI system

      if (!(m.Property() & G__BIT_ISSTATIC) &&
          strcmp(m.Name(), "G__virtualinfo")) {

         // fundamental type: short, int, long, etc....
         if (((m.Type())->Property() & G__BIT_ISFUNDAMENTAL) ||
             ((m.Type())->Property() & G__BIT_ISENUM)) {
            if (m.Property() & G__BIT_ISARRAY &&
                m.Property() & G__BIT_ISPOINTER) {
               sprintf(cvar, "*%s", m.Name());
               for (int dim = 0; dim < m.ArrayDim(); dim++) {
                  sprintf(cdim, "[%d]", m.MaxIndex(dim));
                  strcat(cvar, cdim);
               }
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", &%s);\n",
                       cvar, m.Name());
            } else if (m.Property() & G__BIT_ISPOINTER) {
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"*%s\", &%s);\n",
                       m.Name(), m.Name());
            } else if (m.Property() & G__BIT_ISARRAY) {
               sprintf(cvar, "%s", m.Name());
               for (int dim = 0; dim < m.ArrayDim(); dim++) {
                  sprintf(cdim, "[%d]", m.MaxIndex(dim));
                  strcat(cvar, cdim);
               }
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", %s);\n",
                       cvar, m.Name());
            } else {
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", &%s);\n",
                       m.Name(), m.Name());
            }
         } else {
            // we have an object

            //string
            if (!strcmp(m.Type()->Name(), "string") || !strcmp(m.Type()->Name(), "string*")) {
               if (m.Property() & G__BIT_ISPOINTER) {
                  fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"*%s\", &%s);\n",
                       m.Name(), m.Name());
                  if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"*%s\",R__%s_%s);\n", m.Name(), clName, m.Name());
               } else {
                  fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", &%s);\n",
                          m.Name(), m.Name());
                  if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"%s\",R__%s_%s);\n", m.Name(), clName, m.Name());
               }
               continue;
            }

            if (m.Property() & G__BIT_ISARRAY &&
                m.Property() & G__BIT_ISPOINTER) {
               sprintf(cvar, "*%s", m.Name());
               for (int dim = 0; dim < m.ArrayDim(); dim++) {
                  sprintf(cdim, "[%d]", m.MaxIndex(dim));
                  strcat(cvar, cdim);
               }
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", &%s);\n", cvar,
                       m.Name());
               if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"%s\",R__%s_%s);\n", cvar, clName, m.Name());
            } else if (m.Property() & G__BIT_ISPOINTER) {
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"*%s\", &%s);\n",
                       m.Name(), m.Name());
               if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"*%s\",R__%s_%s);\n", m.Name(), clName, m.Name());
            } else if (m.Property() & G__BIT_ISARRAY) {
               sprintf(cvar, "%s", m.Name());
               for (int dim = 0; dim < m.ArrayDim(); dim++) {
                  sprintf(cdim, "[%d]", m.MaxIndex(dim));
                  strcat(cvar, cdim);
               }
               fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", %s);\n",
                       cvar, m.Name());
               if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"%s\",R__%s_%s);\n", cvar, clName, m.Name());
            } else {
               if ((m.Type())->HasMethod("ShowMembers")) {
                  fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", &%s);\n",
                          m.Name(), m.Name());
                  fprintf(fp, "   %s.ShowMembers(R__insp, strcat(R__parent,\"%s.\")); R__parent[R__ncp] = 0;\n",
                          m.Name(), m.Name());
                  if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"%s\",R__%s_%s);\n", m.Name(), clName, m.Name());
               } else {
                  fprintf(fp, "   R__insp.Inspect(R__cl, R__parent, \"%s\", (void*)&%s);\n",
                          m.Name(), m.Name());
                  if (clflag && IsStreamable(m)) fprintf(fp, "   R__cl->SetStreamer(\"%s\",R__%s_%s);\n", m.Name(), clName, m.Name());
               }
            }
         }
      }
   }

   // Write ShowMembers for base class(es) when they have the ShowMember() method
   G__BaseClassInfo b(cl);

   while (b.Next())
      if (b.HasMethod("ShowMembers"))
         fprintf(fp, "   %s::ShowMembers(R__insp, R__parent);\n", b.Name());

   fprintf(fp, "}\n\n");
}

//______________________________________________________________________________
void WriteClassCode(G__ClassInfo &cl) {

   if ((cl.Property() & G__BIT_ISCLASS) && cl.Linkage() == G__CPPLINK) {

      // Write Code for Class_Name() and static variable
      // to hold initialization object (STK)
      if (cl.IsTmplt()) {
         if (cl.HasMethod("Class_Name")) {
           WriteClassName(cl,1);
         } else {
           fprintf(stderr, "Class %s: Class_Name() and initialization object"
                   " not declared\n", cl.Fullname());
         }
      } else {
         if (cl.HasMethod("Class_Name")) {
           WriteClassName(cl);
         }
      }

      if (cl.HasMethod("Streamer")) {
         //WriteStreamerBases(cl);
         if (cl.RootFlag()) WritePointersSTL(cl);
         if (!(cl.RootFlag() & G__NOSTREAMER)) {
            if ((cl.RootFlag() & G__USEBYTECOUNT /*G__AUTOSTREAMER*/)) {
               WriteAutoStreamer(cl);
            } else {
              WriteStreamer(cl);
            }
         } else
            fprintf(stderr, "Class %s: Do not generate Streamer() [*** custom streamer ***]\n", cl.Fullname());
      } else {
         fprintf(stderr, "Class %s: Streamer() not declared\n", cl.Fullname());
      }
      if (cl.HasMethod("ShowMembers")) {
         WriteShowMembers(cl);
      } else {
         fprintf(stderr, "Class %s: ShowMembers() not declared\n", cl.Fullname());
      }

   }

}

//______________________________________________________________________________
void GenerateLinkdef(int *argc, char **argv, int iv)
{
   FILE *fl = fopen(autold, "w");

   fprintf(fl, "#ifdef __CINT__\n\n");
   fprintf(fl, "#pragma link off all globals;\n");
   fprintf(fl, "#pragma link off all classes;\n");
   fprintf(fl, "#pragma link off all functions;\n\n");

   for (int i = iv; i < *argc; i++) {
      char *s, trail[3];
      int   nostr = 0, noinp = 0, bcnt = 0, l = strlen(argv[i])-1;
      for (int j = 0; j < 3; j++) {
         if (argv[i][l] == '-') {
            argv[i][l] = '\0';
            nostr = 1;
            l--;
         }
         if (argv[i][l] == '!') {
            argv[i][l] = '\0';
            noinp = 1;
            l--;
         }
         if (argv[i][l] == '+') {
            argv[i][l] = '\0';
            bcnt = 1;
            l--;
         }
      }
      if (nostr || noinp) {
         trail[0] = 0;
         if (nostr) strcat(trail, "-");
         if (noinp) strcat(trail, "!");
      }
      if (bcnt) {
         strcpy(trail, "+");
         if (nostr)
            fprintf(stderr, "option + mutual exclusive with -\n");
      }
      char *cls = strrchr(argv[i], '/');
      if (!cls) cls = strrchr(argv[i], '\\');
      if (cls)
         cls++;
      else
         cls = argv[i];
      if ((s = strrchr(cls, '.'))) *s = '\0';
      if (nostr || noinp || bcnt)
         fprintf(fl, "#pragma link C++ class %s%s;\n", cls, trail);
      else
         fprintf(fl, "#pragma link C++ class %s;\n", cls);
      if (s) *s = '.';
   }

   fprintf(fl, "\n#endif\n");
   fclose(fl);
}

//______________________________________________________________________________
const char *Which(const char *fname)
{
   // Find file name in path specified via -I statements to CINT.
   // Can be only called after G__main(). Return pointer to static
   // space containing full pathname or 0 in case file not found.

   static char pname[1024];
   FILE *fp = 0;

   sprintf(pname, "%s", fname);
#ifdef WIN32
   fp = fopen(pname, "rb");
#else
   fp = fopen(pname, "r");
#endif
   if (fp) {
      fclose(fp);
      return pname;
   }

   struct G__includepath *ipath = G__getipathentry();

   while (!fp && ipath->pathname) {
#ifdef WIN32
      sprintf(pname, "%s\\%s", ipath->pathname, fname);
      fp = fopen(pname, "rb");
#else
      sprintf(pname, "%s/%s", ipath->pathname, fname);
      fp = fopen(pname, "r");
#endif
      ipath = ipath->next;
   }
   if (fp) {
      fclose(fp);
      return pname;
   }
   return 0;
}

//______________________________________________________________________________
char *StrDup(const char *str)
{
   // Duplicate the string str. The returned string has to be deleted by
   // the user.

   if (!str) return 0;

   char *s = new char[strlen(str)+1];
   if (s) strcpy(s, str);

   return s;
}

//______________________________________________________________________________
char *Compress(const char *str)
{
   // Remove all blanks from the string str. The returned string has to be
   // deleted by the user.

   if (!str) return 0;

   const char *p = str;
   char *s, *s1 = new char[strlen(str)+1];
   s = s1;

   while (*p) {
      if (*p != ' ')
         *s++ = *p;
      p++;
   }
   *s = '\0';

   return s1;
}

//______________________________________________________________________________
void StrcpyWithEsc(char *escaped, const char *original)
{
   // Copy original into escaped BUT make sure that the \ characters
   // are properly escaped (on Windows temp files have \'s).

   int j, k;
   j = 0; k = 0;
   while (original[j] != '\0') {
      if (original[j] == '\\')
         escaped[k++] = '\\';
      escaped[k++] = original[j++];
   }
   escaped[k] = '\0';
}

//______________________________________________________________________________
void ReplaceBundleInDict(const char *dictname, const char *bundlename)
{
   // Replace the bundlename in the dict.cxx and .h file by the contents
   // of the bundle.

   // First patch dict.cxx. Create tmp file and copy dict.cxx to this file.
   // When discovering a line like:
   //   G__add_compiledheader("bundlename");
   // replace it by the appropriate number of lines contained in the bundle.

   FILE *fpd = fopen(dictname, "r");
   if (!fpd) {
      fprintf(stderr,"rootcint: failed to open %s in ReplaceBundleInDict()\n",
              dictname);
      return;
   }

   char tmpdictname[256];
   sprintf(tmpdictname, "%s_+_+_+rootcinttmp", dictname);
   FILE *tmpdict = fopen(tmpdictname, "w");
   if (!tmpdict) {
      fprintf(stderr,"rootcint: failed to open %s in ReplaceBundleInDict()\n",
              tmpdictname);
      fclose(fpd);
      return;
   }

   char esc_bundlename[256];
   StrcpyWithEsc(esc_bundlename, bundlename);

   char checkline[512];
   sprintf(checkline, "  G__add_compiledheader(\"%s\");", esc_bundlename);
   int clen = strlen(checkline);

   char line[BUFSIZ];
   if (tmpdict && fpd) {
      while (fgets(line, BUFSIZ, fpd)) {
         if (!strncmp(line, checkline, clen)) {
            FILE *fb = fopen(bundlename, "r");
            if (!fb) {
               fprintf(stderr,"rootcint: failed to open %s in ReplaceBundleInDict()\n",
                       bundlename);
               fclose(fpd);
               fclose(tmpdict);
               remove(tmpdictname);
               return;
            }
            while (fgets(line, BUFSIZ, fb)) {
               char *s = strchr(line, '"');
               if (!s) continue;
               s++;
               char *s1 = strrchr(s, '"');
               if (s1) {
                  *s1 = 0;
                  fprintf(tmpdict, "  G__add_compiledheader(\"%s\");\n", s);
               }
            }
            fclose(fb);
         } else
            fprintf(tmpdict, "%s", line);
      }
   }

   fclose(tmpdict);
   fclose(fpd);

   if (unlink(dictname) == -1 || rename(tmpdictname, dictname) == -1)
      fprintf(stderr,"rootcint: failed to rename %s to %s in ReplaceBundleInDict()\n",
              tmpdictname, dictname);

   // Next patch dict.h. Create tmp file and copy dict.h to this file.
   // When discovering a line like:
   //   #include "bundlename"
   // replace it by the appropriate number of lines contained in the bundle.

   // make dict.h
   char dictnameh[512];
   strcpy(dictnameh, dictname);
   char *s = strrchr(dictnameh, '.');
   if (s) {
      *(s+1) = 'h';
      *(s+2) = 0;
   } else {
      fprintf(stderr,"rootcint: failed create dict.h in ReplaceBundleInDict()\n");
      return;
   }

   fpd = fopen(dictnameh, "r");
   if (!fpd) {
      fprintf(stderr,"rootcint: failed to open %s in ReplaceBundleInDict()\n",
              dictnameh);
      return;
   }
   tmpdict = fopen(tmpdictname, "w");

   sprintf(checkline, "#include \"%s\"", esc_bundlename);
   clen = strlen(checkline);

   if (tmpdict && fpd) {
      while (fgets(line, BUFSIZ, fpd)) {
         if (!strncmp(line, checkline, clen)) {
            FILE *fb = fopen(bundlename, "r");
            while (fgets(line, BUFSIZ, fb))
               fprintf(tmpdict, "%s", line);
            fclose(fb);
         } else
            fprintf(tmpdict, "%s", line);
      }
   }

   fclose(tmpdict);
   fclose(fpd);

   if (unlink(dictnameh) == -1 || rename(tmpdictname, dictnameh) == -1)
      fprintf(stderr,"rootcint: failed to rename %s to %s in ReplaceBundleInDict()\n",
              tmpdictname, dictnameh);
}

//______________________________________________________________________________
int main(int argc, char **argv)
{
#ifdef __MWERKS__
   argc = ccommand(&argv);
#endif

   if (argc < 2) {
      fprintf(stderr,
      "Usage: %s [-f] [out.cxx] [-c] file1.h[+][-][!] file2.h[+][-][!]...[LinkDef.h]\n",
              argv[0]);
      fprintf(stderr, "For more extensive help type: %s -h\n", argv[0]);
      return 1;
   }

   char dictname[256];
   int i, ic, ifl, force;
   int icc = 0;
   int use_preprocessor = 0;

   sprintf(autold, autoldtmpl, getpid());

   if (!strcmp(argv[1], "-f")) {
      force = 1;
      ic    = 2;
   } else {
      force = 0;
      ic    = 1;
   }

   if (strstr(argv[ic],".C")  || strstr(argv[ic],".cpp") ||
       strstr(argv[ic],".cp") || strstr(argv[ic],".cxx") ||
       strstr(argv[ic],".cc")) {
      if ((fp = fopen(argv[ic], "r")) != 0) {
         fclose(fp);
         if (!force) {
            fprintf(stderr, "%s: output file %s already exists\n", argv[0], argv[ic]);
            return 1;
         }
      }
      fp = fopen(argv[ic], "w");
      if (fp) fclose(fp);    // make sure file is created and empty
      ifl = ic;
      ic++;

      // remove possible pathname to get the dictionary name
      strcpy(dictname, argv[ifl]);
      char *p = strrchr(dictname, '/');
      if (!p)
         p = dictname;
      else
         p++;
      strcpy(dictname, p);
   } else if (!strcmp(argv[1], "-?") || !strcmp(argv[1], "-h")) {
      fprintf(stderr, "%s\n", help);
      return 1;
   } else {
      fp = stdout;
      ic = 1;
      if (force) ic = 2;
      ifl = 0;
   }

#ifndef __CINT__
   int   argcc, iv, il;
   char  path1[128], path2[128];
   char *argvv[500];

   path1[0] = path2[0] = 0;

#ifndef ROOTINCDIR
# ifndef ROOTBUILD
   if (getenv("ROOTSYS")) {
#  ifdef __MWERKS__
      sprintf(path1,"-I%s:include", getenv("ROOTSYS"));
      sprintf(path2,"-I%s:src", getenv("ROOTSYS"));
#  else
      sprintf(path1,"-I%s/include", getenv("ROOTSYS"));
      sprintf(path2,"-I%s/src", getenv("ROOTSYS"));
#  endif
   } else {
      fprintf(stderr, "%s: env var ROOTSYS not defined\n", argv[0]);
      return 1;
   }
# else
   sprintf(path1,"-Iinclude");
# endif
#else
   sprintf(path1,"-I%s", ROOTINCDIR);
#endif

   argvv[0] = argv[0];
   argcc = 1;

   if (!strcmp(argv[ic], "-c")) {
      icc++;
      if (ifl) {
         char *s;
         ic++;
         argvv[argcc++] = "-q0";
         argvv[argcc++] = "-n";
         argvv[argcc] = (char *)calloc(strlen(argv[ifl])+1, 1);
         strcpy(argvv[argcc], argv[ifl]); argcc++;
         argvv[argcc++] = "-N";
         s = strchr(dictname,'.');
         argvv[argcc] = (char *)calloc(strlen(dictname), 1);
         strncpy(argvv[argcc], dictname, s-dictname); argcc++;

         while (ic < argc && (*argv[ic] == '-' || *argv[ic] == '+')) {
            argvv[argcc++] = argv[ic++];
         }

         argvv[argcc++] = path1;
         if (strlen(path2))
            argvv[argcc++] = path2;
#ifdef __hpux
         argvv[argcc++] = "-I/usr/include/X11R5";
#endif
         argvv[argcc++] = "-DTRUE=1";
         argvv[argcc++] = "-DFALSE=0";
         argvv[argcc++] = "-Dexternalref=extern";
         argvv[argcc++] = "-DSYSV";
         argvv[argcc++] = "-D__MAKECINT__";
         argvv[argcc++] = "-V";        // include info on private members
         argvv[argcc++] = "-c-1";
         argvv[argcc++] = "+V";        // turn on class comment mode
         argvv[argcc++] = "TROOT.h";
         argvv[argcc++] = "TMemberInspector.h";
      } else {
         fprintf(stderr, "%s: option -c can only be used when an output file has been specified\n", argv[0]);
         return 1;
      }
   }

   iv = 0;
   il = 0;
   // If the user request use of a preprocessor we are going to bundle
   // all the files into one so that cint considers them one compilation
   // unit and so that each file that contains code guard is really
   // included only once.
   for (i = 1; i < argc; i++)
      if (strcmp(argv[i], "-p") == 0) use_preprocessor = 1;

   char bundlename[L_tmpnam+2];
   char esc_arg[256];
   FILE *bundle = 0;
   if (use_preprocessor) {
      tmpnam(bundlename);
      strcat(bundlename,".h");
      bundle = fopen(bundlename, "w");
      if (!bundle) {
         fprintf(stderr,"%s: failed to open %s, usage of external preprocessor by CINT is not optimal\n",
                 argv[0], bundlename);
         use_preprocessor = 0;
      }
   }
   for (i = ic; i < argc; i++) {
      if (!iv && *argv[i] != '-' && *argv[i] != '+') {
         if (!icc) {
            argvv[argcc++] = path1;
            argvv[argcc++] = path2;
            argvv[argcc++] = "+V";
         }
         iv = i;
      }
      if ((strstr(argv[i],"LinkDef") || strstr(argv[i],"Linkdef") ||
           strstr(argv[i],"linkdef")) && strstr(argv[i],".h")) {
         il = i;
         if (i != argc-1) {
            fprintf(stderr, "%s: %s must be last file on command line\n", argv[0], argv[i]);
            return 1;
         }
         if (use_preprocessor) argvv[argcc++] = bundlename;
      }
      if (!strcmp(argv[i], "-c")) {
         fprintf(stderr, "%s: option -c must come directly after the output file\n", argv[0]);
         return 1;
      }
      if (use_preprocessor && *argv[i] != '-' && *argv[i] != '+' && !il) {
         StrcpyWithEsc(esc_arg, argv[i]);
         fprintf(bundle,"#include \"%s\"\n", esc_arg);
      } else
         argvv[argcc++] = argv[i];
   }
   if (use_preprocessor) {
      // Since we have not seen a linkdef file, we have not yet added the
      // bundle file to the command line!
      if (!il) argvv[argcc++] = bundlename;
      fclose(bundle);
   }

   if (!iv) {
      fprintf(stderr, "%s: no input files specified\n", argv[0]);
      return 1;
   }

   if (!il) {
      GenerateLinkdef(&argc, argv, iv);
      argvv[argcc++] = autold;
   }

   G__setothermain(2);
   if (G__main(argcc, argvv) < 0) {
      fprintf(stderr, "%s: error loading headers...\n", argv[0]);
      return 1;
   }
   G__setglobalcomp(0);  // G__NOLINK

#endif

   if (use_preprocessor && icc)
      ReplaceBundleInDict(argv[ifl], bundlename);

   // Check if code goes to stdout or cint file, use temporary file
   // for prepending of the rootcint generated code (STK)
   char tname[L_tmpnam];
   if (ifl) {
      tmpnam(tname);
      fp = fopen(tname, "w");
   } else
      fp = stdout;

   time_t t = time(0);
   fprintf(fp, "//\n// File generated by %s at %.24s.\n", argv[0], ctime(&t));
   fprintf(fp, "// Do NOT change. Changes will be lost next time file is generated\n//\n\n");
   fprintf(fp, "#include \"TClass.h\"\n");
   fprintf(fp, "#include \"TBuffer.h\"\n");
   fprintf(fp, "#include \"TMemberInspector.h\"\n");
   fprintf(fp, "#include \"TError.h\"\n\n");
   fprintf(fp, "#ifndef G__ROOT\n");
   fprintf(fp, "#define G__ROOT\n");
   fprintf(fp, "#endif\n\n");

   // Loop over all command line arguments and write include statements.
   // Skip options and any LinkDef.h.
   if (ifl && !icc) {
      for (i = ic; i < argc; i++) {
         if (*argv[i] != '-' && *argv[i] != '+' &&
             !((strstr(argv[i],"LinkDef") || strstr(argv[i],"Linkdef") ||
                strstr(argv[i],"linkdef")) && strstr(argv[i],".h")))
            fprintf(fp, "#include \"%s\"\n", argv[i]);
      }
      fprintf(fp, "\n");
   }

   // Loop over all classes and create Streamer() & Showmembers() methods
   G__ClassInfo cl;

#if 0
   // Write all TBuffer &operator>>(...) first to allow template
   // specialisation to occur before template instantiation (STK)
   while (cl.Next()) {
      if ((cl.Property() & G__BIT_ISCLASS) && cl.Linkage() == G__CPPLINK) {
         if (cl.HasMethod("Class")) {
         }
      }
   }
#endif

   // Open LinkDef file for reading, so that we can process classes
   // in order of appearence in this file (STK)
   FILE *fpld = 0;
   if (!il) {
      // Open auto-generated file
      fpld = fopen(autold, "r");
   } else {
      // Open file specified on command line
      fpld = fopen(Which(argv[il]), "r");
   }
   if (!fpld) {
      fprintf(stderr, "%s: cannot open file %s\n", argv[0], il ? argv[il] : autold);
      if (use_preprocessor) remove(bundlename);
      if (!il) remove(autold);
      if (ifl) {
         remove(tname);
         remove(argv[ifl]);
      }
      return 1;
   }

   // Keep track of classes processed by reading Linkdef file.
   // When all classes in LinkDef are done, loop over all classes known
   // to CINT output the ones that were not in the LinkDef. This can happen
   // in case "#pragma link C++ defined_in" is used.
   const int kMaxClasses = 2000;
   char *clProcessed[kMaxClasses];
   int   ncls = 0;

   // Read LinkDef file and process valid entries (STK)
   char line[256];
   while (fgets(line, 256, fpld)) {

      // Check if the line contains a "#pragma link C++ class" specification,
      // if so, process the class (STK)
      if ((strcmp(strtok(line, " "), "#pragma") == 0) &&
          (strcmp(strtok(0, " "), "link") == 0) &&
          (strcmp(strtok(0, " "), "C++") == 0) &&
          (strcmp(strtok(0, " " ), "class") == 0)) {

         // Create G__ClassInfo object for this class and process. Be
         // careful with the hardcoded string of trailing options in case
         // these change (STK)
         char *request = strtok(0, "-!+;");
         // just in case remove trailing space and tab
         while (*request == ' ') request++;
         int len = strlen(request)-1;
         while (request[len]==' ' || request[len]=='\t') request[len--] = '\0';
         request = Compress(request); //no space between tmpl arguments allowed
         G__ClassInfo cl(request);
         if (cl.IsValid())
            clProcessed[ncls] = StrDup(cl.Fullname());
         else
            clProcessed[ncls] = StrDup(request);
         ncls++;
         delete [] request;

         WriteClassCode(cl);
      }
   }

   // Loop over all classes and create Streamer() & ShowMembers() methods
   // for classes not in clProcessed list (exported via
   // "#pragma link C++ defined_in")
   cl.Init();

   while (cl.Next()) {
      int nxt = 0;
      // skip utility class defined in ClassImp
      if (!strncmp(cl.Fullname(), "R__Init", 7) ||
           strstr(cl.Fullname(), "::R__Init"))
         continue;
      for (i = 0; i < ncls; i++)
         if (!strcmp(clProcessed[i], cl.Fullname())) {
            nxt++;
            break;
         }
      if (nxt) continue;

      WriteClassCode(cl);
   }

   // Write all TBuffer &operator>>(...) first to allow template
   // specialisation to occur before template instantiation (STK)
   cl.Init();
   while (cl.Next()) {
      if ((cl.Property() & G__BIT_ISCLASS) && cl.Linkage() == G__CPPLINK) {
         if (cl.HasMethod("Streamer")) {
            if (!(cl.RootFlag() & G__NOINPUTOPERATOR)) {
               WriteInputOperator(cl);
            } else {
               fprintf(stderr, "Class %s: Do not generate operator>>()\n",
                       cl.Fullname());
            }
         }
      }
   }

   fclose(fp);
   fclose(fpld);

   if (!il) remove(autold);
   if (use_preprocessor) remove(bundlename);

   // Append CINT dictionary to file containing Streamers and ShowMembers
   if (ifl) {
      char line[BUFSIZ];
      FILE *fpd = fopen(argv[ifl], "r");
      fp = fopen(tname, "a");

      if (fp && fpd)
         while (fgets(line, BUFSIZ, fpd))
            fprintf(fp, "%s", line);

      if (fp)  fclose(fp);
      if (fpd) fclose(fpd);

      // copy back to dictionary file
      fpd = fopen(argv[ifl], "w");
      fp  = fopen(tname, "r");

      if (fp && fpd) {

         // make name of dict include file "aapDict.cxx" -> "aapDict.h"
         int  nl = 0;
         char inclf[512];
         char *s = strrchr(dictname, '.');
         if (s) *s = 0;
         sprintf(inclf, "%s.h", dictname);
         if (s) *s = '.';

         // during copy put dict include on top and remove later reference
         while (fgets(line, BUFSIZ, fp)) {
            if (!strncmp(line, "#include", 8) && strstr(line, inclf))
               continue;
            fprintf(fpd, "%s", line);
            if (++nl == 4 && icc)
               fprintf(fpd, "#include \"%s\"\n", inclf);
         }
      }

      if (fp)  fclose(fp);
      if (fpd) fclose(fpd);
      remove(tname);
   }

   G__setglobalcomp(-1);  // G__CPPLINK
   G__exit(0);

   return 0;
}
