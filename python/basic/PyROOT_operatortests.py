# File: roottest/python/basic/PyROOT_operatortests.py
# Author: Wim Lavrijsen (LBNL, WLavrijsen@lbl.gov)
# Created: 06/04/05
# Last: 05/05/15

"""C++ operators interface unit tests for PyROOT package."""

import sys, os, unittest
sys.path.append( os.path.join( os.getcwd(), os.pardir ) )

from ROOT import *
from common import *

__all__ = [
   'Cpp1OperatorsTestCase',
   'Cpp2ConverterOperatorsTestCase',
   'Cpp3TemplatedMathOperatorsTestCase'
]

gROOT.LoadMacro( "Operators.C+" )


### C++ operators overloading test cases =====================================
class Cpp1OperatorsTestCase( MyTestCase ):
   def test1MathOperators( self ):
      """Test overloading of math operators"""

      self.failUnlessEqual( Number(20) + Number(10), Number(30) )
      self.failUnlessEqual( Number(20) + 10        , Number(30) )
      self.failUnlessEqual( Number(20) - Number(10), Number(10) )
      self.failUnlessEqual( Number(20) - 10        , Number(10) )
      self.failUnlessEqual( Number(20) / Number(10), Number(2) )
      self.failUnlessEqual( Number(20) / 10        , Number(2) )
      self.failUnlessEqual( Number(20) * Number(10), Number(200) )
      self.failUnlessEqual( Number(20) * 10        , Number(200) )
      self.failUnlessEqual( Number(20) % 10        , Number(0) )
      self.failUnlessEqual( Number(20) % Number(10), Number(0) )
      self.failUnlessEqual( Number(5)  & Number(14), Number(4) )
      self.failUnlessEqual( Number(5)  | Number(14), Number(15) )
      self.failUnlessEqual( Number(5)  ^ Number(14), Number(11) )
      self.failUnlessEqual( Number(5)  << 2, Number(20) )
      self.failUnlessEqual( Number(20) >> 2, Number(5) )

   def test2UnaryMathOperators( self ):
      """Test overloading of unary math operators"""

      n  = Number(20)
      n += Number(10)
      n -= Number(10)
      n *= Number(10)
      n /= Number(2)
      self.failUnlessEqual( n, Number(100) )

      nn = -n;
      self.failUnlessEqual( nn, Number( -100 ) )

   def test3ComparisonOperators( self ):
      """Test overloading of comparison operators"""

      self.failUnlessEqual( Number(20) >  Number(10), 1 )
      self.failUnlessEqual( Number(20) <  Number(10), 0 )
      self.failUnlessEqual( Number(20) >= Number(20), 1 )
      self.failUnlessEqual( Number(20) <= Number(10), 0 )
      self.failUnlessEqual( Number(20) != Number(10), 1 )
      self.failUnlessEqual( Number(20) == Number(10), 0 )

   def test4BooleanOperator( self ):
      """Test implementation of operator bool"""

      n = Number(20)
      self.assert_( n )

      n = Number( 0 )
      self.assert_( not n )


### Instance to builtin type converters test cases ===========================
class Cpp2ConverterOperatorsTestCase( MyTestCase ):
   def test1ExactTypes( self ):
      """Test converter operators of exact types"""

      o = OperatorCharStar()
      self.assertEqual( o.m_str, 'OperatorCharStar' )
      self.assertEqual( str(o),  'OperatorCharStar' )

      o = OperatorConstCharStar()
      self.assertEqual( o.m_str, 'OperatorConstCharStar' )
      self.assertEqual( str(o),  'OperatorConstCharStar' )

      o = OperatorInt(); o.m_int = -13
      self.assertEqual( o.m_int,   -13 )
      self.assertEqual( int( o ),  -13 )

      o = OperatorLong(); o.m_long = 42
      self.assertEqual( o.m_long,    42 )
      self.assertEqual( pylong( o ), 42 )

      o = OperatorDouble(); o.m_double = 3.1415
      self.assertEqual( o.m_double,      3.1415 )
      self.assertEqual( float( o ),      3.1415 )

   def test2ApproximateTypes( self ):
      """Test converter operators of approximate types"""

      o = OperatorShort(); o.m_short = 256
      self.assertEqual( o.m_short,     256 )
      self.assertEqual( int( o ),      256 )

      o = OperatorUnsignedInt(); o.m_uint = 2147483647 + 32
      self.assertEqual( o.m_uint,           2147483647 + 32 )
      self.assertEqual( pylong( o ),        2147483647 + 32 )

      o = OperatorUnsignedLong(); o.m_ulong = maxvalue + 128
      self.assertEqual( o.m_ulong,            maxvalue + 128 )
      self.assertEqual( pylong( o ),          maxvalue + 128 )

      o = OperatorFloat(); o.m_float =      3.14
      self.assertEqual( round( o.m_float  - 3.14, 5 ), 0. )
      self.assertEqual( round( float( o ) - 3.14, 5 ), 0. )


### Templated math operators =================================================
class Cpp3TemplatedMathOperatorsTestCase( MyTestCase ):
   def test01LorentzVector( self ):
      """Templated method operator+/-"""

      v1 = ROOT.Math.LorentzVector('ROOT::Math::PxPyPzE4D<double>')(1, 2, 3, 4)
      v2 = ROOT.Math.LorentzVector('ROOT::Math::PxPyPzE4D<double>')(4, 3, 2, 1)

      v3 = v1.__add__( v2 )
      self.assertEqual( v3.X(), v1.X() + v2.X() )
      v4 = v1 + v2
      self.assertEqual( v4.X(), v1.X() + v2.X() )
      self.assertEqual( v3, v4 )


## actual test run
if __name__ == '__main__':
   from MyTextTestRunner import MyTextTestRunner

   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = MyTextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )
