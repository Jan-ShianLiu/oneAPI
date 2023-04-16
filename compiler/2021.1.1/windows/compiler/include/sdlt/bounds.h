//
// Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_BOUNDS_H
#define SDLT_BOUNDS_H

#include <iostream>

#include <sdlt/common.h>
#include <sdlt/aligned.h>
#include <sdlt/fixed.h>
#include <sdlt/linear_index.h>

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

    template<class T>
    struct is_bounds_component_type : public std::false_type{};

    template<>
    struct is_bounds_component_type<int> : public std::true_type {};

    template<int ValueT>
    struct is_bounds_component_type<sdlt::fixed<ValueT>> : public std::true_type {};

    template<int IndexAlignmentT>
    struct is_bounds_component_type<sdlt::aligned<IndexAlignmentT>> : public std::true_type {};

    // Only support the minimum interface required for a range based interations
    struct index_iter
    {
    protected:
        int m_index;
    public:
        typedef int difference_type;

        SDLT_INLINE explicit index_iter(int a_index)
            : m_index(a_index)
        {}

        SDLT_INLINE index_iter(const index_iter &a_other)
            : m_index(a_other.m_index)
        {}

        SDLT_INLINE index_iter operator ++(int)
        {
            index_iter before_increment(*this);
            ++m_index;
            return before_increment;
        }

        // #pragma omp simd
        // with a for loop composed of index_iter calls += unsigned int
        SDLT_INLINE index_iter & operator +=(unsigned int a_step)
        {
            m_index += a_step;
            return *this;
        }

        SDLT_INLINE index_iter & operator ++()
        {
            ++m_index;
            return *this;
        }

        SDLT_INLINE difference_type operator -(const index_iter & a_other) const
        {
            return m_index - a_other.m_index;
        }

        // VERY IMPORTANT!
        // DO NOT PROVIDE A != operator
        // instead support conversion operator to int
        // this is necessary to allow a canonical loop to be formed
        // apparently integers for a loop count is necessary
        SDLT_INLINE operator int() const
        {
            return m_index;
        }

        SDLT_INLINE linear_index operator*() const
        {
            return linear_index(m_index);
        }
    };
} // namespace internal



// Consider deriving from base class with LowerT and UpperT to allow for a 0 sized data members
// when they are fixed
template<typename LowerT = int, typename UpperT = int>
struct bounds_t
{
    static_assert(internal::is_bounds_component_type<LowerT>::value, "Invalid LowerT type used");
    static_assert(internal::is_bounds_component_type<UpperT>::value, "Invalid UpperT type used");

    typedef LowerT lower_type;
    typedef UpperT upper_type;

    typedef internal::index_iter iterator;

    template<typename, typename >
    friend struct bounds_t;
protected:
    lower_type m_lower;
    upper_type m_upper;
public:

    SDLT_INLINE bounds_t()
    {}

    SDLT_INLINE bounds_t(lower_type a_lower, upper_type a_upper)
    : m_lower(a_lower)
    , m_upper(a_upper)
    {
        __SDLT_ASSERT(a_upper >= a_lower);
    }

    SDLT_INLINE bounds_t(const bounds_t & a_other)
    : m_lower(a_other.m_lower)
    , m_upper(a_other.m_upper)
    {}

    // Use SFINAE to only enable implicit conversion for castable
    // lower and upper types
    template<typename OtherLowerT, typename OtherUpperT,
        typename = decltype(static_cast<LowerT>(std::declval<OtherLowerT>())),
        typename = decltype(static_cast<UpperT>(std::declval<OtherUpperT>()))>
    SDLT_INLINE bounds_t(const bounds_t<OtherLowerT, OtherUpperT> & a_other)
    : m_lower(a_other.m_lower)
    , m_upper(a_other.m_upper)
    {}


    SDLT_INLINE void set(lower_type a_lower, upper_type a_upper)
    {
        m_lower = a_lower;
        m_upper = a_upper;
    }

    SDLT_INLINE void set_lower(lower_type a_lower)
    {
        m_lower = a_lower;
    }

    SDLT_INLINE void set_upper(upper_type a_upper)
    {
        m_upper = a_upper;
    }

    SDLT_INLINE lower_type lower() const
    {
        return m_lower;
    }

    SDLT_INLINE upper_type upper() const
    {
        return m_upper;
    }

