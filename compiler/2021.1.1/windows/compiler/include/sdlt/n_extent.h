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

#ifndef SDLT_EXTENTS_H
#define SDLT_EXTENTS_H

#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include "common.h"

#include "aligned.h"
#include "fixed.h"
#include "internal/const_bool.h"
#include "internal/index_list.h"
#include "internal/is_size_provider.h"
#include "internal/logical_and_of.h"
#include "internal/tuple.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward
template<class... TypeListT>
class n_extent_t;

namespace internal {

    template<class T>
    struct is_extent_type : public std::false_type{};

    template<>
    struct is_extent_type<int> : public std::true_type {};

    template<int OffsetT>
    struct is_extent_type<sdlt::fixed<OffsetT>> : public const_bool<(OffsetT >= 0)> {};

    template<int IndexAlignmentT>
    struct is_extent_type<sdlt::aligned<IndexAlignmentT>> : public std::true_type {};


    template<typename T>
    class is_n_extent_provider
    {
        static_assert(sizeof(long) > sizeof(char), "Target platform must have sizeof long > char");
        template<typename CheckT> static long check(decltype(&CheckT::n_extent));
        template<typename CheckT> static char check(...);
    public:
        static const bool value = sizeof(check<T>(nullptr)) > sizeof(char);
    };

    template<typename T>
    class is_n_extent_interface
    {
        static_assert(sizeof(long) > sizeof(char), "Target platform must have sizeof long > char");

        template<typename CheckT> static long check(decltype(&CheckT::template extent_d<0>));
        template<typename CheckT> static char check(...);
    public:
        static const bool value = sizeof(check<T>(nullptr)) > sizeof(char);
    };

    namespace soaNd {
        template<int MemberSizeT, int ComponentT, int RowComponentT, typename ExtentsT>
        struct stride_component;

    } // namespace soaNd


    template<typename T, int... DimensionListT>
    struct c_array_type_builder;

    template<typename T, int DimensionT>
    struct c_array_type_builder<T, DimensionT>
    {
        typedef T type[DimensionT];
    };

    template<typename T, int HeadDimensionT, int... RemainingDimensionsT>
    struct c_array_type_builder<T, HeadDimensionT, RemainingDimensionsT...>
    {
        typedef typename c_array_type_builder<T, RemainingDimensionsT...>::type type[HeadDimensionT];

    };

    template<typename T, int... DT>
    using c_array_type=typename c_array_type_builder<T, DT...>::type;

    template<bool IsExtentInterface, bool IsExtentProvider, bool IsSizeCompatible, bool IsCArray, typename ObjT>
    struct extent_for_d;


    template<typename ExtentsT>
    SDLT_INLINE size_t
    padded_size(const ExtentsT &a_extents, int a_row_stride)
    {
        __SDLT_ASSERT(a_row_stride >= a_extents.template get<ExtentsT::row_dimension>());

        typename ExtentsT::hyper_volume_calculator calculator(a_extents.m_extent_values, a_row_stride);
        static_for<0, ExtentsT::rank-1>(calculator);
        return calculator.hyper_volume_size;
    }

} // namespace internal




template<class... TypeListT>
class n_extent_t
{
protected:
    template<int MemberSizeT, int ComponentT, int RowComponentT, typename ExtentsT>
    friend struct internal::soaNd::stride_component;

    template<bool IsExtentInterface, bool IsExtentProvider, bool IsSizeCompatible, bool IsCArray, typename ObjT>
    friend struct internal::extent_for_d;

    template<typename T>
    friend size_t internal::padded_size(const T &, int a_row_stride);


    typedef internal::tuple<TypeListT...> tuple_type;

    static_assert(internal::logical_and_of<internal::is_extent_type<TypeListT>...>::value, "Extent sizes can only be 'int' or 'sdlt::fixed<int NumberT>()'");
    tuple_type  m_extent_values;

    template <int DimensionT, typename NDimensionalT>
    friend struct internal::type_of_d;

    explicit SDLT_INLINE
    n_extent_t(const tuple_type &a_extents_tuple)
    : m_extent_values(a_extents_tuple)
    {}

public:

    template<typename = std::enable_if<std::is_default_constructible<tuple_type>::value>>
    SDLT_INLINE
    n_extent_t()
    {}

    explicit n_extent_t(const TypeListT&... a_values)
    : m_extent_values(a_values...)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    n_extent_t(const n_extent_t &a_other)
    : m_extent_values(a_other.m_extent_values)
    {}


    static constexpr int rank = internal::tuple_size<tuple_type>::value;
    static constexpr int row_dimension = rank-1;


