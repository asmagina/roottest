# File: roottest/python/pickle/PyROOT_readingtests.py
# Author: Wim Lavrijsen (LBNL, WLavrijsen@lbl.gov)
# Created: 04/16/08
# Last: 04/16/08

"""Pickle writing unit tests for PyROOT package."""

import os, sys, unittest
import pickle, cPickle
from ROOT import *

__all__ = [
   'PickleReadingSimpleObjectsTestCase'
]

gROOT.LoadMacro( "PickleTypes.C+" )

from common import *


### Read various objects with the two pickle modules =========================
class PickleReadingSimpleObjectsTestCase( unittest.TestCase ):
   in1 = open( pclfn, 'r' )        # names from common.py
   in2 = open( cpclfn, 'r' )

 # note that the order of these tests have to match the writing order (for
 # simple indexing, shelve should have been used instead); this also means
 # that if reading of one test fails, everything downstream fails as well
   def test1ReadTObjectDerived( self ):
      """Test reading of a histogram from a pickle file"""

      def __doh1test( self, h1 ):
         self.assertEqual( h1.__class__, TH1F )
         self.assertEqual( h1.GetName(),     h1name )
         self.assertEqual( h1.GetTitle(),    h1title )
         self.assertEqual( h1.GetNbinsX(),   h1nbins )

      h1 = pickle.load( self.in1 )
      __doh1test( self, h1 )

      h1 = cPickle.load( self.in2 )
      __doh1test( self, h1 )

   def test2ReadNonTObjectDerived( self ):
      """Test reading of an std::vector<double> from a pickle file"""

      def __dovtest( self, v ):
         self.assertEqual( v.__class__, std.vector( 'double' ) )
         self.assertEqual( v.size(), Nvec )

         for i in range( Nvec ):
            self.assertEqual( v[i], i*i )

      v = pickle.load( self.in1 )
      __dovtest( self, v )

      v = cPickle.load( self.in2 )
      __dovtest( self, v )


   def test3ReadSomeDataObject( self ):
      """Test reading of a user-defined object from a pickle file"""

      def __dodtest( self, d ):
         self.assertEqual( d.__class__, SomeDataObject )

         i = 0
         for entry in d.GetFloats():
            self.assertEqual( i, int(entry) )
            i += 1

         for mytuple in d.GetTuples():
            i = 0
            for entry in mytuple:
               self.assertEqual( i, int(entry) )
               i += 1

      d = pickle.load( self.in1 )
      __dodtest( self, d )

      d = cPickle.load( self.in2 )
      __dodtest( self, d )


## actual test run
if __name__ == '__main__':
   sys.path.append( os.path.join( os.getcwd(), os.pardir ) )
   from MyTextTestRunner import MyTextTestRunner

   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = MyTextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )