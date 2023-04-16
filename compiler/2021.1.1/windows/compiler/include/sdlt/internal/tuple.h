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

#ifndef SDLT_TUPLE_H
#define SDLT_TUPLE_H


#include <type_traits>
#include "../common.h"
#include "int_sequence.h"
#include "logical_and_of.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template <typename TypeT>
struct tuple_size; /* undefined */

template<typename T>
class is_get_compatible
{
    static_assert(sizeof(long) > sizeof(char), "Target platform must have sizeof long > char");
    template<typename CheckT> static long check(decltype(&CheckT::template get <0>));
    template<typename CheckT> static char check(...);
public:
    static const bool value = sizeof(check<T>(nullptr)) > sizeof(char);
};

template<typename... TypeListT>
struct type_sequence
{
};

template <typename... TypeListT>
struct tuple;

template <int IndexT, typename tupleT>
struct tuple_element;

template <int IndexT, typename HeadT, typename... TailListT>
struct tuple_element<IndexT, type_sequence<HeadT, TailListT...>>
{
    typedef typename tuple_element<IndexT-1, type_sequence<TailListT...> >::type type;
};

template <typename HeadT, typename... TailListT>
struct tuple_element<0, type_sequence<HeadT, TailListT...>>
{
    typedef HeadT type;
};

template <int IndexT, typename... TypeListT>
struct tuple_element<IndexT, tuple<TypeListT...>>
{
    typedef typename tuple_element<IndexT, type_sequence<TypeListT...> >::type type;
};


// capacity is used by the Containers to represent the number of elements
// a container is capable of holding.
// We choose to explicitly model capacity versus using an int.
// This allows for more readable user code and meaningful constructor
// overloading

template<int IndexT, typename TypeT>
class tuple_component
{
public:
    typedef TypeT type;
    static constexpr int index = IndexT;

    template<typename = std::enable_if<std::is_default_constructible<TypeT>::value>>
    SDLT_INLINE
    tuple_component()
    {}

    SDLT_INLINE
    tuple_component(const TypeT &a_value)
    : m_value(a_value)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    tuple_component(const tuple_component &a_other)
    : m_value(a_other.m_value)
    {}


    SDLT_INLINE TypeT
    get() const
    {
        return m_value;
    }

    SDLT_INLINE void
    set(TypeT a_value)
    {
        m_value = a_value;
    }

    template<int AlignmentT>
    SDLT_INLINE void
    assume_aligned() const
    {
        __SDLT_ASSUME(m_value%(AlignmentT) == 0);
    }

private:
    TypeT m_value;

};

template <typename IntSequenceT, typename... TypeListT>
struct tuple_components;

template <int... IntegerListT, typename... TypeListT>
struct tuple_components<int_sequence<IntegerListT...>, TypeListT...>
: public tuple_component<IntegerListT, TypeListT>...
{
    typedef tuple_components<int_sequence<IntegerListT...>, TypeListT...> self;

    template<typename = std::enable_if<internal::logical_and_of<std::is_default_constructible<TypeListT>...>::value>>
    SDLT_INLINE tuple_components()
    {}

    template<typename ValueProviderT, typename = std::enable_if<is_get_compatible<ValueProviderT>::value>>
    SDLT_INLINE tuple_components(const ValueProviderT & a_value_provider)
    : tuple_component<IntegerListT, TypeListT>(a_value_provider.template get<IntegerListT>())...
    {}

    explicit
    SDLT_INLINE tuple_components(int_sequence<IntegerListT...>, type_sequence<TypeListT...>, const TypeListT &... a_values)
    : tuple_component<IntegerListT, TypeListT>(a_values)...
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE tuple_components(const tuple_components & a_other)
    : tuple_component<IntegerListT, TypeListT>(a_other.template get<IntegerListT>())...
    {}

protected:
    template <int... OtherIntegerListT, typename... OtherTypesT, int... TailIntegerListT, typename... TailListT>
    SDLT_INLINE explicit
    tuple_components(const tuple_components<int_sequence<OtherIntegerListT...>, OtherTypesT...> & a_other, int_sequence<TailIntegerListT...>, const TailListT &... a_values)
    : tuple_component<OtherIntegerListT, OtherTypesT>(a_other.template get<OtherIntegerListT>())...
    , tuple_component<sizeof...(OtherIntegerListT) + TailIntegerListT, TailListT>(a_values)...
    {}
public:

    template <int... OtherIntegerListT, typename... OtherTypesT, typename... TailListT>
    SDLT_INLINE explicit
    tuple_components(const tuple_components<int_sequence<OtherIntegerListT...>, OtherTypesT...> & a_other, const TailListT &... a_values)
    : tuple_components(a_other, make_int_sequence<sizeof...(TailListT)>(), a_values...)
    {}

    template<int IndexT>
    SDLT_INLINE typename tuple_element<IndexT, self>::type
    get() const
    {
        static_assert(sizeof...(TypeListT) > IndexT, "To get, IndexT must be the less than the number of types");
        typedef typename tuple_element<IndexT, self>::type component_type;
        return tuple_component<IndexT, component_type>::get();
    }

    template<int IndexT>
    SDLT_INLINE void
    set(const typename tuple_element<IndexT, self>::type &aValue)
    {
        typedef typename tuple_element<IndexT, self>::type component_type;
        tuple_component<IndexT, component_type>::set(aValue);
    }



    template<int IndexT, int AlignmentT>
    SDLT_INLINE void
    assume_aligned() const
    {
        typedef typename tuple_element<IndexT, self>::type component_type;
        tuple_component<IndexT, component_type>::template assume_aligned<AlignmentT>();
    }

};

