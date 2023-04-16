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

#ifndef SDLT_IS_ALIGNED_H
#define SDLT_IS_ALIGNED_H

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template <size_t ByteAlignmentT, typename T>
SDLT_INLINE bool is_aligned(const T *a_pointer)
{
    union pointer_or_number {
        const T * pointer;
        size_t number;
    };
    pointer_or_number pon;
    pon.pointer = a_pointer;
    return (pon.number%ByteAlignmentT==0);
}

} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_IS_ALIGNED_H