    // Provide c++11 range based for 
    iterator begin() const
    {
        return iterator(m_lower);
    }

    iterator end() const
    {
        return iterator(m_upper);
    }

    SDLT_INLINE auto
    width() const
    ->decltype(m_upper - m_lower)
    {
        return m_upper - m_lower;
    }

    SDLT_INLINE
    bool operator == (const bounds_t &a_other) const
    {
        return lower() == a_other.lower() && upper() == a_other.upper();
    }

    template<typename OtherLowerT, typename OtherUpperT>
    SDLT_INLINE
    bool operator == (const bounds_t<OtherLowerT, OtherUpperT> &a_other) const
    {
        return lower() == a_other.lower() && upper() == a_other.upper();
    }

    SDLT_INLINE
    bool operator != (const bounds_t &a_other) const
    {
        return lower() != a_other.lower() || upper() != a_other.upper();
    }

    template<typename OtherLowerT, typename OtherUpperT>
    SDLT_INLINE
    bool operator != (const bounds_t<OtherLowerT, OtherUpperT> &a_other) const
    {
        return lower() != a_other.lower() || upper() != a_other.upper();
    }


    template<typename OtherLowerT, typename OtherUpperT>
    SDLT_INLINE bool contains(const bounds_t<OtherLowerT, OtherUpperT> &a_other) const
    {
        return a_other.m_lower >= m_lower &&
               a_other.m_upper <= m_upper;
    }

    template<typename T, typename = internal::enable_if_type<std::is_same<T, fixed<0>>::value == false>>
    SDLT_INLINE bounds_t<decltype(std::declval<LowerT>() + std::declval<T>()), decltype(std::declval<UpperT>() + std::declval<T>())>
    operator + (const T &a_arg) const
    {
        return bounds_t<decltype(std::declval<LowerT>() + std::declval<T>()), decltype(std::declval<UpperT>() + std::declval<T>())>(
                m_lower + a_arg, m_upper + a_arg
                );
    }

    template<typename T, typename = internal::enable_if_type<std::is_same<T, fixed<0>>::value == false>>
    SDLT_INLINE bounds_t<decltype(std::declval<LowerT>() - std::declval<T>()), decltype(std::declval<UpperT>() - std::declval<T>())>
    operator - (const T &a_arg) const
    {
        return bounds_t<decltype(std::declval<LowerT>() - std::declval<T>()), decltype(std::declval<UpperT>() - std::declval<T>())>(
                m_lower - a_arg, m_upper - a_arg
                );
    }



#ifdef __SDLT_RESERVED_FOR_FUTURE_USE
    SDLT_INLINE int median() const
    {
        return width()/2;
    }


    SDLT_INLINE bool empty() const {
        return width() == 0;
    }

    SDLT_INLINE void union_with(const bounds_t &a_other)
    {
        // to influence a bounds_t, it must be non-empty
        if (empty())
        {
            m_lower = a_other.m_lower;
            m_upper = a_other.m_upper;
        } else {
            if (a_other.empty() == false) {
                if (a_other.m_lower < m_lower) {
                    m_lower = a_other.m_lower;
                }
                if (a_other.m_upper > m_upper) {
                    m_upper = a_other.m_upper;
                }
            }
        }
    }

    SDLT_INLINE void clear() {
        m_lower = m_upper = 0;
    }

    SDLT_INLINE int offset_from(const bounds_t &a_other) const
    {
        return a_other.m_lower - m_lower;
    }
#endif

    SDLT_INLINE
    friend std::ostream& operator << (std::ostream& a_output_stream, const bounds_t &a_bounds)
    {
        a_output_stream << "bounds_t{lower()="
                      << a_bounds.lower()
                      << ", upper()="
                      << a_bounds.upper() <<"}";
        return a_output_stream;
    }
};


    
} // namepace __SDLT_IMPL
using __SDLT_IMPL::bounds_t;

// Factory function, choose to drop the usual "make_" prefix to
// allow to users to read it as if it were an explicit constructor
//
template<typename LowerT, typename UpperT>
SDLT_INLINE auto
bounds(LowerT a_lower, UpperT a_upper)
-> bounds_t<LowerT, UpperT>
{
    return bounds_t<LowerT, UpperT>(a_lower, a_upper);
}

} // namespace sdlt

#endif // SDLT_BOUNDS_H
