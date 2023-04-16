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

#ifndef SDLT_N_BOUNDS_H
#define SDLT_N_BOUNDS_H

#include <type_traits>

#include "common.h"
#include "bounds.h"
#include "internal/is_size_provider.h"
#include "internal/static_loop.h"
#include "internal/tuple.h"
#include "n_extent.h"
#include "n_index.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward
template<class... TypeListT>
class n_bounds_t;


namespace internal {

    template<class T>
    struct is_bounds_type : public std::false_type{};

    template<typename LeftT, typename RightT>
    struct is_bounds_type<bounds_t<LeftT, RightT>> : public std::true_type {};


    template<typename T>
    class is_n_bounds_provider
    {
        static_assert(sizeof(long) > sizeof(char), "Target platform must have sizeof long > char");
        template<typename CheckT> static long check(decltype(&CheckT::n_bounds));
        template<typename CheckT> static char check(...);
    public:
        static const bool value = sizeof(check<T>(nullptr)) > sizeof(char);
    };

    template<typename T>
    class is_n_bounds_interface
    {
        static_assert(sizeof(long) > sizeof(char), "Target platform must have sizeof long > char");
        template<typename CheckT> static long check(decltype(&CheckT::template bounds_d<0>));
        template<typename CheckT> static char check(...);
    public:
        static const bool value = sizeof(check<T>(nullptr)) > sizeof(char);
    };

    template<class IndicesT, class ExtentsT>
    struct bounds_builder;
    // undefined

    template<class... IndexTypeListT, class... ExtentTypeListT>
    struct bounds_builder<n_index_t<IndexTypeListT...>, n_extent_t<ExtentTypeListT...>>
    {
        typedef n_extent_t<ExtentTypeListT...> extents_type;
        typedef n_index_t<IndexTypeListT...> indices_type;

        static_assert(extents_type::rank == indices_type::rank, "n_extent_t and n_index_t must have the same rank to build a n_bounds_t");

        typedef n_bounds_t<bounds_t<IndexTypeListT, decltype(std::declval<IndexTypeListT>() + std::declval<ExtentTypeListT>())>...> result_type;

    protected:
        const indices_type m_indices;
        const extents_type m_extents;
    public:

        SDLT_INLINE bounds_builder(const indices_type & a_indices, const extents_type &a_extents)
        : m_indices(a_indices)
        , m_extents(a_extents)
        {}

        template<int IndexT>
        SDLT_INLINE auto
        get() const
        ->bounds_t<decltype(std::declval<indices_type>().template get<IndexT>()),
                   decltype(std::declval<indices_type>().template get<IndexT>() +
                            std::declval<extents_type>().template get<IndexT>())>
        {
            return bounds(this->m_indices.template get<IndexT>(), (this->m_indices.template get<IndexT>() + this->m_extents.template get<IndexT>()));
        }
    };


    template<class... TypeListT>
    struct lower_bounds_builder
    {
        typedef n_index_t<typename TypeListT::lower_type...> type;

        template<int ... IntegerListT>
        static SDLT_INLINE type
        build(const internal::tuple<TypeListT...> &a_boundaries, int_sequence<IntegerListT...>)
        {
            return type(a_boundaries.template get<IntegerListT>().lower()...);
        }
    };

    template<class... TypeListT>
    struct upper_bounds_builder
    {
        typedef n_index_t<typename TypeListT::upper_type...> type;

        template<int ... IntegerListT>
        static SDLT_INLINE type
        build(const internal::tuple<TypeListT...> &a_boundaries, int_sequence<IntegerListT...>)
        {
            return type(a_boundaries.template get<IntegerListT>().upper()...);
        }
    };

} // namespace internal




template<class... TypeListT>
class n_bounds_t
{
protected:
    typedef internal::tuple<TypeListT...> tuple_type;

    static_assert(internal::logical_and_of<internal::is_bounds_type<TypeListT>...>::value, "Invalid type specified as bounds");
    tuple_type  m_bounds_values;

    template<typename...>
    friend class n_bounds_generator;

