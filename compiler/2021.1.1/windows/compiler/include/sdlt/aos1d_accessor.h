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

#ifndef SDLT_AOS_1D_ACCESSOR_H
#define SDLT_AOS_1D_ACCESSOR_H

#include <type_traits>

#include "access_by.h"
#include "internal/aos1d_by_stride/accessor.h"
#include "internal/aos1d_by_struct/accessor.h"

namespace sdlt
{
// Can create an SDLT compatible accessor over existing Array of Structures data,
// Just instantiate with the structure type as the template parameter and pass in the
// pointer to the first element and the number of elements.  Intended usage
// is when you can not put the data inside an SDLT container because its
// managed outside the code that can be modified.
// IE:
//
// struct MyStruct { float weight; float x; float y; };
// SDLT_PRIMITIVE(MyStruct, weight, x, y)
//
// MyStruct dataBuffer[100];
// aos1d_accessor<MyStruct, access_by_struct> accessor(dataBuffer, 100);
//
// Then accessor can be used inside SIMD loops
//
// const MyStruct existingElement = accessor[i];
// MyStruct newElement(existingElement);
// newElement.weight += 20;
// accessor[i] = newElement;
//
// and or utilize the accessor's member_interface to struct data members
//
// const float existingWeight = accessor[i].weight();
// float newWeight = existingWeight + 20;
// accessor[i].weight() = newWeight;
//
//
// Also supports construction directly from std::vector.  IE:
//
// std::vector<MyStruct> dataBuffer(100);
// aos1d_accessor<MyStruct, access_by_stride> accessor(dataBuffer);
//

template <
    typename PrimitiveT,
    access_by AccessByT
>
using aos1d_accessor =
    typename std::conditional<AccessByT == access_by_stride,
            __SDLT_IMPL::internal::aos1d_by_stride::accessor<PrimitiveT>,
            __SDLT_IMPL::internal::aos1d_by_struct::accessor<PrimitiveT>
            >::type;

} // namespace sdlt

#endif // SDLT_AOS_1D_ACCESSOR_H
