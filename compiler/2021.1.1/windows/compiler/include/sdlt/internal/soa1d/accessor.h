//
// Copyright (C) 2014-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_SOA_1D_ACCESSOR_H
#define SDLT_SOA_1D_ACCESSOR_H

#include <type_traits>

#include "../../common.h"
#include "../../linear_index.h"
#include "../../no_offset.h"
#include "../default_offset.h"
#include "../enable_if_type.h"
#include "../is_linear_compatible_index.h"
#include "../zero_offset.h"
#include "element.h"
#include "organization.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa1d {

// Forward declaration
template<
    typename PrimitiveT,
    int AlignD1OnIndexT,
    typename OffsetT>
class const_accessor;


__SDLT_INHERIT_IMPL_BEGIN

// Expect lambda closures to copy these accessors around
// so treat copied version as a "view" that doesn't own/manage the data
// MUST BE PLAIN OLD DATA, for use with lambda closures, therefore
// no copy constructor, assignment operator, or virtual functions
template<
    typename PrimitiveT,
    int AlignD1OnIndexT,
    typename OffsetT = no_offset
>
class accessor :
    protected organization
{
protected:
    template<typename , int, typename> friend class soa1d::const_accessor;
    template<typename , int, typename> friend class soa1d::accessor;

    unsigned char * m_data;
    OffsetT m_offset;

public:
    typedef PrimitiveT primitive_type;
    typedef OffsetT offset_type;

    // default copy and assignment constructors OK
    SDLT_INLINE
    accessor()
    : organization(0, 0)
    , m_data(nullptr)
    , m_offset(default_offset<OffsetT>::value)
    {
    }

    // Implicit conversion constructor from un-offset accessor
    // NOTE: when OffsetT == no_offset, it is a copy constructor vs. conversion constructor
    SDLT_INLINE
    accessor(const accessor<PrimitiveT, AlignD1OnIndexT, no_offset> &unoffset_accessor)
    : organization(unoffset_accessor)
    , m_data(unoffset_accessor.m_data)
    , m_offset(zero_offset<OffsetT>::value)
    {
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(this->m_data);
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
    }

    explicit SDLT_INLINE
    accessor(
        unsigned char * a_data,
        const int size_d1,
        const int iStrideD1,
        OffsetT offset = default_offset<OffsetT>::value
    )
    : organization(size_d1, iStrideD1)
    , m_data(a_data)
    , m_offset(offset)
    {
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
    }

    SDLT_INLINE
    const int & get_size_d1() const { return this->m_size_d1; }

    SDLT_INLINE
    offset_type get_offset() const { return m_offset; }

    SDLT_INLINE
    accessor<PrimitiveT, AlignD1OnIndexT, int>
    reaccess(const int offset) const
    {
        return accessor<PrimitiveT, AlignD1OnIndexT, int>(
            m_data,
            this->m_size_d1,
            this->m_stride_d1,
            offset);
    }

    template<int IndexAlignmentT>
    SDLT_INLINE
    accessor<PrimitiveT, AlignD1OnIndexT, aligned<IndexAlignmentT> >
    reaccess(aligned<IndexAlignmentT> offset) const
    {
        return accessor<PrimitiveT, AlignD1OnIndexT, aligned<IndexAlignmentT>>(
            m_data,
            this->m_size_d1,
            this->m_stride_d1,
            offset);
    }

    template<int FixedOffsetT>
    SDLT_INLINE
    accessor<PrimitiveT, AlignD1OnIndexT, fixed<FixedOffsetT> >
    reaccess(fixed<FixedOffsetT>) const
    {
        return accessor<PrimitiveT, AlignD1OnIndexT, fixed<FixedOffsetT> >(
            m_data,
            this->m_size_d1,
            this->m_stride_d1);
    }

    template<typename IndexD1T, typename = internal::enable_if_type<is_linear_compatible_index<IndexD1T>::value>>
    SDLT_INLINE
    auto
    operator [] (const IndexD1T an_index_d1) const
    -> element<PrimitiveT,
               AlignD1OnIndexT,
               decltype(this->m_offset + an_index_d1)>
    {
        __SDLT_ASSERT(std::ptrdiff_t(value_of(m_offset + an_index_d1)) >= 0);
        __SDLT_ASSERT(size_t(value_of(m_offset + an_index_d1)) < size_t(this->get_size_d1()));
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(this->m_data);
        return element<PrimitiveT,
                            AlignD1OnIndexT,
                            decltype(m_offset + an_index_d1)>
                            (m_data, *this, (m_offset + an_index_d1));
    }


    SDLT_INLINE
    bool operator == (const accessor &other) const
    {
        return m_data == other.m_data &&
               m_offset == other.m_offset &&
               organization::operator==(other);
    }
};

__SDLT_INHERIT_IMPL_END

} // namespace soa1d
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_1D_ACCESSOR_H
