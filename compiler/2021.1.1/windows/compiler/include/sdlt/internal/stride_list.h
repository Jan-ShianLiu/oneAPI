//
// Copyright (C) 2012-2016 Intel Corporation. All Rights Reserved.
//
// The source code contained or described herein and all documents related
// to the source code ("Material") are owned by Intel Corporation or its
// suppliers or licensors.  Title to the Material remains with Intel
// Corporation or its suppliers and licensors.  The Material is protected
// by worldwide copyright laws and treaty provisions.  No part of the
// Material may be used, copied, reproduced, modified, published, uploaded,
// posted, transmitted, distributed, or disclosed in any way without
// Intel's prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual
// property right is granted to or conferred upon you by disclosure or
// delivery of the Materials, either expressly, by implication, inducement,
// estoppel or otherwise. Any license under such intellectual property rights
// must be express and approved by Intel in writing.
//
// Unless otherwise agreed by Intel in writing, you may not remove or alter
// this notice or any other notice embedded in Materials by Intel or Intel's
// suppliers or licensors in any way.
//

#ifndef SDLT_STRIDE_LIST_H
#define SDLT_STRIDE_LIST_H

#include <cstddef>
#include <iostream>
#include <type_traits>


#include "../common.h"
#include "../fixed.h"
#include "aligned_stride.h"
#include "deferred.h"
#include "enable_if_type.h"
#include "fixed_stride.h"
#include "static_loop.h"
#include "tuple.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

// Problem:
//     Because the "value" could be passed to a function/constructor that
//     accepts a const reference, we must be prepared for the address of
//     the value to be taken.  This means we need to declare the member
//     value outside the class definition.  This can then lead to multiple
//     compilation units declaring instances of the static value variable.
//     If the value is declared as part of a template, the language will
//     take care of combining those instances and only keeping one.
//     However the after specialization for std::ptrdiff_t, the class has
//     no other templated dependencies and technically no longer a template
//     causing multiple instances of "value" to exist.
// Solution:
//     We need all specializations to still be dependent on a template
//     argument.  So we will add an extra dummy DependentT argument so
//     our specializations will always have at least one template argument
//     left.  As a default template argument is specified, it should be
//     transparent to users.
template<typename T, typename DependentT=dependent_arg>
struct default_stride; /* undefined */

template<typename DependentT>
struct default_stride<std::ptrdiff_t, DependentT>
{
  //  static constexpr fixed_stride<0> value=fixed_stride<0>();
    static constexpr sdlt::__SDLT_IMPL::internal::fixed_stride<0> value={};
};

template<typename DependentT>
constexpr sdlt::__SDLT_IMPL::internal::fixed_stride<0> default_stride<std::ptrdiff_t, DependentT>::value;


template<std::ptrdiff_t FixedT, typename DependentT>
struct default_stride<fixed_stride<FixedT>, DependentT>
{
    //static constexpr fixed_stride<FixedT> value=fixed_stride<FixedT>();
    static constexpr sdlt::__SDLT_IMPL::internal::fixed_stride<FixedT> value={};
};

template<std::ptrdiff_t FixedT, typename DependentT>
constexpr sdlt::__SDLT_IMPL::internal::fixed_stride<FixedT> default_stride<fixed_stride<FixedT>, DependentT>::value;

template<int AlignmentT, typename DependentT>
struct default_stride<aligned_stride<AlignmentT>, DependentT>
{
    static const sdlt::__SDLT_IMPL::internal::aligned_stride<AlignmentT> value;
};

template<int AlignmentT, typename DependentT>
const sdlt::__SDLT_IMPL::internal::aligned_stride<AlignmentT> default_stride<aligned_stride<AlignmentT>, DependentT>::value(0);



template<class... TypeListT>
class stride_list
{
public:
    typedef tuple<TypeListT...> strides_tuple;
    static_assert(tuple_size<strides_tuple>::value > 0, "Must have at least 1 stride");
private:
    strides_tuple  m_stride_values;
public:

    stride_list()
    : m_stride_values(default_stride<TypeListT>::value...)
    {
    }

    explicit stride_list(const TypeListT&... a_values)
        : m_stride_values(a_values...)
    {}

    explicit stride_list(const strides_tuple &a_stride_values)
    : m_stride_values(a_stride_values)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    stride_list(const stride_list &a_other)
    : m_stride_values(a_other.m_stride_values)
    {
    }

    static constexpr int rank = tuple_size<strides_tuple>::value;
    static constexpr int row = rank-1;


    template<int ElementIndexT>
    auto get() const
    -> decltype(this->m_stride_values.template get<ElementIndexT>())
    {
        return m_stride_values.template get<ElementIndexT>();
    }

    auto row_stride() const
        -> decltype(this->m_stride_values.template get<row>())
    {
        return m_stride_values.template get<row>();
    }

    template<
        int ElementIndexT,
        typename = enable_if_type<std::is_same<std::ptrdiff_t, typename tuple_element<ElementIndexT, strides_tuple>::type>::value>
    >
    void set(std::ptrdiff_t a_stride)
    {
        static_assert(ElementIndexT >=0 && ElementIndexT < rank, "ElementIndexT is out of bounds");
        m_stride_values.template set<ElementIndexT>(a_stride);
    }

    template<
        int ElementIndexT,
        typename T = enable_if_type<is_fixed_stride<typename tuple_element<ElementIndexT, strides_tuple>::type>::value>
    >
    void set(std::ptrdiff_t __SDLT_DEBUG_ONLY(a_stride), T = T())
    {
        __SDLT_ASSERT(get<ElementIndexT>() == a_stride);
    }

    template<
        int ElementIndexT,
        typename T = enable_if_type<is_aligned_stride<typename tuple_element<ElementIndexT, strides_tuple>::type>::value>
    >
    void set(std::ptrdiff_t a_stride, T = T(), T = T())
    {
        m_stride_values.template set<ElementIndexT>(typename tuple_element<ElementIndexT, strides_tuple>::type(a_stride));
    }


#ifdef __SDLT_RESERVED_FOR_FUTURE_USE
    template<int IndexT, int AlignmentT, typename = enable_if_type<std::is_same<std::ptrdiff_t, typename tuple_element<IndexT, strides_tuple>::type>::value>>
    void
    assume_aligned() const
    {
        m_stride_values.template assume_aligned<IndexT, AlignmentT>();
    }

    template<int IndexT, int AlignmentT, typename T= enable_if_type<is_fixed_stride<typename tuple_element<IndexT, strides_tuple>::type>::value>>
    void
    assume_aligned(T = T()) const
    {
    }
#endif

protected:
    struct stream_out_functor
    {
    private:
        const strides_tuple  & m_stride_values;
        std::ostream& m_output_stream;
    public:
        stream_out_functor(
            const strides_tuple& a_extent_values,
            std::ostream& a_output_stream)
        : m_stride_values(a_extent_values)
        , m_output_stream(a_output_stream)
        {}

        template<int IndexT>
        void operator()()
        {
            auto value = m_stride_values.template get<IndexT>();
            m_output_stream << value;
            if (IndexT < rank-1)
            {
                m_output_stream << ", ";
            }
        }
    };
public:
    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const stride_list & a_stride_list)
    {
        output_stream << "stride_list{ ";

        stream_out_functor functor(a_stride_list.m_stride_values, output_stream);
        static_for<0, rank>(functor);

        output_stream <<  " }";
        return output_stream;
    }

};


} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_STRIDE_LIST_H