    template <int DimensionT, typename NDimensionalT>
    friend struct internal::type_of_d;

    explicit SDLT_INLINE
    n_bounds_t(const tuple_type &a_bounds_tuple)
    : m_bounds_values(a_bounds_tuple)
    {}

public:

    template<typename = std::enable_if<std::is_default_constructible<tuple_type>::value>>
    SDLT_INLINE
    n_bounds_t()
    {}

    explicit SDLT_INLINE
    n_bounds_t(const TypeListT&... a_values)
    : m_bounds_values(a_values...)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    n_bounds_t(const n_bounds_t &a_other)
    : m_bounds_values(a_other.m_bounds_values)
    {}


    static constexpr int rank = internal::tuple_size<tuple_type>::value;
    static constexpr int row_dimension = rank-1;

    template<int DimensionT>
    SDLT_INLINE auto
    get() const
    -> typename internal::tuple_element<DimensionT, tuple_type>::type
    {
        static_assert(rank > DimensionT, "To access the bounds of dimension, DimensionT must be less than the rank");
        return m_bounds_values.template get<DimensionT>();
    }

    // returns n_index_t representing with the lower value of each dimension's bounds
    // in 2d we might call this the top left of the area
    typedef typename internal::lower_bounds_builder<TypeListT...>::type lower_type;
    SDLT_INLINE lower_type
    lower() const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return internal::lower_bounds_builder<TypeListT...>::template build(m_bounds_values, index_sequence());
    }

    // returns n_index_t representing with the upper value of each dimension's bounds
    // in 2d we might call this the bottom right of the area
    typedef typename internal::upper_bounds_builder<TypeListT...>::type upper_type;
    SDLT_INLINE upper_type
    upper() const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return internal::upper_bounds_builder<TypeListT...>::template build(m_bounds_values, index_sequence());
    }


protected:
    template<typename... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    calc_n_contains (const n_bounds_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_and_of<true>((this->template get<IntegerListT>().contains(a_other.template get<IntegerListT>()))...);
    }

public:

    template<typename... OtherTypeListT, typename = std::enable_if<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE bool contains(const n_bounds_t<OtherTypeListT...> &a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return calc_n_contains(a_other, index_sequence());
    }

#ifdef __SDLT_RESERVED_FOR_FUTURE_USE
protected:

    template<typename TrueValT, typename FalseValT>
    SDLT_INLINE TrueValT
    ternary_value(std::true_type, const TrueValT a_true_value, FalseValT)
    {
        return a_true_value;
    }

    template<typename TrueValT, typename FalseValT>
    SDLT_INLINE FalseValT
    ternary_value(std::false_type, TrueValT, FalseValT a_false_value)
    {
        return a_false_value;
    }

    template<int DimensionT, typename BoundsT, int ... IntegerListT>
    SDLT_INLINE
    n_bounds_t<typename std::conditional<IntegerListT == DimensionT, BoundsT, TypeListT>::type... >
    replace_dimension(const BoundsT & a_bounds, internal::int_sequence<IntegerListT...>) const
    {
        return n_bounds_t<typename std::conditional<IntegerListT == DimensionT, BoundsT, TypeListT>::type... >
            (
                // GCC 4.8.2 did not want to expand the ternary expression as a parameter pack
                //(IntegerListT == DimensionT) ? a_bounds : this->template get<IntegerListT>())...

                // Workaround is to provide our own ternary function sets that return different values
                // based on the 1st parameter
                ternary_value(internal::const_bool<IntegerListT == DimensionT>(), a_bounds, this->template get<IntegerListT>())...
            );
    }

public:

    template<int DimensionT, typename BoundsT>
    SDLT_INLINE auto
    replaced(const BoundsT & a_bounds) const
    -> decltype(replace_dimension<DimensionT, BoundsT>(a_bounds, internal::make_int_sequence<sizeof...(TypeListT)>()))
    {
        static_assert(internal::is_bounds_type<BoundsT>::value, "Invalid type specified as bounds");
        static_assert(rank > DimensionT, "To access the bounds of dimension, DimensionT must be less than the rank");
        __SDLT_ASSERT(this->template get<DimensionT>().contains(a_bounds));

        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return replace_dimension<DimensionT, BoundsT>(a_bounds, index_sequence());
    }