template <int IndexT, typename IndexSequenceT, typename... TypeListT>
struct tuple_element<IndexT, tuple_components<IndexSequenceT, TypeListT...>>
{
    static_assert(sizeof...(TypeListT) > IndexT, "IndexT must be less than the size of the TypeListT");

    typedef typename tuple_element<IndexT, type_sequence<TypeListT...> >::type type;
};




template <typename... TypeListT>
struct tuple
{
protected:
    typedef make_int_sequence<sizeof...(TypeListT)> index_sequence;
    typedef tuple_components<index_sequence, TypeListT...> components;

    template <typename...> friend struct tuple;

    components m_components;
public:
    typedef tuple<TypeListT...> self;

    template<typename = std::enable_if<std::is_default_constructible<components>::value>>
    SDLT_INLINE
    tuple()
    {}

    SDLT_INLINE explicit
    tuple(const TypeListT&... a_values)
    : m_components(index_sequence(), type_sequence<TypeListT...>(), a_values...)
    {}

    template<typename ValueProviderT, typename = std::enable_if<is_get_compatible<ValueProviderT>::value>>
    SDLT_INLINE explicit
    tuple(const ValueProviderT & a_value_provider)
    : m_components(a_value_provider)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE tuple(const tuple& a_other)
    : m_components(a_other.m_components)
    {}

protected:
    template <typename... HeadListT, typename... TailListT>
    SDLT_INLINE explicit
    tuple(const tuple<HeadListT...> & a_other, const TailListT &... a_values)
    : m_components(a_other.m_components, a_values...)
    {}
public:

    template<int IndexT>
    SDLT_INLINE auto
    get() const
    ->decltype(std::declval<components>().template get<IndexT>())
    {
        static_assert(sizeof...(TypeListT) > IndexT, "To get, IndexT must be less than the size of the TypeListT");
        return m_components.template get<IndexT>();
    }

    template<int IndexT>
    SDLT_INLINE void
    set(const typename tuple_element<IndexT, self>::type &a_value)
    {
        m_components.template set<IndexT>(a_value);
    }

    template<int IndexT, int AlignmentT>
    SDLT_INLINE void
    assume_aligned() const
    {
        m_components.template assume_aligned<IndexT, AlignmentT>();
    }


    template<typename... ValueListT>
    SDLT_INLINE
    tuple<TypeListT..., ValueListT...> concatenate(const ValueListT &... a_values) const
    {
        return tuple<TypeListT..., ValueListT...>(*this, a_values...);
    }


};

template <>
struct tuple<>
{

    template<typename OtherT>
    SDLT_INLINE tuple<OtherT>
    concatenate(const OtherT &a_other) const
    {
        return tuple<OtherT>(a_other);
    }

    template<typename... ValueListT>
    SDLT_INLINE tuple<ValueListT...>
    concatenate(const ValueListT &... a_values) const
    {
        return tuple<ValueListT...>(a_values...);
    }

};


template <typename... TypeListT>
struct tuple_size<tuple<TypeListT...>>
: public std::integral_constant<int, sizeof...(TypeListT)>
{
};

// TODO:  Move to its own file
template <int DimensionT, typename NDimensionalT>
struct type_of_d; // undefined


// TODO:  Move to its own file
template<typename NTypeT>
SDLT_INLINE auto
get_row(const NTypeT & a_n_type)
->decltype(a_n_type.template get<NTypeT::row_dimension>())
{
    return a_n_type.template get<NTypeT::row_dimension>();
}

} // namespace internal

template <int DimensionT, typename N_ObjT>
using type_d = typename internal::type_of_d<DimensionT, N_ObjT>::type;

} // namepace __SDLT_IMPL
using __SDLT_IMPL::type_d;

} // namespace sdlt


#endif // SDLT_CAPACITY_H
