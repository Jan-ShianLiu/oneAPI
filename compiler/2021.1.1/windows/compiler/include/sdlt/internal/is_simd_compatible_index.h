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

#ifndef SDLT_IS_SIMD_COMPATIBLE_INDEX_H
#define SDLT_IS_SIMD_COMPATIBLE_INDEX_H

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<typename T, int SimdLaneCountT>
class is_simd_compatible_index
{
    static_assert(sizeof(long) > sizeof(char), "Target platform must have sizeof long > char");
    template<typename CheckT> static long check(decltype(&CheckT::template simd_struct_index <SimdLaneCountT>),
                                                decltype(&CheckT::simdlane_index),
                                                decltype(&CheckT::value));
    template<typename CheckT> static char check(...);
public:
    static const bool value = sizeof(check<T>(nullptr, nullptr, nullptr)) > sizeof(char);
};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_IS_SIMD_COMPATIBLE_INDEX_H