#endif

protected:

    template<class... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    equals (const n_bounds_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_and_of<true>((this->template get<IntegerListT>() == a_other.template get<IntegerListT>())...);
    }

    template<class... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    not_equals (const n_bounds_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_or_of<false>((this->template get<IntegerListT>() != a_other.template get<IntegerListT>())...);
    }

    template<typename ResultT, typename OtherT, int ... IntegerListT>
    SDLT_INLINE
    ResultT
    addition (const OtherT & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return ResultT((this->template get<IntegerListT>() + a_other.template get<IntegerListT>())...);
    }

public:

    template<class... OtherTypeListT, typename = std::enable_if<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    bool
    operator == (const n_bounds_t<OtherTypeListT...> a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return equals(a_other, index_sequence());
    }

    template<class... OtherTypeListT, typename = std::enable_if<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    bool
    operator != (const n_bounds_t<OtherTypeListT...> a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return not_equals(a_other, index_sequence());
    }

    template<class... OtherTypeListT>
    SDLT_INLINE
    auto
    operator +(const n_index_t<OtherTypeListT...> & a_other) const
    -> n_bounds_t<decltype(std::declval<TypeListT>() + std::declval<OtherTypeListT>())...>
    {

        typedef n_bounds_t<decltype(std::declval<TypeListT>() + std::declval<OtherTypeListT>())...> result_type;

        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return addition<result_type, n_index_t<OtherTypeListT...>>(a_other, index_sequence());
    }

protected:
    template<int... ReducedRankSequenceT>
    SDLT_INLINE auto
    sequence_reduced_rank(internal::int_sequence<ReducedRankSequenceT...>) const
    ->n_bounds_t<type_d<ReducedRankSequenceT, n_bounds_t>...>
    {
        return n_bounds_t<type_d<ReducedRankSequenceT, n_bounds_t>...>(this->template get<ReducedRankSequenceT>()...);
    }

public:

    // returns a n_bounds with the final ReduceRankT values from this n_bounds
    //
    // NOTE:  Despite enabling of the function for only valid rank values,
    // we still need a valid trailing return type, so need to use min_of
    // to keep ranks used in the return type in a valid range.
    //template<int ReducedRankT, typename = internal::enable_if_type<(ReducedRankT <= rank) && (ReducedRankT>=1)> >
    template<int ReducedRankT, typename = internal::enable_if_type<(ReducedRankT <= rank) && (ReducedRankT>=0)> >
    SDLT_INLINE auto
    rightmost_dimensions() const
    -> decltype(this->template sequence_reduced_rank(internal::make_int_sequence<rank, rank - internal::min_int<rank, ReducedRankT>::value>()))
    {
        static_assert(ReducedRankT <= rank, "Can only reduce number of ranks, not increase them");
        static_assert(ReducedRankT >= 0, "Can not reduce number of ranks below 0");

        typedef internal::make_int_sequence<rank, rank - internal::min_int<rank, ReducedRankT>::value> reduced_index_sequence;
        return sequence_reduced_rank(reduced_index_sequence());
    }

protected:

    template<typename OtherT, int ... IntegerListT>
    SDLT_INLINE
    auto
    sequenced_overlay(const OtherT & a_other, internal::int_sequence<IntegerListT...>) const
    ->n_bounds_t<decltype(internal::rightmost_ops<(IntegerListT < (rank - OtherT::rank))>::template overlay_d<IntegerListT, n_bounds_t, OtherT>(std::declval<n_bounds_t>(), std::declval<OtherT>()))...>
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return n_bounds_t<decltype(internal::rightmost_ops<(IntegerListT < (rank - OtherT::rank))>::template overlay_d<IntegerListT, n_bounds_t, OtherT>(*this, a_other))...>
            (internal::rightmost_ops<(IntegerListT < (rank - OtherT::rank))>::template overlay_d<IntegerListT, n_bounds_t, OtherT>(*this, a_other)...);
    }


