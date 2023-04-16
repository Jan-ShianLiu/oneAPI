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

#ifndef SDLT_INDEX_LIST_H
#define SDLT_INDEX_LIST_H

#include <algorithm>
#include <iterator>
#include <limits>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "../common.h"

#include "../aligned.h"
#include "../fixed.h"
#include "../linear_index.h"
#include "enable_if_type.h"
#include "logical_and_of.h"
#include "simd_index_ref.h"
#include "static_loop.h"
#include "tuple.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

#if 0
template<class T>
struct is_index_type : public std::false_type{};

template<>
struct is_index_type<int> : public std::true_type {};

template<>
struct is_index_type<sdlt::linear_index> : public std::true_type {};

template<>
struct is_index_type<sdlt::__SDLT_IMPL::internal::simd_index_ref> : public std::true_type {};

template<int ValueT>
struct is_index_type<sdlt::fixed<ValueT>> : public std::true_type {};

template<int IndexAlignmentT>
struct is_index_type<sdlt::aligned<IndexAlignmentT>> : public std::true_type {};


template<typename LoopIndexT, typename OperationT>
struct is_index_type<__SDLT_IMPL::internal::compound_index<LoopIndexT, OperationT>> : public std::true_type {};
#endif


template<class... TypesT>
class index_list;

template<class... TypesT>
class index_list
{
public:
    typedef __SDLT_IMPL::internal::tuple<TypesT...> index_tuple;
    //static_assert(internal::logical_and_of<internal::is_index_type<TypesT>...>::value, "Invalid index type used");

private:
    index_tuple  m_index_values;
public:


    template<typename IndexTypeT>
    struct append_index
    {
        typedef index_list<TypesT..., IndexTypeT> type;
    };

    // Only provide a default constructor when there are 0 indices
    template<typename = std::enable_if<sizeof...(TypesT) == 0>>
    index_list()
    {}

    index_list(const index_tuple &a_index_values2)
    : m_index_values(a_index_values2)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    index_list(const index_list &a_other)
    : m_index_values(a_other.m_index_values)
    {}


    template<typename IndexTypeT/*, typename = __SDLT_IMPL::internal::enable_if_type<internal::is_index_type<IndexTypeT>::value>*/>
    index_list<TypesT..., IndexTypeT> operator + (const IndexTypeT & a_index) const
    {
        return index_list<TypesT..., IndexTypeT>(m_index_values.concatenate(a_index));
    }

    template<typename... IndexListT/*, typename = __SDLT_IMPL::internal::enable_if_type<internal::is_index_type<IndexTypeT>::value>*/>
    index_list<TypesT..., IndexListT...> append(const IndexListT &... a_indices) const
    {
        return index_list<TypesT..., IndexListT...>(m_index_values.concatenate(a_indices...));
    }


    static constexpr int rank = __SDLT_IMPL::internal::tuple_size<index_tuple>::value;
    static constexpr int row_element = rank-1;

    template<int ElementIndexT, typename = __SDLT_IMPL::internal::enable_if_type<(ElementIndexT < rank)>>
    auto
    get() const
    -> typename __SDLT_IMPL::internal::tuple_element<ElementIndexT, index_tuple>::type
    {
        return m_index_values.template get<ElementIndexT>();
    }

protected:
    struct stream_out_functor
    {
    private:
        const index_tuple  & m_index_values;
        std::ostream& m_output_stream;
    public:
        stream_out_functor(
            const index_tuple& a_index_values,
            std::ostream& a_output_stream)
        : m_index_values(a_index_values)
        , m_output_stream(a_output_stream)
        {}

        template<int ElementIndexT>
        void operator()()
        {
            m_output_stream << m_index_values.template get<ElementIndexT>();
            if (ElementIndexT < rank-1)
            {
                m_output_stream << ", ";
            }
        }
    };
public:
    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const index_list & a_index_list)
    {
        output_stream << "index_list{ ";

        stream_out_functor functor(a_index_list.m_index_values, output_stream);
        static_for<0, rank>(functor);

        output_stream <<  " }";
        return output_stream;
    }

};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_INDEX_LIST_H