    template<int DimensionT>
    SDLT_INLINE auto
    get() const
    -> typename internal::tuple_element<DimensionT, tuple_type>::type
    {
        // We choose to not disable this function because the compiler error message would say
        //     "error: no instance of function template "..."  matches the argument list
        // which could be hard to figure out.
        // Instead we provide a static assert with a meaningful message
        static_assert(rank > DimensionT, "To access the extent of dimension, DimensionT must be less than the rank");
        return m_extent_values.template get<DimensionT>();
    }

protected:
    template<int... ReducedRankSequenceT>
    SDLT_INLINE auto
    sequence_dimensions(internal::int_sequence<ReducedRankSequenceT...>) const
    ->n_extent_t<type_d<ReducedRankSequenceT, n_extent_t>...>
    {
        return n_extent_t<type_d<ReducedRankSequenceT, n_extent_t>...>(this->template get<ReducedRankSequenceT>()...);
    }

public:

    // returns a n_extent_t with the final ReduceRankT values from this n_extent_t
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

    template<class... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    equals (const n_extent_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_and_of<true>((this->template get<IntegerListT>() == a_other.template get<IntegerListT>())...);
    }

    template<class... OtherTypeListT, int ... IntegerListT>
    SDLT_INLINE
    bool
    not_equals (const n_extent_t<OtherTypeListT...> & a_other, internal::int_sequence<IntegerListT...>) const
    {
        // When OtherTypeListT is empty, the parameter may be considered unused
        __SDLT_UNUSED_PARAM(a_other);
        return internal::calc_logical_or_of<false>((this->template get<IntegerListT>() != a_other.template get<IntegerListT>())...);
    }
public:
    template<class... OtherTypeListT, typename = internal::enable_if_type<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    bool
    operator == (const n_extent_t<OtherTypeListT...> a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return equals(a_other, index_sequence());
    }

    template<class... OtherTypeListT, typename = internal::enable_if_type<rank == sizeof...(OtherTypeListT)> >
    SDLT_INLINE
    bool
    operator != (const n_extent_t<OtherTypeListT...> a_other) const
    {
        typedef internal::make_int_sequence<sizeof...(TypeListT)> index_sequence;

        return not_equals(a_other, index_sequence());
    }



protected:
    // To be compatible with extents_d<> template function
    template<int DimensionT>
    SDLT_INLINE auto
    extent_d() const
    -> typename internal::tuple_element<DimensionT, tuple_type>::type
    {
        // We choose to not disable this function because the compiler error message would say
        //     "error: no instance of function template "..."  matches the argument list
        // which could be hard to figure out.
        // Instead we provide a static assert with a meaningful message
        static_assert(rank > DimensionT, "To access the extent of dimension, DimensionT must be less than the rank");
        return m_extent_values.template get<DimensionT>();
    }

    struct hyper_volume_calculator
    {
    private:
        tuple_type  m_extent_values;
    public:
        size_t hyper_volume_size;

        SDLT_INLINE
        hyper_volume_calculator(
            const tuple_type  & extent_values_,
            int row_stride_)
        : m_extent_values(extent_values_)
        , hyper_volume_size(row_stride_)
        {
        }

        template<int IndexT>
        SDLT_INLINE void
        operator()()
        {
            static_assert(IndexT < row_dimension, "Row Stride was already included as a construction parameter, no reason to allow the 0th Extent to contribute twice");
            hyper_volume_size *= m_extent_values.template get<IndexT>();
        }
    };

public:

    SDLT_INLINE size_t
    size() const
    {
        hyper_volume_calculator calculator(m_extent_values, internal::get_row(*this));
        internal::static_for<0, rank-1>(calculator);
        return calculator.hyper_volume_size;
    }

protected:
    struct stream_out_functor
    {
    private:
        const tuple_type  & m_extent_values;
        std::ostream& m_output_stream;
    public:
        SDLT_INLINE
        stream_out_functor(
            const tuple_type& a_extent_values,
            std::ostream& a_output_stream)
        : m_extent_values(a_extent_values)
        , m_output_stream(a_output_stream)
        {}

        template<int IndexT>
        SDLT_INLINE void
        operator()()
        {
            m_output_stream << m_extent_values.template get<IndexT>();
            if (IndexT < rank-1)
            {
                m_output_stream << ", ";
            }
        }
    };
public:
    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const n_extent_t & a_extents)
    {
        output_stream << "n_extent_t{ ";

        stream_out_functor functor(a_extents.m_extent_values, output_stream);
        internal::static_for<0, rank>(functor);

        output_stream <<  " }";
        return output_stream;
    }

};

