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

#ifndef SDLT_AOS_1D_BY_STRIDE_ACCESOR_H
#define SDLT_AOS_1D_BY_STRIDE_ACCESOR_H

#include <vector>

#include "../../common.h"
#include "../../linear_index.h"
#include "../../no_offset.h"
#include "../default_offset.h"
#include "../disambiguator.h"
#include "../enable_if_type.h"
#include "../is_linear_compatible_index.h"
#include "../zero_offset.h"
#include "element.h"

namespace sdlt
{
namespace __SDLT_IMPL
{
namespace internal
{
namespace aos1d_by_stride
{

// Forward declaration
template<typename PrimitiveT, typename OffsetT>
class const_accessor;

// Expect lambda closures to copy these accessors around
// so treat copied version as a "view" that doesn't own/manage the data
// MUST BE PLAIN OLD DATA, for use with lambda closures, therefore
// no copy constructor, assignment operator, or virtual functions
template<
    typename PrimitiveT,
    typename OffsetT = no_offset
>
class accessor
{
public:
    template<typename, typename> friend class aos1d_by_stride::const_accessor;
    template<typename, typename> friend class aos1d_by_stride::accessor;
private:
    unsigned char * m_data;
    int m_size_d1;
    OffsetT m_offset;

public:
    typedef PrimitiveT primitive_type;
    typedef OffsetT offset_type;
    template<typename IndexD1T> using element = aos1d_by_stride::element< PrimitiveT, IndexD1T >;

    // default copy and assignment constructors OK
    SDLT_INLINE
    accessor()
    : m_data(nullptr)
    , m_size_d1(0)
    , m_offset(default_offset<OffsetT>::value)
    {
    }

    // Implicit conversion constructor from un-offset accessor
    // NOTE: when OffsetT == no_offset, it is a copy constructor vs. conversion constructor
    SDLT_INLINE
    accessor(const accessor<PrimitiveT, no_offset> &unoffset_accessor)
    : m_data(unoffset_accessor.m_data)
    , m_size_d1(unoffset_accessor.m_size_d1)
    , m_offset(zero_offset<OffsetT>::value)
    {
    }

    // Constructor is public to allow use of this accessor with user's
    // own Containers vs. SDLT's Containers
    explicit SDLT_INLINE
    accessor(
        PrimitiveT * a_data,
        const int size_d1,
        OffsetT offset = default_offset<OffsetT>::value)
    : m_data(reinterpret_cast<unsigned char *>(a_data))
    , m_size_d1(size_d1)
    , m_offset(offset)
    {
    }

    // Implicit conversion constructor from std::vector
    template<typename StlAllocatorT>
    SDLT_INLINE
    accessor(std::vector<PrimitiveT, StlAllocatorT> &other,
             OffsetT offset = default_offset<OffsetT>::value)
    : m_data(reinterpret_cast<unsigned char *>(&other[0]))
    , m_size_d1(other.size())
    , m_offset(offset)
    {
    }

    explicit SDLT_INLINE
    accessor(
        unsigned char * a_data,
        const int size_d1,
        OffsetT offset,
        disambiguator)
    : m_data(a_data)
    , m_size_d1(size_d1)
    , m_offset(offset)
    {
    }

    SDLT_INLINE
    accessor<PrimitiveT, int>
    reaccess(const int offset) const
    {
        return accessor<PrimitiveT, int>(
            m_data,
            this->m_size_d1,
            offset,
            disambiguator());
    }

    template<int IndexAlignmentT>
    SDLT_INLINE
    accessor<PrimitiveT, aligned<IndexAlignmentT> >
    reaccess(aligned<IndexAlignmentT> offset) const
    {
        return accessor<PrimitiveT, aligned<IndexAlignmentT>>(
            m_data,
            this->m_size_d1,
            offset,
            disambiguator());
    }

    template<int FixedOffsetT>
    SDLT_INLINE
    accessor<PrimitiveT, fixed<FixedOffsetT> >
    reaccess(fixed<FixedOffsetT> offset) const
    {
        return accessor<PrimitiveT, fixed<FixedOffsetT> >(
            m_data,
            this->m_size_d1,
            offset,
            disambiguator());
    }


    SDLT_INLINE
    int get_size_d1() const { return m_size_d1; }

    SDLT_INLINE
    offset_type get_offset() const { return m_offset; }

    template<typename IndexD1T, typename = internal::enable_if_type<is_linear_compatible_index<IndexD1T>::value>>
    SDLT_INLINE
    auto
    operator [] (const IndexD1T an_index_d1) const
    -> element<decltype(this->m_offset + an_index_d1)>
    {
        __SDLT_ASSERT(std::ptrdiff_t(value_of(m_offset + an_index_d1)) >= 0);
        __SDLT_ASSERT(std::size_t(value_of(m_offset + an_index_d1)) < std::size_t(this->get_size_d1()));
        return element<decltype(m_offset + an_index_d1)>
                       (m_data, (m_offset + an_index_d1), disambiguator());
    }

    SDLT_INLINE
    bool operator == (const accessor &other) const
    {
        return m_data == other.m_data &&
               m_size_d1 == other.m_size_d1;
    }

    SDLT_INLINE
    bool operator != (const accessor &other) const
    {
        return !(*this == other);
    }


};


} // namespace aos1d_by_stride
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_AOS_1D_BY_STRIDE_ACCESOR_H
