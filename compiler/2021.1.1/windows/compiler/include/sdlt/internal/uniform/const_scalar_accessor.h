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

#ifndef SDLT_UNIFORM_CONST_SCALAR_ACCESSOR_H
#define SDLT_UNIFORM_CONST_SCALAR_ACCESSOR_H


#include "../../common.h"
#include "../../primitive.h"
#include "../enable_if_type.h"
#include "const_scalar_element.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace uniform {


__SDLT_INHERIT_IMPL_BEGIN

template<typename ScalarDataT>
class const_scalar_accessor
{
public:
    SDLT_INLINE
    explicit const_scalar_accessor(const ScalarDataT & a_scalar_data)
    : m_scalar_data(a_scalar_data)
    {
    }

    const_scalar_accessor(const const_scalar_accessor & other)
    : m_scalar_data(other.m_scalar_data)
    {
    }

    template<typename OffsetT>
    SDLT_INLINE
    const const_scalar_accessor<ScalarDataT> &
    reaccess(const OffsetT &) const
    {
        // As the values are uniform
        // no matter what the offset this accessor will suffice
        return *this;
    }

    SDLT_INLINE
    operator ScalarDataT() const
    {
        return m_scalar_data;
    }

    template<typename IndexD1T>
    SDLT_INLINE
    const_scalar_element<ScalarDataT>
    operator [] (const IndexD1T /*an_index_d1*/) const
    {
        return const_scalar_element<ScalarDataT>(m_scalar_data);
    }

protected:
    template<typename>
    friend class sdlt::primitive;

    template<typename>
    friend class const_scalar_element;


    const ScalarDataT & root_scalar_data() const { return m_scalar_data; }

    const ScalarDataT m_scalar_data;
};

__SDLT_INHERIT_IMPL_END

} // namespace uniform
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_UNIFORM_CONST_SCALAR_ACCESSOR_H