template<class... TypeListT>
class n_extent_generator
: public n_extent_t<TypeListT...>
{

public:
    typedef n_extent_t<TypeListT...> value_type;
protected:
    typedef typename value_type::tuple_type tuple_type;

    template<class... OtherTypeListT>
    friend class n_extent_generator;

    explicit SDLT_INLINE
    n_extent_generator(const tuple_type &a_values)
    : value_type(a_values)
    {}

public:

    // Only provide a default constructor when there are 0 extents
    template<typename = std::enable_if<sizeof...(TypeListT) == 0>>
    SDLT_INLINE
    n_extent_generator()
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    n_extent_generator(const n_extent_generator &a_other)
    : value_type(a_other)
    {}

    SDLT_INLINE n_extent_generator<TypeListT..., int>
    operator [] (int a_size) const
    {
        __SDLT_ASSERT(a_size >= 0);
        return n_extent_generator<TypeListT..., int>(value_type::m_extent_values.concatenate(a_size));
    }

    template<int OffsetT>
    SDLT_INLINE n_extent_generator<TypeListT..., fixed<OffsetT>>
    operator [] (fixed<OffsetT> a_size) const
    {
        // is_extent_type checks for the fixed<OffsetT> to be >= 0, no need for runtime assert
        return n_extent_generator<TypeListT..., fixed<OffsetT>>(value_type::m_extent_values.concatenate(a_size));
    }

    template<int AlignmentT>
    SDLT_INLINE n_extent_generator<TypeListT..., aligned<AlignmentT>>
    operator [] (const aligned<AlignmentT> & a_size) const
    {
        __SDLT_ASSERT(a_size >= 0);
        return n_extent_generator<TypeListT..., aligned<AlignmentT>>(value_type::m_extent_values.concatenate(a_size));
    }

    SDLT_INLINE value_type
    value() const
    {
        return *this;
    }

};

namespace {
    n_extent_generator<> n_extent;
}


namespace internal {

    template<class... TypeListT>
    struct extent_for_d<false, false, false, false, n_extent_t<TypeListT...>>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const n_extent_t<TypeListT...> &a_extents)
        ->decltype(a_extents.template get<DimensionT>())
        {
            return a_extents.template get<DimensionT>();
        }
    };

    template<class... TypeListT>
    struct extent_for_d<false, false, false, false, n_extent_generator<TypeListT...>>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const n_extent_t<TypeListT...> &a_extents)
        ->decltype(a_extents.template get<DimensionT>())
        {
            return a_extents.template get<DimensionT>();
        }
    };

    template<typename T>
    struct extent_for_d<false, false, false, true, T>
    {
        typedef T type;

        template<int DimensionT>
        static constexpr SDLT_INLINE auto
        get(const type &)
        ->fixed<std::extent<type, DimensionT>::value>
        {
            return fixed<std::extent<type, DimensionT>::value>();
        }
    };

    // Specialize for types with a size_type size() const method
    template<typename T>
    struct extent_for_d<false, false, true, false, T>
    {
        template<int DimensionT>
        static SDLT_INLINE int
        get(const T & a_container)
        {
            static_assert(DimensionT == 0, "This container type supports only dimension index 0");
            __SDLT_ASSERT(a_container.size() < static_cast<size_t>(std::numeric_limits<int>::max()));
            return static_cast<int>(a_container.size());
        }
    };
} // namespace internal


template<int DimensionT, typename ObjT>
SDLT_INLINE auto
extent_d(const ObjT &a_obj)
-> decltype(internal::extent_for_d<internal::is_n_extent_interface<ObjT>::value,
                         internal::is_n_extent_provider<ObjT>::value,
                         internal::is_size_provider<ObjT>::value,
                         std::is_array<ObjT>::value,
                         ObjT>::template get<DimensionT>(a_obj))
{
    return internal::extent_for_d<internal::is_n_extent_interface<ObjT>::value,
                        internal::is_n_extent_provider<ObjT>::value,
                        internal::is_size_provider<ObjT>::value,
                        std::is_array<ObjT>::value,
                        ObjT>::template get<DimensionT>(a_obj);
}

namespace internal {
    template<bool SupportsSizeT, typename T>
    struct extent_for_d<true, false, SupportsSizeT, false, T>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const T & a_obj)
        ->decltype(a_obj.template extent_d<DimensionT>())
        {
            return a_obj.template extent_d<DimensionT>();
        }
    };

    template<bool SupportsSizeT, typename T>
    struct extent_for_d<false, true, SupportsSizeT, false, T>
    {
        template<int DimensionT>
        static SDLT_INLINE auto
        get(const T & a_obj)
        ->decltype(extent_d<DimensionT>(a_obj.n_extent()))
        {
            return extent_d<DimensionT>(a_obj.n_extent());
        }
    };

    template <int DimensionT, typename... TypeListT>
    struct type_of_d<DimensionT, n_extent_t<TypeListT...>>
    {
        typedef typename tuple_element<DimensionT, typename n_extent_t<TypeListT...>::tuple_type>::type type;
    };


} // namespace internal
} // namepace __SDLT_IMPL
using __SDLT_IMPL::n_extent_t;
using __SDLT_IMPL::n_extent;
using __SDLT_IMPL::extent_d;

} // namespace sdlt

#endif // SDLT_EXTENTS_H
