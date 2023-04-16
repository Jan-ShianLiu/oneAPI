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

#ifndef SDLT_BUFFER_OFFSET_IN_CACHE_LINES_H
#define SDLT_BUFFER_OFFSET_IN_CACHE_LINES_H

#include "common.h"

namespace sdlt {
namespace __SDLT_IMPL {

// buffer_offset_in_cachelines is used by the Containers to add a certain number
// of units of padding to avoid cache conflicts due to set associativity.
// http://software.intel.com/en-us/articles/optimization-and-performance-tuning-for-intel-xeon-phi-coprocessors-part-2-understanding
// "Set associativity issues (conflict misses) can occur on the Intel Xeon Phi
// coprocessor when an application is access data in L1 with a 4KB stride or
// data in L2 with a 64KB stride."
// To avoid issues, user can construct containers manually passing different
// units of padding to avoid set associativity issues.
// The Containers will guarantee that accessing the same index on containers
// with different units of padding will not interfere, up to the limits
// of the caches associativity.  It's certainly better than doing nothing.

class buffer_offset_in_cachelines
{
public:
    SDLT_INLINE explicit buffer_offset_in_cachelines(int units)
    : m_units(units)
    {}

    SDLT_INLINE buffer_offset_in_cachelines(const buffer_offset_in_cachelines &other)
    : m_units(other.m_units)
    {}

    SDLT_INLINE buffer_offset_in_cachelines & operator= (const buffer_offset_in_cachelines &other)
    {
        m_units = other.m_units;
        return *this;
    }

    SDLT_INLINE int units() const
    {
        return m_units;
    }
protected:
    int m_units;
};

} // namepace __SDLT_IMPL
using __SDLT_IMPL::buffer_offset_in_cachelines;
} // namespace sdlt


#endif // SDLT_BUFFER_OFFSET_IN_CACHE_LINES_H
