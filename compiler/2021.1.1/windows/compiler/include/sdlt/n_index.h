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

#ifndef SDLT_N_INDEX_H
#define SDLT_N_INDEX_H

#include <cstddef>
#include <utility>

#include "common.h"

#include "aligned.h"
#include "fixed.h"
#include "internal/const_bool.h"
#include "internal/logical_and_of.h"
#include "internal/static_loop.h"
#include "internal/tuple.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward
template<class... TypeListT>
class n_index_t;

namespace internal {

    template<class T>
    struct is_index_component_type : public std::false_type{};

    template<>
    struct is_index_component_type<int> : public std::true_type {};

    template<int NumberT>
    struct is_index_component_type<sdlt::fixed<NumberT>> :  public std::true_type {};

    template<int IndexAlignmentT>
    struct is_index_component_type<sdlt::aligned<IndexAlignmentT>> : public std::true_type {};


    template<bool DimensionIsOutOfRangeForRightT>
    struct rightmost_ops; /* undefined */

    template<>
    struct rightmost_ops<false /*DimensionIsOutOfRangeForRightT*/>
    {
        template<
               int DimensionT,
               typename LeftT, typename RightT>
           static decltype(std::declval<RightT>().template get<(DimensionT - (LeftT::rank - RightT::rank))>())
           overlay_d(const LeftT &, const RightT &a_right)
           {
               return a_right.template get<(DimensionT - (LeftT::rank - RightT::rank))>();
           }
    };

    template<>
    struct rightmost_ops<true /*DimensionIsOutOfRangeForRightT*/>
    {

        template<
             int DimensionT,
             typename LeftT, typename RightT>
         static  decltype(std::declval<LeftT>().template get<DimensionT>())
         overlay_d(const LeftT &a_left, const RightT &)
         {
             return a_left.template get<DimensionT>();
         }

    };

} // namespace internal




template<class... TypeListT>
class n_index_t
{
protected:
    typedef internal::tuple<TypeListT...> tuple_type;

    static_assert(internal::logical_and_of<internal::is_index_component_type<TypeListT>...>::value, "type of extent sizes is unsupported");
    tuple_type  m_index_values;

    template<class... OtherTypeListT>
    friend class n_index_t;

    template <int DimensionT, typename NDimensionalT>
    friend struct internal::type_of_d;

    explicit SDLT_INLINE
    n_index_t(const tuple_type &a_index_tuple)
    : m_index_values(a_index_tuple)
    {}

public:

    template<typename = internal::enable_if_type<std::is_default_constructible<tuple_type>::value>>
    SDLT_INLINE
    n_index_t()
    {}



    explicit SDLT_INLINE
    n_index_t(const TypeListT&... a_values)
    : m_index_values(a_values...)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    n_index_t(const n_index_t &a_other)
    : m_index_values(a_other.m_index_values)
    {}


    static constexpr int rank = internal::tuple_size<tuple_type>::value;
    static constexpr int row_dimension = rank-1;


    template<int DimensionT>
    SDLT_INLINE auto
    get() const
    -> typename internal::tuple_element<DimensionT, tuple_type>::type
    {
        static_assert(rank > DimensionT, "To access the index of dimension, DimensionT must be less than the rank");
        return m_index_values.template get<DimensionT>();
    }

protected:
    struct stream_out_functor
    {
    private:
        const tuple_type  & m_index_values;
        std::ostream& m_output_stream;
    public:
        SDLT_INLINE stream_out_functor(
            const tuple_type& a_index_values,
            std::ostream& a_output_stream)
        : m_index_values(a_index_values)
        , m_output_stream(a_output_stream)
        {}

        template<int IndexT>
        SDLT_INLINE
        void operator()()
        {
            m_output_stream << m_index_values.template get<IndexT>();
            if (IndexT < rank-1)
            {
                m_output_stream << ", ";
            }
        }
    };
public:
    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const n_index_t & a_index_list)
    {
        output_stream << "n_index_t{ ";

        stream_out_functor functor(a_index_list.m_index_values, output_stream);
        internal::static_for<0, rank>(functor);

        output_stream <<  " }";
        return output_stream;
    }


