// __BEGIN_LICENSE__
// 
// Copyright (C) 2006 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration
// (NASA).  All Rights Reserved.
// 
// Copyright 2006 Carnegie Mellon University. All rights reserved.
// 
// This software is distributed under the NASA Open Source Agreement
// (NOSA), version 1.3.  The NOSA has been approved by the Open Source
// Initiative.  See the file COPYING at the top of the distribution
// directory tree for the complete NOSA document.
// 
// THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY
// KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT
// LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO
// SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
// A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT
// THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT
// DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE.
// 
// __END_LICENSE__

/// \file Functors.h
/// 
/// General-purpose functors and functor support code.
/// 
/// This file provides a few mix-in base classes that you can use when
/// writing your own polymorphic functors.  They instruct the return
/// type deduction system about the return type of your functor in
/// some of the common cases.
///
/// It also provides a collection of functors implementing the
/// standard arithmetic operations and so forth, which also serve to
/// demonstrate how to use the mix-ins.
///
#ifndef __VW_CORE_FUNCTORS_H__
#define __VW_CORE_FUNCTORS_H__

#include <vw/Core/TypeDeduction.h>

namespace vw {

  /// A mix-in specifying that a functor is an unary functor 
  /// whose return type is the same as its argument type.
  struct UnaryReturnSameType {
    /// \cond INTERNAL
    template <class Args> struct result;
    template <class ChannelT, class FuncT>
    struct result<FuncT(ChannelT)> {
      typedef ChannelT type;
    };    
    /// \endcond
  };

  /// A mix-in specifying that a functor is a unary functor 
  /// whose return type is determined by the given traits 
  /// template class.
  template <template<class> class ResultT>
  struct UnaryReturnTemplateType {
    /// \cond INTERNAL
    template <class Args> struct result {};
    template <class FuncT, class ArgT>
    struct result<FuncT(ArgT)> {
      typedef typename ResultT<ArgT>::type type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor is a binary functor 
  /// whose return type is determined by the given traits 
  /// template class.
  template <template<class,class> class ResultT>
  struct BinaryReturnTemplateType {
    /// \cond INTERNAL
    template <class Args> struct result {};
    template <class FuncT, class Arg1T, class Arg2T>
    struct result<FuncT(Arg1T,Arg2T)> {
      typedef typename ResultT<Arg1T,Arg2T>::type type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor always returns a 
  /// particular type.
  template <class T>
  struct ReturnFixedType {
    /// \cond INTERNAL
    template <class Args>
    struct result {
      typedef T type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor is an unary functor 
  /// whose return type is determined by the given binary 
  /// type function with the given first argument.
  template <template<class,class> class ResultT, class ValT>
  struct UnaryReturnBinaryTemplateBind1st {
    /// \cond INTERNAL
    template <class Args> struct result { typedef void type; };
    template <class FuncT, class ArgT>
    struct result<FuncT(ArgT)> {
      typedef typename ResultT<ValT,ArgT>::type type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor is an unary functor 
  /// whose return type is determined by the given binary 
  /// type function with the given second argument.
  template <template<class,class> class ResultT, class ValT>
  struct UnaryReturnBinaryTemplateBind2nd {
    /// \cond INTERNAL
    template <class Args> struct result { typedef void type; };
    template <class FuncT, class ArgT>
    struct result<FuncT(ArgT)> {
      typedef typename ResultT<ArgT,ValT>::type type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor is a binary functor 
  /// whose return type is determined by the given ternary 
  /// type function with the given first argument.
  template <template<class,class,class> class ResultT, class ValT>
  struct BinaryReturnTernaryTemplateBind1st {
    /// \cond INTERNAL
    template <class Args> struct result { typedef void type; };
    template <class FuncT, class Arg1T, class Arg2T>
    struct result<FuncT(Arg1T,Arg2T)> {
      typedef typename ResultT<ValT,Arg1T,Arg2T>::type type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor is an unary functor 
  /// whose return type is determined by the given ternary 
  /// type function with the given first and second arguments.
  template <template<class,class,class> class ResultT, class Val1T, class Val2T>
  struct UnaryReturnTernaryTemplateBind1st2nd {
    /// \cond INTERNAL
    template <class Args> struct result { typedef void type; };
    template <class FuncT, class ArgT>
    struct result<FuncT(ArgT)> {
      typedef typename ResultT<Val1T,Val2T,ArgT>::type type;
    };
    /// \endcond
  };

  /// A mix-in specifying that a functor is an unary functor 
  /// whose return type is determined by the given ternary 
  /// type function with the given first and third arguments.
  template <template<class,class,class> class ResultT, class Val1T, class Val3T>
  struct UnaryReturnTernaryTemplateBind1st3rd {
    /// \cond INTERNAL
    template <class Args> struct result { typedef void type; };
    template <class FuncT, class ArgT>
    struct result<FuncT(ArgT)> {
      typedef typename ResultT<Val1T,ArgT,Val3T>::type type;
    };
    /// \endcond
  };

