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

#ifndef SDLT_ALIGNED_BASE_H
#define SDLT_ALIGNED_BASE_H

#include "../common.h"
#include "power_of_two.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

// Issues with using alignas(ByteAlignmentT) or
// __declspec(aligned(ByteAlignmentT)) with template classes have caused us
// to employ the workaround of having a base class that specifies the alignment
template <int ByteAlignmentT> struct aligned_base
{
    static_assert(internal::power_of_two<ByteAlignmentT>::exists, "ByteAlignmentT must be a power of two");
    static_assert(ByteAlignmentT <= 64, "No need to align to a byte boundary > 64");
};

template <> struct __SDLT_ALIGN(2) aligned_base<2> {};
template <> struct __SDLT_ALIGN(4) aligned_base<4> {};
template <> struct __SDLT_ALIGN(8) aligned_base<8> {};
template <> struct __SDLT_ALIGN(16) aligned_base<16> {};
template <> struct __SDLT_ALIGN(32) aligned_base<32> {};
template <> struct __SDLT_ALIGN(64) aligned_base<64> {};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ALIGNED_BASE_H