protected:
    template<typename ResultT, int ... IntegerListT>
    SDLT_INLINE
    ResultT
    negate (internal::int_sequence<IntegerListT...>) const
    {
        return ResultT((-this->template get<IntegerListT>())...);
    }

    template<typename ResultT, typename OtherT, int ... IntegerListT>
    SDLT_INLINE
    ResultT
    addition (const OtherT & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherT's TypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return ResultT((this->template get<IntegerListT>() + a_other.template get<IntegerListT>())...);
    }

    template<typename ResultT, typename OtherT, int ... IntegerListT>
    SDLT_INLINE
    ResultT
    subtraction (const OtherT & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherT's TypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return ResultT((this->template get<IntegerListT>() - a_other.template get<IntegerListT>())...);
    }

    template<class... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    equals (const n_index_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_and_of<true>((this->template get<IntegerListT>() == a_other.template get<IntegerListT>())...);
    }

    template<class... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    not_equals (const n_index_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_or_of<false>((this->template get<IntegerListT>() != a_other.template get<IntegerListT>())...);
    }


public:

    SDLT_INLINE
    n_index_t
    operator +() const
    {
        return *this;
    }

    SDLT_INLINE
    auto
    operator -() const
    -> n_index_t<decltype(-std::declval<TypeListT>())...>
    {

        typedef n_index_t<decltype(-std::declval<TypeListT>())...> result_type;

        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return negate<result_type>(index_sequence());
    }


    template<class... OtherTypeListT, typename = internal::enable_if_type<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    auto
    operator +(const n_index_t<OtherTypeListT...> & a_other) const
    -> n_index_t<decltype(std::declval<TypeListT>() + std::declval<OtherTypeListT>())...>
    {

        typedef n_index_t<decltype(std::declval<TypeListT>() + std::declval<OtherTypeListT>())...> result_type;

        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return addition<result_type, n_index_t<OtherTypeListT...>>(a_other, index_sequence());
    }


    template<class... OtherTypeListT, typename = internal::enable_if_type<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    auto
    operator -(const n_index_t<OtherTypeListT...> & a_other) const
    -> n_index_t<decltype(std::declval<TypeListT>() - std::declval<OtherTypeListT>())...>
    {

        typedef n_index_t<decltype(std::declval<TypeListT>() - std::declval<OtherTypeListT>())...> result_type;

        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return subtraction<result_type, n_index_t<OtherTypeListT...>>(a_other, index_sequence());
    }



    template<class... OtherTypeListT, typename = internal::enable_if_type<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    bool
    operator == (const n_index_t<OtherTypeListT...> a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return equals(a_other, index_sequence());
    }

    template<class... OtherTypeListT, typename = internal::enable_if_type<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    bool
    operator != (const n_index_t<OtherTypeListT...> a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return not_equals(a_other, index_sequence());
    }

protected:
    template<int... ReducedRankSequenceT>
    SDLT_INLINE auto
    sequence_dimensions(internal::int_sequence<ReducedRankSequenceT...>) const
    ->n_index_t<type_d<ReducedRankSequenceT, n_index_t>...>
    {
        return n_index_t<type_d<ReducedRankSequenceT, n_index_t>...>(this->template get<ReducedRankSequenceT>()...);
    }

public:

    // returns a n_index_t with the final ReduceRankT values from this n_index_t
    //
    // NOTE:  Despite enabling of the function for only valid rank values,
    // we still need a valid trailing return type, so need to use min_of
    // to keep ranks used in the return type in a valid range.
    template<int ReducedRankT, typename = internal::enable_if_type<(ReducedRankT <= rank) && (ReducedRankT>=0)> >
    SDLT_INLINE auto
    rightmost_dimensions() const
    -> decltype(this->template sequence_dimensions(internal::make_int_sequence<rank, rank - internal::min_int<rank, ReducedRankT>::value>()))
    {
        static_assert(ReducedRankT <= rank, "Can only reduce number of ranks, not increase them");
        static_assert(ReducedRankT >= 0, "Can not reduce number of ranks below 0");

        typedef internal::make_int_sequence<rank, rank - internal::min_int<rank, ReducedRankT>::value> reduced_index_sequence;
        return sequence_dimensions(reduced_index_sequence());
    }


