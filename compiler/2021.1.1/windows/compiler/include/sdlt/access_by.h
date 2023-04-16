//
// Copyright (c) 2015-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_ACCESS_BY_H
#define SDLT_ACCESS_BY_H

namespace sdlt
{

// Used by aos1d_container to control the type of accessor.
//
// access_by_struct will cause data access via structure member access.
// Nested structures will drill down through the structure members in a
// nested manner.
// Which expands to something like
//     AABB local;
//     local = accessor.m_data[i];
//
// access_by_stride will cause data access through pointers to a built in types
// with a stride to account for the size of the primitive.
// Which expands to something like
//     AABB local;
//     local.topLeft.x = *(accessor.m_data + offsetof(AABS,topLeft) + offset(Point3d,x) + (sizeof(AABB)*i));
//     local.topLeft.y = *(accessor.m_data + offsetof(AABS,topLeft) + offset(Point3d,y) + (sizeof(AABB)*i));
//     local.topLeft.z = *(accessor.m_data + offsetof(AABS,topLeft) + offset(Point3d,z) + (sizeof(AABB)*i));
//     local.topRight.x = *(accessor.m_data + offsetof(AABS,topRight) + offset(Point3d,x) + (sizeof(AABB)*i));
//     local.topRight.y = *(accessor.m_data + offsetof(AABS,topRight) + offset(Point3d,y) + (sizeof(AABB)*i));
//     local.topRight.z = *(accessor.m_data + offsetof(AABS,topRight) + offset(Point3d,z) + (sizeof(AABB)*i));
//
//
// When vectorizing, access_by_struct can sometimes generate better code as the
// compiler could perform wide loads and use shuffle/insert instructions to
// move data into SIMD registers.  However, depending on the complexity of
// the primitive, it can also fail to vectorize.  Especially when the
// primitive contains nested structures.
//
// On the other hand access_by_stride has always vectorized successfully,
// because the data access is simplified to an array pointer with a stride.
// The compiler has shown to be able to handle any complexity of primitive,
// because it never sees the complexity and instead just sees the simple
// array pointer with strided access.
//
// So access_by_stride is a safe choice, but can be worth trying access_by_struct
// to see if it provides better code generation.
//
// We leave this choice up to the developer and require they explicitly
// make a choice, so this is not hidden behavior.
enum access_by
{
    // Assign specific values to enable compiler supplied defines
    access_by_struct = 0,
    access_by_stride = 1
};

} // namespace sdlt

#endif // SDLT_ACCESS_BY_H