public:

    template<class... OtherTypeListT, typename = internal::enable_if_type<(sizeof...(OtherTypeListT) <= rank)> >
    SDLT_INLINE auto
    overlay_rightmost(const n_bounds_t<OtherTypeListT...> & a_other) const
    -> decltype(std::declval<n_bounds_t>().sequenced_overlay(std::declval<n_bounds_t<OtherTypeListT...>>(),
                                                           internal::make_int_sequence<sizeof...(TypeListT)>()))
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;
        return this->template sequenced_overlay(a_other, index_sequence());
    }

protected:
    struct stream_out_functor
    {
    private:
        const tuple_type  & m_bounds_values;
        std::ostream& m_output_stream;
    public:
        stream_out_functor(
            const tuple_type& a_bound_values,
            std::ostream& a_output_stream)
        : m_bounds_values(a_bound_values)
        , m_output_stream(a_output_stream)
        {}

        template<int DimensionT>
        void operator()()
        {
            m_output_stream << m_bounds_values.template get<DimensionT>();
            if (DimensionT < rank-1)
            {
                m_output_stream << ", ";
            }
        }
    };

public:
    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const n_bounds_t & a_bounds_list)
    {
        output_stream << "n_bounds_t{ ";

        stream_out_functor functor(a_bounds_list.m_bounds_values, output_stream);
        internal::static_for<0, rank>(functor);

        output_stream <<  " }";
        return output_stream;
    }
};


template<class... TypeListT>
class n_bounds_generator
: public n_bounds_t<TypeListT...>
{
public:
    typedef n_bounds_t<TypeListT...> value_type;
protected:
    typedef internal::tuple<TypeListT...> tuple_type;


    template<class... OtherTypeListT>
    friend class n_bounds_generator;

    explicit n_bounds_generator(const tuple_type &a_values)
    : value_type(a_values)
    {}

public:
    // Only provide a default constructor when there are 0 bounds
    template<typename = std::enable_if<sizeof...(TypeListT) == 0>>
    n_bounds_generator()
    : value_type()
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    n_bounds_generator(const n_bounds_generator &a_other)
    : value_type(a_other)
    {}

    template<typename LeftT, typename RightT>
    n_bounds_generator<TypeListT..., bounds_t<LeftT, RightT>>
    operator [] (const bounds_t<LeftT, RightT> & a_bounds) const
    {
        return n_bounds_generator<TypeListT..., bounds_t<LeftT, RightT>>(value_type::m_bounds_values.concatenate(a_bounds));
    }

    template<class... IndexTypeListT, class... ExtentTypeListT>
    typename internal::bounds_builder<n_index_t<IndexTypeListT...>, n_extent_t<ExtentTypeListT...>>::result_type
    operator () (const n_index_t<IndexTypeListT...> & a_indices, const n_extent_t<ExtentTypeListT...> & a_extents) const
    {
        typedef internal::bounds_builder<n_index_t<IndexTypeListT...>, n_extent_t<ExtentTypeListT...>> op_type;

        op_type op(a_indices, a_extents);

        // Let the tuple constructor get the values from op index by index
        // Since the tuple already has an index list, no need to repeat work
        // our operation will provide results when get<IndextT> is called
        return typename op_type::result_type(typename op_type::result_type::tuple_type(op));
    }

    value_type value() const
    {
        return *this;
    }
};

namespace {
    n_bounds_generator<> n_bounds;
}


namespace internal {

    template<bool IsBoundsInterfaceT, bool IsBoundsProviderT, bool IsSizeProviderT, bool IsArrayT, typename ObjT>
    struct bounds_for_d;

