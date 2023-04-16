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

#ifndef SDLT_SOA_1D_ORGANIZATION_1D_H
#define SDLT_SOA_1D_ORGANIZATION_1D_H

#include "../../common.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa1d {


struct organization
{
public:
    explicit SDLT_INLINE organization(
        const int size_d1,
        const int stride_d1
    )
    : m_stride_d1(stride_d1)
    , m_size_d1(size_d1)
    {}

    SDLT_INLINE organization(const organization &other)
    : m_stride_d1(other.m_stride_d1)
    , m_size_d1(other.m_size_d1)
    {}


    // public on purpose, because we need to issue hints to the compiler
    // on actual variables, not on member function return values

    int m_stride_d1; // Maybe larger than m_size_d1 to allow SIMD chunks
    int m_size_d1;

    bool operator == (const organization &other) const
    {
        return m_stride_d1 == other.m_stride_d1 &&
            m_size_d1 == other.m_size_d1;
    }

    organization & operator = (const organization &other)
    {
        m_size_d1 = other.m_size_d1;
        m_stride_d1 = other.m_stride_d1;
        return *this;
    }

};


} // namespace soa1d
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_1D_ORGANIZATION_1D_H
