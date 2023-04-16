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

#ifndef SDLT_CAPACITY_H
#define SDLT_CAPACITY_H

#include "../common.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

// capacity is used by the Containers to represent the number of elements
// a container is capable of holding.
// We choose to explicitly model capacity versus using an int.
// This allows for more readable user code and meaningful constructor
// overloading

class capacity
{
public:
    SDLT_INLINE explicit capacity(int a_count)
    : m_count(a_count)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE capacity(const capacity &other)
    : m_count(other.m_count)
    {}

    SDLT_INLINE capacity & operator= (const capacity &other)
    {
        m_count = other.m_count;
        return *this;
    }

    SDLT_INLINE int count() const
    {
        return m_count;
    }
protected:
    int m_count;
};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_CAPACITY_H