protected:

    template<typename OtherT, int ... IntegerListT>
    SDLT_INLINE
    auto
    sequenced_overlay(const OtherT & a_other, internal::int_sequence<IntegerListT...>) const
    ->n_index_t<decltype(internal::rightmost_ops<(IntegerListT < (rank - OtherT::rank))>::template overlay_d<IntegerListT, n_index_t, OtherT>(std::declval<n_index_t>(), std::declval<OtherT>()))...>
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return n_index_t<decltype(internal::rightmost_ops<(IntegerListT < (rank - OtherT::rank))>::template overlay_d<IntegerListT, n_index_t, OtherT>(*this, a_other))...>
            (internal::rightmost_ops<(IntegerListT < (rank - OtherT::rank))>::template overlay_d<IntegerListT, n_index_t, OtherT>(*this, a_other)...);
    }


public:

    template<class... OtherTypeListT, typename = internal::enable_if_type<(sizeof...(OtherTypeListT) <= rank)> >
    SDLT_INLINE auto
    overlay_rightmost(const n_index_t<OtherTypeListT...> & a_other) const
    -> decltype(std::declval<n_index_t>().sequenced_overlay(std::declval<n_index_t<OtherTypeListT...>>(),
                                                           internal::make_int_sequence<sizeof...(TypeListT)>()))
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;
        return this->template sequenced_overlay(a_other, index_sequence());
    }

};


template<class... TypeListT>
class n_index_generator
: public n_index_t<TypeListT...>
{
public:
    typedef n_index_t<TypeListT...> value_type;
protected:
    typedef typename value_type::tuple_type tuple_type;

    template<class... OtherTypeListT>
    friend class n_index_generator;

    explicit SDLT_INLINE
    n_index_generator(const tuple_type &a_values)
    : value_type(a_values)
    {}

public:

    // Only provide a default constructor when there are 0 indices
    template<typename = std::enable_if<sizeof...(TypeListT) == 0>>
    SDLT_INLINE
    n_index_generator()
    : value_type()
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    n_index_generator(const n_index_generator &a_other)
    : value_type(a_other)
    {}

    SDLT_INLINE n_index_generator<TypeListT..., int>
    operator [] (int a_size) const
    {
        return n_index_generator<TypeListT..., int>(value_type::m_index_values.concatenate(a_size));
    }

    template<int NumberT>
    SDLT_INLINE n_index_generator<TypeListT..., fixed<NumberT>>
    operator [] (fixed<NumberT> a_size) const
    {
        return n_index_generator<TypeListT..., fixed<NumberT>>(value_type::m_index_values.concatenate(a_size));
    }

    template<int AlignmentT>
    SDLT_INLINE n_index_generator<TypeListT..., aligned<AlignmentT>>
    operator [] (const aligned<AlignmentT> & a_size) const
    {
        return n_index_generator<TypeListT..., aligned<AlignmentT>>(value_type::m_index_values.concatenate(a_size));
    }

    SDLT_INLINE value_type value() const
    {
        return *this;
    }

};

namespace {
    n_index_generator<> n_index;
}


namespace internal {

    template<typename ObjT>
    struct index_for_d;

    template<class... TypeListT>
    struct index_for_d<n_index_t<TypeListT...>>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const n_index_t<TypeListT...> &a_indices)
        ->decltype(a_indices.template get<DimensionT>())
        {
            return a_indices.template get<DimensionT>();
        }
    };

    template<class... TypeListT>
    struct index_for_d<n_index_generator<TypeListT...>>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const n_index_t<TypeListT...> &a_indices)
        ->decltype(a_indices.template get<DimensionT>())
        {
            return a_indices.template get<DimensionT>();
        }
    };

    template <int DimensionT, typename... TypeListT>
    struct type_of_d<DimensionT, n_index_t<TypeListT...>>
    {
        typedef typename tuple_element<DimensionT, typename n_index_t<TypeListT...>::tuple_type>::type type;
    };

} // namespace internal



template<int DimensionT, typename ObjT>
SDLT_INLINE auto
index_d(const ObjT &a_obj)
-> decltype(internal::index_for_d<ObjT>::template get<DimensionT>(a_obj))
{
    return internal::index_for_d<ObjT>::template get<DimensionT>(a_obj);
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::n_index_t;
using __SDLT_IMPL::n_index;
using __SDLT_IMPL::index_d;
} // namespace sdlt

#endif // SDLT_N_INDEX_H
