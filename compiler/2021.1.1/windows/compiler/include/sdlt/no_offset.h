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

#ifndef SDLT_NO_OFFSET_H
#define SDLT_NO_OFFSET_H

//#include "../common.h"
#include "internal/zero_offset.h"

namespace sdlt {
namespace __SDLT_IMPL {

struct no_offset
{
    constexpr bool operator == (const no_offset &) const
    {
        return true;
    }
    constexpr bool operator != (const no_offset &) const
    {
        return false;
    }
};

namespace internal
{
    template<>
    struct zero_offset<no_offset>
    {
        static constexpr no_offset value = no_offset();
    };
}


// Free functions need to be in same namespaces as their parameters to properly support
// argument dependent lookup

template<typename OtherT>
SDLT_INLINE constexpr
OtherT operator +(const no_offset, const OtherT &a_other)
{
    // NOTE:  Notice return type is by value versus a const &.
    // This is a conscience choice as the return type is used as a template parameter
    // and we want the parameter to just be the value type not a const &.
    // TODO:  There are other solutions to this
    return a_other;
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::no_offset;
} // namespace sdlt

#endif // SDLT_NO_OFFSET_H