  // XXX What on earth is this for? :
  template <template<class,class> class FuncT, class ValT>
  struct BinaryBind1st {
    template <class ArgT>
    struct bound_type {
      typedef typename FuncT<ValT,ArgT>::type type;
    };
  };


  // ********************************************************************
  // PixelType Arithmetic Operator Functors
  // ********************************************************************

  // Unary negation of an argument
  struct ArgNegationFunctor : UnaryReturnSameType {
    template <class ArgT>
    typename result<ArgNegationFunctor(ArgT)>::type
    inline operator()( ArgT const& arg ) const { return -arg; }
  };

  // Binary sum of two arguments
  struct ArgArgSumFunctor : BinaryReturnTemplateType<SumType> {
    template <class Arg1T, class Arg2T>
    inline typename SumType<Arg1T,Arg2T>::type operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1+arg2; }
  };

  // Unary sum of an argument and a value
  template <class ValT>
  struct ArgValSumFunctor : UnaryReturnBinaryTemplateBind2nd<SumType,ValT> {
  private:
    const ValT m_val;
  public:
    ArgValSumFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename SumType<ArgT,ValT>::type operator()( ArgT const& arg ) const { return arg+m_val; }
  };

  // Unary sum of a value and an argument
  template <class ValT>
  struct ValArgSumFunctor : UnaryReturnBinaryTemplateBind1st<SumType,ValT> {
  private:
    const ValT m_val;
  public:
    ValArgSumFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename SumType<ValT,ArgT>::type operator()( ArgT const& arg ) const { return m_val+arg; }
  };

  // Binary difference of two arguments
  struct ArgArgDifferenceFunctor : BinaryReturnTemplateType<DifferenceType> {
    template <class Arg1T, class Arg2T>
    inline typename DifferenceType<Arg1T,Arg2T>::type operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1-arg2; }
  };
  
  // Unary difference of an argument and a value
  template <class ValT>
  struct ArgValDifferenceFunctor : UnaryReturnBinaryTemplateBind2nd<DifferenceType,ValT> {
  private:
    const ValT m_val;
  public:
    ArgValDifferenceFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename DifferenceType<ArgT,ValT>::type operator()( ArgT const& arg ) const { return arg-m_val; }
  };

  // Unary difference of a value and an argument
  template <class ValT>
  struct ValArgDifferenceFunctor : UnaryReturnBinaryTemplateBind1st<DifferenceType,ValT> {
  private:
    const ValT m_val;
  public:
    ValArgDifferenceFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename DifferenceType<ValT,ArgT>::type operator()( ArgT const& arg ) const { return m_val-arg; }
  };

  // Binary product of two arguments
  struct ArgArgProductFunctor : BinaryReturnTemplateType<ProductType> {
    template <class Arg1T, class Arg2T>
    inline typename ProductType<Arg1T,Arg2T>::type operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1*arg2; }
  };

  // Unary product of an argument and a value
  template <class ValT>
  struct ArgValProductFunctor : UnaryReturnTemplateType<BinaryBind1st<ProductType,ValT>::template bound_type> {//UnaryReturnBinaryTemplateBind2nd<ProductType,ValT> {
  private:
    const ValT m_val;
  public:
    ArgValProductFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename ProductType<ArgT,ValT>::type operator()( ArgT const& arg ) const { return arg*m_val; }
  };

  // Unary product of a value and an argument
  template <class ValT>
  struct ValArgProductFunctor : UnaryReturnBinaryTemplateBind1st<ProductType,ValT> {
  private:
    const ValT m_val;
  public:
    ValArgProductFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename ProductType<ValT,ArgT>::type operator()( ArgT const& arg ) const { return m_val*arg; }
  };

  // Binary quotient of two arguments
  struct ArgArgQuotientFunctor : BinaryReturnTemplateType<QuotientType> {
    template <class Arg1T, class Arg2T>
    inline typename QuotientType<Arg1T,Arg2T>::type operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1/arg2; }
  };

  // Unary quotient of an argument and a value
  template <class ValT>
  struct ArgValQuotientFunctor : UnaryReturnBinaryTemplateBind2nd<QuotientType,ValT> {
  private:
    const ValT m_val;
  public:
    ArgValQuotientFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename QuotientType<ArgT,ValT>::type operator()( ArgT const& arg ) const { return arg/m_val; }
  };

  // Unary quotient of a value and an argument
  template <class ValT>
  struct ValArgQuotientFunctor : UnaryReturnBinaryTemplateBind1st<QuotientType,ValT> {
  private:
    const ValT m_val;
  public:
    ValArgQuotientFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename QuotientType<ValT,ArgT>::type operator()( ArgT const& arg ) const { return m_val/arg; }
  };

  // **** WARNING ****
  // The syntax in all three SafeQuotient functors is carefully 
  // crafted to work around an issue with RedHat's gcc 3.2.3-56.
  // If you change this, be sure to test it on an RHEL3 box.
  // **** WARNING ****

