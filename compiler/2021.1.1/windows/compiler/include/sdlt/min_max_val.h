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

#ifndef SDLT_MIN_MAX_VAL_H
#define SDLT_MIN_MAX_VAL_H

#include "common.h"

namespace sdlt {
namespace __SDLT_IMPL {

// C++ implementations of std::min and std::max may be creating conditional
// flow returning references to its parameters, this can create flow
// dependencies and interfere with vectorization
//
// Here are some really simple min max templates that returns by value
// They should inline and add no overhead
// Also both only use the < operator, perhaps enabling code using both min
// and max to do a single comparison versus both < and >.

template<typename T>
SDLT_INLINE
T max_val(const T left, const T right)
{
    T val;
    if (left < right) {
      val = right;
    } else {
      val = left;
    }
    return val;
}

template<typename T>
SDLT_INLINE
T min_val(const T left, const T right)
{
    T val;
    if (left < right) {
      val = left;
    } else {
      val = right;
    }
    return val;
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::max_val;
using __SDLT_IMPL::min_val;
} // namespace sdlt


#endif // SDLT_MIN_MAX_VAL_H