    template<class... TypeListT>
    struct bounds_for_d<false /*IsBoundsInterfaceT*/, false /*IsBoundsProviderT*/, false /*IsSizeProviderT*/, false /*IsArrayT*/, n_bounds_t<TypeListT...>>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const n_bounds_t<TypeListT...> &a_boundaries)
        ->decltype(a_boundaries.template get<DimensionT>())
        {
            return a_boundaries.template get<DimensionT>();
        }
    };

    template<class... TypeListT>
    struct bounds_for_d<false /*IsBoundsInterfaceT*/, false /*IsBoundsProviderT*/, false/*IsSizeProviderT*/, false /*IsArrayT*/, n_bounds_generator<TypeListT...>>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const n_bounds_t<TypeListT...> &a_boundaries)
        ->decltype(a_boundaries.template get<DimensionT>())
        {
            return a_boundaries.template get<DimensionT>();
        }
    };

    // Specialize for C arrays
    template<typename T>
    struct bounds_for_d<false /*IsBoundsInterfaceT*/, false /*IsBoundsProviderT*/, false /*IsSizeProviderT*/, true /*IsArrayT*/, T>
    {
        //typedef internal::c_array_type<T, DT...> type;

        template<int DimensionT>
        static constexpr SDLT_INLINE auto
        get(const T &)
        ->bounds_t<fixed<0>, fixed<std::extent<T, DimensionT>::value>>
        {
            return bounds_t<fixed<0>, fixed<std::extent<T, DimensionT>::value>>();
        }
    };

    // Specialize for types with a size_type size() const method
    template<typename T>
    struct bounds_for_d<false /*IsBoundsInterfaceT*/, false/*IsBoundsProviderT*/, true/*IsSizeProviderT*/, false /*IsArrayT*/, T>
    {
        template<int DimensionT>
        static SDLT_INLINE bounds_t<fixed<0>, int>
        get(const T & a_container)
        {
            static_assert(DimensionT == 0, "This container type supports only dimension index 0");
            __SDLT_ASSERT(a_container.size() < static_cast<size_t>(std::numeric_limits<int>::max()));
            return bounds_t<fixed<0>, int>(0_fixed, static_cast<int>(a_container.size()));
        }
    };


    template<bool IsBoundsProviderT, bool IsSizeProviderT, typename ObjT>
    struct bounds_for_d<true /*IsBoundsInterfaceT*/, IsBoundsProviderT, IsSizeProviderT, false /*IsArrayT*/, ObjT>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
            get(const ObjT & a_obj)
            ->decltype(a_obj.template bounds_d<DimensionT>())
        {
            return a_obj.template bounds_d<DimensionT>();
        }
    };
} // namespace internal

template<int DimensionT, typename ObjT>
SDLT_INLINE auto
bounds_d(const ObjT &a_obj)
-> decltype(internal::bounds_for_d<internal::is_n_bounds_interface<ObjT>::value,
                         internal::is_n_bounds_provider<ObjT>::value,
                         internal::is_size_provider<ObjT>::value,
                         std::is_array<ObjT>::value,
                         ObjT>::template get<DimensionT>(a_obj))
{
    return internal::bounds_for_d<internal::is_n_bounds_interface<ObjT>::value,
                        internal::is_n_bounds_provider<ObjT>::value,
                        internal::is_size_provider<ObjT>::value,
                        std::is_array<ObjT>::value,
                        ObjT>::template get<DimensionT>(a_obj);
}

namespace internal {

    template<typename ObjT>
    struct bounds_for_d<false /*IsBoundsInterfaceT*/, true/*IsBoundsProviderT*/, false/*IsSizeProviderT*/, false /*IsArrayT*/, ObjT>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const ObjT & a_obj)
        ->decltype(bounds_d<DimensionT>(a_obj.n_bounds()))
        {
            return bounds_d<DimensionT>(a_obj.n_bounds());
        }
    };

    template <int DimensionT, typename... TypeListT>
    struct type_of_d<DimensionT, n_bounds_t<TypeListT...>>
    {
        typedef typename tuple_element<DimensionT, typename n_bounds_t<TypeListT...>::tuple_type>::type type;
    };


} // namespace internal

} // namepace __SDLT_IMPL
using __SDLT_IMPL::n_bounds_t;
using __SDLT_IMPL::n_bounds;
using __SDLT_IMPL::bounds_d;

} // namespace sdlt

#endif // SDLT_N_BOUNDS_H