  // Safe binary quotient of two arguments
  struct ArgArgSafeQuotientFunctor : BinaryReturnTemplateType<QuotientType> {
    template <class Arg1T, class Arg2T>
    inline typename QuotientType<Arg1T,Arg2T>::type operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { 
      if( arg2==Arg2T() ) return typename QuotientType<Arg1T,Arg2T>::type();
      else return (arg1/arg2);
    }
  };

  // Safe unary quotient of an argument and a value
  template <class ValT>
  struct ArgValSafeQuotientFunctor : UnaryReturnBinaryTemplateBind2nd<QuotientType,ValT> {
  private:
    const ValT m_val;
  public:
    ArgValSafeQuotientFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename QuotientType<ArgT,ValT>::type operator()( ArgT const& arg ) const {
      if ( m_val==ValT() ) return typename QuotientType<ArgT,ValT>::type();
      else return (arg/m_val);
    }
  };

  // Safe unary quotient of a value and an argument
  template <class ValT>
  struct ValArgSafeQuotientFunctor : UnaryReturnBinaryTemplateBind1st<QuotientType,ValT> {
  private:
    const ValT m_val;
  public:
    ValArgSafeQuotientFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline typename QuotientType<ValT,ArgT>::type operator()( ArgT const& arg ) const {
      if ( arg==ArgT() ) return typename QuotientType<ValT,ArgT>::type();
      else return (m_val/arg);
    }
  };

  // Binary equality operator of two arguments
  struct ArgArgEqualityFunctor : ReturnFixedType<bool> {
    template <class Arg1T, class Arg2T>
    inline bool operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1==arg2; }
  };

  // Unary equality operator of an argument and a value
  template <class ValT>
  struct ArgValEqualityFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ArgValEqualityFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return arg==m_val; }
  };

  // Unary equality operator of a value and an argument
  template <class ValT>
  struct ValArgEqualityFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ValArgEqualityFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return m_val==arg; }
  };

  // Binary inequality operator of two arguments
  struct ArgArgInequalityFunctor : ReturnFixedType<bool> {
    template <class Arg1T, class Arg2T>
    inline bool operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1!=arg2; }
  };

  // Unary inequality operator of an argument and a value
  template <class ValT>
  struct ArgValInequalityFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ArgValInequalityFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return arg!=m_val; }
  };

  // Unary inequality operator of a value and an argument
  template <class ValT>
  struct ValArgInequalityFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ValArgInequalityFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return m_val!=arg; }
  };

  // Binary less-than operator of two arguments
  struct ArgArgLessThanFunctor : ReturnFixedType<bool> {
    template <class Arg1T, class Arg2T>
    inline bool operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1<arg2; }
  };

  // Unary less-than operator of an argument and a value
  template <class ValT>
  struct ArgValLessThanFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ArgValLessThanFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return arg<m_val; }
  };

  // Unary less-than operator of a value and an argument
  template <class ValT>
  struct ValArgLessThanFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ValArgLessThanFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return m_val<arg; }
  };

  // Binary less-than-or-equal operator of two arguments
  struct ArgArgLessThanOrEqualFunctor : ReturnFixedType<bool> {
    template <class Arg1T, class Arg2T>
    inline bool operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1<=arg2; }
  };

  // Unary less-than-or-equal operator of an argument and a value
  template <class ValT>
  struct ArgValLessThanOrEqualFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ArgValLessThanOrEqualFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return arg<=m_val; }
  };

  // Unary less-than-or-equal operator of a value and an argument
  template <class ValT>
  struct ValArgLessThanOrEqualFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ValArgLessThanOrEqualFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return m_val<=arg; }
  };

  // Binary greater-than operator of two arguments
  struct ArgArgGreaterThanFunctor : ReturnFixedType<bool> {
    template <class Arg1T, class Arg2T>
    inline bool operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1>arg2; }
  };

  // Unary greater-than operator of an argument and a value
  template <class ValT>
  struct ArgValGreaterThanFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ArgValGreaterThanFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return arg>m_val; }
  };

  // Unary greater-than operator of a value and an argument
  template <class ValT>
  struct ValArgGreaterThanFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ValArgGreaterThanFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return m_val>arg; }
  };

  // Binary greater-than-or-equal operator of two arguments
  struct ArgArgGreaterThanOrEqualFunctor : ReturnFixedType<bool> {
    template <class Arg1T, class Arg2T>
    inline bool operator()( Arg1T const& arg1, Arg2T const& arg2 ) const { return arg1>=arg2; }
  };

  // Unary greater-than-or-equal operator of an argument and a value
  template <class ValT>
  struct ArgValGreaterThanOrEqualFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ArgValGreaterThanOrEqualFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return arg>=m_val; }
  };

  // Unary greater-than-or-equal operator of a value and an argument
  template <class ValT>
  struct ValArgGreaterThanOrEqualFunctor : ReturnFixedType<bool> {
  private:
    const ValT m_val;
  public:
    ValArgGreaterThanOrEqualFunctor( ValT const& val ) : m_val(val) {}

    template <class ArgT>
    inline bool operator()( ArgT const& arg ) const { return m_val>=arg; }
  };

} // namespace vw

#endif  // __VW_CORE_FUNCTORS_H__
