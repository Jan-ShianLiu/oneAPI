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

#ifndef SDLT_PRIMITIVE_H
#define SDLT_PRIMITIVE_H

#include <cstddef>
#include <stdint.h>
#include <type_traits>

#include "common.h"
#include "internal/aligned_base.h"
#include "internal/always_false_type.h"
#include "internal/compiler_defined.h"
#include "internal/emit_for_each.h"
#include "internal/find_largest_builtin.h"
#include "internal/find_smallest_builtin.h"
#include "internal/is_builtin.h"
#include "internal/simd/element.h"
#include "internal/simd_vector_register.h"

namespace sdlt
{
namespace __SDLT_IMPL
{

// The layout of the Objects used within Containers and Accessors
// must be described by specializing the template
// primitive<> for each type of Object
//
// Use the macro SDLT_PRIMITIVE to easily define the specialized template
// specialization for you.  IE:
//    struct UserObject
//    {
//        float x;
//        float y;
//        double acceleration;
//        int behavior;
//    };
//    SDLT_PRIMITIVE(
//        UserObject,
//        x
//        y
//        acceleration
//        behavior
//    )

template <typename StructT>
class primitive
{
public:
    static_assert(std::is_same<bool, StructT>::value == false, "bool is an unsupported type for use in SDLT Containers.  Instead change to a 32 bit integer (int) and set and test explicit values 0 and 1 (or 0xFFFFFFFF)");

    primitive() {
        static_assert(internal::always_false_type<StructT>::value, "You must declare the struct as a SDLT_PRIMITIVE(STRUCT_NAME, MEMBER1, MEMBER2, ..., MemberN) before trying to instantiate a Container with it");
    }

    static const bool declared = false;

    template<int SimdLaneCountT, int ByteAlignmentT>
    struct simd_type
    : public internal::aligned_base<ByteAlignmentT>
    {
    };
};

} // namepace __SDLT_IMPL
using __SDLT_IMPL::primitive;
} // namespace sdlt

#define __SDLT_BUILTIN_PRIMITIVE(BUILTIN_TYPE)   \
namespace sdlt { \
namespace __SDLT_IMPL { \
template <> \
class primitive<BUILTIN_TYPE> {   \
public: \
    static_assert(internal::is_builtin<BUILTIN_TYPE>::value, "SDLT_BUILTIN_PRIMITIVE declared for non builtin type:" #BUILTIN_TYPE); \
    static const bool declared = true; \
    static constexpr bool is_declared() { return true; } \
    \
    typedef BUILTIN_TYPE struct_type;   \
    typedef BUILTIN_TYPE largest_builtin_type; \
    typedef BUILTIN_TYPE smallest_builtin_type; \
    \
    template <class ElementAgentT>  \
    static    \
    void transfer(ElementAgentT & an_element_agent, BUILTIN_TYPE &an_object); \
    \
    template<typename DerivedT, int InterfaceOffsetT, template <typename, int> class ProxyT> \
    class member_interface { \
    public: \
        struct_type * \
        get_address() \
        { \
            return static_cast<DerivedT *>(this)->protected_get_address(); \
        } \
    };    \
    \
    template<typename DerivedT, template <typename> class MemberFactoryT> \
    class member_interface_alt { \
    };    \
    \
    template<typename DerivedT, int InterfaceOffsetT, int AlignD1OnIndexT, typename IndexD1T, template <typename, int, int, typename> class ProxyT> \
    class interface2 { \
    public: \
    };    \
    \
    template<typename DerivedT, template <typename> class ProxyT> \
    class aos_interface { \
    public: \
    };    \
    \
    template<int SimdLaneCountT, int ByteAlignmentT = internal::simd_vector_register<BUILTIN_TYPE>::width_in_bytes> \
    struct simd_type \
    : public internal::aligned_base<ByteAlignmentT> \
    { \
      typedef BUILTIN_TYPE primitive_type; \
      static const int simdlane_count = SimdLaneCountT; \
      \
      BUILTIN_TYPE value[SimdLaneCountT]; \
      \
      const BUILTIN_TYPE & operator [] (const int a_simdlane_index) const \
      { \
          return value[a_simdlane_index]; \
      } \
      BUILTIN_TYPE & operator [] (const int a_simdlane_index) \
      { \
          return value[a_simdlane_index]; \
      } \
    }; \
    \
    template<typename DerivedT, \
             typename RootSimdDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT> \
    class const_uniform_interface { \
    public: \
    };    \
    \
    template<typename DerivedT, \
             typename RootSimdDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT, \
             template <typename, typename, typename> class ProxyT> \
    class uniform_interface { \
    public: \
    };    \
    \
    template<typename DerivedT, \
             typename RootScalarDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT> \
    class const_scalar_uniform_interface { \
    public: \
    };    \
    \
    template<typename DerivedT, \
             typename RootSimdDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT, \
             template <typename, typename, typename> class ProxyT> \
    class scalar_uniform_interface { \
    public: \
    };    \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    static    \
    SDLT_INLINE \
    void simd_extract(const RootSimdDataT &a_simd_data, const int a_simdlane_index, BUILTIN_TYPE &an_object); \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    static    \
    SDLT_INLINE \
    void simd_import(RootSimdDataT &a_simd_data, const int a_simdlane_index, const BUILTIN_TYPE &an_object); \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    static    \
    SDLT_INLINE \
    void simd_copy(const RootSimdDataT &a_simd_data, const int a_simdlane_index, RootSimdDataT &oSimdData); \
    \
};  \
\
template <class ElementAgentT>  \
void    \
primitive<BUILTIN_TYPE>::transfer(ElementAgentT & an_element_agent, BUILTIN_TYPE &an_object) { \
    an_element_agent.template move<0>(an_object); \
} \
template<typename RootSimdDataT, \
         typename CompositeAccessT> \
void    \
primitive<BUILTIN_TYPE>::simd_extract(const RootSimdDataT & a_root_simd_data, const int a_simdlane_index, BUILTIN_TYPE &an_object) { \
    __SDLT_ASSERT(a_simdlane_index < RootSimdDataT::simdlane_count); \
    an_object = CompositeAccessT::const_simd_access(a_root_simd_data, a_simdlane_index); \
} \
\
template<typename RootSimdDataT, \
         typename CompositeAccessT> \
void    \
primitive<BUILTIN_TYPE>::simd_import(RootSimdDataT & a_root_simd_data, const int a_simdlane_index, const BUILTIN_TYPE &an_object) { \
    __SDLT_ASSERT(a_simdlane_index < RootSimdDataT::simdlane_count); \
    CompositeAccessT::simd_access(a_root_simd_data, a_simdlane_index) = an_object; \
} \
\
template<typename RootSimdDataT, \
         typename CompositeAccessT> \
void    \
primitive<BUILTIN_TYPE>::simd_copy(const RootSimdDataT & a_root_simd_data, const int a_simdlane_index, RootSimdDataT &oSimdData) { \
    __SDLT_ASSERT(a_simdlane_index < RootSimdDataT::simdlane_count); \
    CompositeAccessT::simd_access(oSimdData, a_simdlane_index) = CompositeAccessT::const_simd_access(a_root_simd_data, a_simdlane_index); \
} \
} /* namepace __SDLT_IMPL */ \
} // namespace sdlt


// NOTE: bool is unsupported, advise to use signed int
// NOTE: char is different type from signed char (int8_t) and 
// unsigned char (uint8_t).  Therefore it must be declared separately
__SDLT_BUILTIN_PRIMITIVE(char)
__SDLT_BUILTIN_PRIMITIVE(uint8_t)
__SDLT_BUILTIN_PRIMITIVE(int8_t)
__SDLT_BUILTIN_PRIMITIVE(uint16_t)
__SDLT_BUILTIN_PRIMITIVE(int16_t)
__SDLT_BUILTIN_PRIMITIVE(uint32_t)
__SDLT_BUILTIN_PRIMITIVE(int32_t)
__SDLT_BUILTIN_PRIMITIVE(uint64_t)
__SDLT_BUILTIN_PRIMITIVE(int64_t)
#if __SDLT_ON_64BIT == 0
// define these types only for 32bit compiler, because these
// types have different meanings on 32bit and and 64bit.
// if define them for 64bit compiler these specializations will collide
// with specializations above
__SDLT_BUILTIN_PRIMITIVE(unsigned long)
__SDLT_BUILTIN_PRIMITIVE(signed long)
#endif // !__SDLT_ON_64BIT

__SDLT_BUILTIN_PRIMITIVE(float)
__SDLT_BUILTIN_PRIMITIVE(double)

#undef __SDLT_BUILTIN_PRIMITIVE


#define __SDLT_LARGEST_BUILTIN_TYPE_FOR_MEMBER(MEMBER_NAME) \
    typename sdlt::primitive<typeof_##MEMBER_NAME>::largest_builtin_type

#define __SDLT_SMALLEST_BUILTIN_TYPE_FOR_MEMBER(MEMBER_NAME) \
    typename sdlt::primitive<typeof_##MEMBER_NAME>::smallest_builtin_type

#define __SDLT_MOVE_PRIMITIVE_MEMBER(MEMBER_NAME) \
    an_element_agent.template move<__SDLT_OFFSETOF(struct_type, MEMBER_NAME)>(an_object.MEMBER_NAME);

#define __SDLT_SIMD_FOR_MEMBER(MEMBER_NAME) \
    typedef decltype(struct_type::MEMBER_NAME) typeof_##MEMBER_NAME; \
    static_assert(std::is_same<bool, typeof_##MEMBER_NAME>::value == false, "The member \"" #MEMBER_NAME "\" is of type \"bool\" which is unsupported for SDLT_PRIMITIVE declarations.  Instead change to a 32 bit integer (int) and set and test explicit values 0 and 1 (or 0xFFFFFFFF)"); \
    static_assert((primitive<typeof_##MEMBER_NAME>::declared || std::is_same<bool, typeof_##MEMBER_NAME>::value), "You must declare an SDLT_PRIMTIVE for the type of the member \"" #MEMBER_NAME "\" before trying to nest inside another SDLT_PRIMITIVE declaration"); \
    \
    static const bool is_builtin_##MEMBER_NAME = internal::is_builtin<typeof_##MEMBER_NAME>::value; \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    struct simd_struct_access_##MEMBER_NAME \
    { \
        static \
        SDLT_INLINE \
        auto const_access(const RootSimdDataT &root)-> const decltype(CompositeAccessT::const_access(root).MEMBER_NAME) & \
        { \
            return CompositeAccessT::const_access(root).MEMBER_NAME; \
        } \
        \
        static \
        SDLT_INLINE \
        auto access(RootSimdDataT &root)-> decltype(CompositeAccessT::access(root).MEMBER_NAME) & \
        { \
            return CompositeAccessT::access(root).MEMBER_NAME; \
        } \
    }; \
    /* can only emit [] accesses for built in types*/ \
    /* will only ever be instantiated against built in types */ \
    /* despite being defined for all member names */ \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    struct simd_builtin_access_##MEMBER_NAME \
    { \
        static \
        SDLT_INLINE \
        typeof_##MEMBER_NAME const_simd_access(const RootSimdDataT &root, const int a_simdlane_index) \
        { \
            return CompositeAccessT::const_access(root).MEMBER_NAME[a_simdlane_index]; \
        } \
        \
        static \
        SDLT_INLINE \
        typeof_##MEMBER_NAME & simd_access(RootSimdDataT &root, const int a_simdlane_index) \
        { \
            return CompositeAccessT::access(root).MEMBER_NAME[a_simdlane_index]; \
        } \
        \
    }; \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    struct simd_##MEMBER_NAME \
    { \
        typedef typename std::conditional<is_builtin_##MEMBER_NAME, \
            simd_builtin_access_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>, \
            simd_struct_access_##MEMBER_NAME<RootSimdDataT, CompositeAccessT> \
            >::type accessor; \
    }; \
    \
    template<typename RootScalarDataT, \
             typename CompositeAccessT> \
    struct scalar_##MEMBER_NAME \
    { \
        static \
        SDLT_INLINE \
        auto const_access(const RootScalarDataT &root)-> const decltype(CompositeAccessT::const_access(root).MEMBER_NAME) & \
        { \
            return CompositeAccessT::const_access(root).MEMBER_NAME; \
        } \
        \
        static \
        SDLT_INLINE \
        auto access(RootScalarDataT &root)-> decltype(CompositeAccessT::access(root).MEMBER_NAME) & \
        { \
            return CompositeAccessT::access(root).MEMBER_NAME; \
        } \
    };


#define __SDLT_EXTRACT_PRIMITIVE_MEMBER(MEMBER_NAME) \
    sdlt::primitive<typeof_##MEMBER_NAME>::template simd_extract< \
        RootSimdDataT, \
        typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor>( \
            a_root_simd_data, \
            a_simdlane_index, \
        an_object.MEMBER_NAME);


#define __SDLT_IMPORT_PRIMITIVE_MEMBER(MEMBER_NAME) \
    sdlt::primitive<typeof_##MEMBER_NAME>::template simd_import< \
        RootSimdDataT, \
        typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor>( \
            a_root_simd_data, \
            a_simdlane_index, \
        an_object.MEMBER_NAME);

#define __SDLT_COPY_PRIMITIVE_MEMBER(MEMBER_NAME) \
    sdlt::primitive<typeof_##MEMBER_NAME>::template simd_copy< \
        RootSimdDataT, \
        typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor>( \
            a_root_simd_data, \
            a_simdlane_index, \
        oRootSimdData);



#define __SDLT_INTERFACE_FOR_MEMBER(MEMBER_NAME) \
private:\
    static const int offsetof_##MEMBER_NAME = __SDLT_OFFSETOF(struct_type, MEMBER_NAME); \
    \
public: \
    ProxyT<typeof_##MEMBER_NAME, InterfaceOffsetT+offsetof_##MEMBER_NAME> \
    MEMBER_NAME() const \
    { \
        return static_cast<const DerivedT *>(this)->template member<typeof_##MEMBER_NAME, offsetof_##MEMBER_NAME>(); \
    }

#define __SDLT_INTERFACE_FOR_MEMBER_ALT(MEMBER_NAME) \
private:\
    static const int offsetof_##MEMBER_NAME = __SDLT_OFFSETOF(struct_type, MEMBER_NAME); \
    \
public: \
    struct broker_for_##MEMBER_NAME \
    { \
        static constexpr int offset_of = offsetof_##MEMBER_NAME; \
        typedef typeof_##MEMBER_NAME type; \
        \
        template<typename T> \
        static SDLT_INLINE const type & \
            const_ref(const T &a_obj) \
        { \
            return a_obj.MEMBER_NAME; \
        } \
        \
        template<typename T> \
        static SDLT_INLINE type & \
            ref(T &a_obj) \
        { \
            return a_obj.MEMBER_NAME; \
        } \
    };\
    \
    auto \
    MEMBER_NAME() const \
    ->decltype(MemberFactoryT<broker_for_##MEMBER_NAME>::template build_member_proxy(std::declval<DerivedT>())) \
/*->decltype(static_cast<const DerivedT *>(this)->template build_member_proxy<broker_for_##MEMBER_NAME>())*/ \
/* ->decltype(std::declval<DerivedT>().template build_member_proxy<broker_for_##MEMBER_NAME>())*/ \
{ \
/*return static_cast<const DerivedT *>(this)->template build_member_proxy<broker_for_##MEMBER_NAME>();*/ \
    return MemberFactoryT<broker_for_##MEMBER_NAME>::template build_member_proxy(*static_cast<const DerivedT *>(this)); \
    }

#define __SDLT_INTERFACE_FOR_MEMBER2(MEMBER_NAME) \
private:\
    static const int offsetof_##MEMBER_NAME = __SDLT_OFFSETOF(struct_type, MEMBER_NAME); \
    \
public: \
    ProxyT<typeof_##MEMBER_NAME, InterfaceOffsetT+offsetof_##MEMBER_NAME, AlignD1OnIndexT, IndexD1T> \
    MEMBER_NAME() const \
    { \
        return static_cast<const DerivedT *>(this)->template member<typeof_##MEMBER_NAME, offsetof_##MEMBER_NAME>(); \
    }

#define __SDLT_INTERFACE_FOR_CONST_UNIFORM_MEMBER(MEMBER_NAME) \
public: \
    ConstProxyT< RootSimdDataT, \
           typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor, \
           typeof_##MEMBER_NAME > \
    MEMBER_NAME() const \
    { \
        return ConstProxyT< RootSimdDataT, \
                      typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor, \
                      typeof_##MEMBER_NAME >( \
                      *static_cast<const DerivedT *>(this)); \
    }

#define __SDLT_INTERFACE_FOR_UNIFORM_MEMBER(MEMBER_NAME) \
    __SDLT_INTERFACE_FOR_CONST_UNIFORM_MEMBER(MEMBER_NAME) \
public: \
    ProxyT<RootSimdDataT, \
           typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor, \
           typeof_##MEMBER_NAME> \
    MEMBER_NAME() \
    { \
        return ProxyT<RootSimdDataT, \
                      typename simd_##MEMBER_NAME<RootSimdDataT, CompositeAccessT>::accessor, \
                      typeof_##MEMBER_NAME >( \
            *static_cast<DerivedT *>(this)); \
    }

#define __SDLT_INTERFACE_FOR_CONST_SCALAR_UNIFORM_MEMBER(MEMBER_NAME) \
public: \
    ConstProxyT< RootScalarDataT, \
           scalar_##MEMBER_NAME<RootScalarDataT, CompositeAccessT>, \
           typeof_##MEMBER_NAME > \
    MEMBER_NAME() const \
    { \
        return ConstProxyT< RootScalarDataT, \
                      scalar_##MEMBER_NAME<RootScalarDataT, CompositeAccessT>, \
                      typeof_##MEMBER_NAME >( \
                      *static_cast<const DerivedT *>(this)); \
    }

#define __SDLT_INTERFACE_FOR_SCALAR_UNIFORM_MEMBER(MEMBER_NAME) \
    __SDLT_INTERFACE_FOR_CONST_SCALAR_UNIFORM_MEMBER(MEMBER_NAME) \
public: \
    ProxyT<RootScalarDataT, \
           scalar_##MEMBER_NAME<RootScalarDataT, CompositeAccessT>, \
           typeof_##MEMBER_NAME> \
    MEMBER_NAME() \
    { \
        return ProxyT<RootScalarDataT, \
                      scalar_##MEMBER_NAME<RootScalarDataT, CompositeAccessT>, \
                      typeof_##MEMBER_NAME >( \
            *static_cast<DerivedT *>(this)); \
    }

#define __SDLT_AOS_INTERFACE_FOR_MEMBER(MEMBER_NAME) \
public: \
    \
    ProxyT<typeof_##MEMBER_NAME> \
    MEMBER_NAME() const \
    { \
        return ProxyT<typeof_##MEMBER_NAME>(static_cast<const DerivedT *>(this)->ref_data().MEMBER_NAME); \
    }

#define __SDLT_SIMD_TYPE_FOR_MEMBER(MEMBER_NAME) \
private:\
    typedef typeof_##MEMBER_NAME array_typeof_##MEMBER_NAME[SimdLaneCountT]; \
    typedef typename std::conditional<internal::is_builtin<typeof_##MEMBER_NAME>::value, \
                array_typeof_##MEMBER_NAME, \
                typename sdlt::primitive<typeof_##MEMBER_NAME>::simd_type<SimdLaneCountT, ByteAlignmentT> \
                >::type simd_member_typeof_##MEMBER_NAME; \
public: \
    \
    __SDLT_STATIC_ASSERT(sizeof(simd_member_typeof_##MEMBER_NAME)%SimdLaneCountT == 0, "alignment problem exists with member \"" #MEMBER_NAME "\""); \
    simd_member_typeof_##MEMBER_NAME MEMBER_NAME;

// Different standard libraries don't support all of type_traits for c++11
#if defined(__GLIBCXX__) && (__GLIBCXX__ == 20120313)
    // Reduced full C++11 library support for specific GCC release
    #define __SDLT_PRIMITIVE_ASSERT_REQUIREMENTS(STRUCT_NAME) \
        __SDLT_STATIC_ASSERT(false == internal::is_builtin<STRUCT_NAME>::value, "SDLT_BUILTIN_PRIMITIVE not declared for a builtin type of" #STRUCT_NAME); \
        __SDLT_STATIC_ASSERT(std::is_class<STRUCT_NAME>::value, #STRUCT_NAME " must be a class or struct type"); \
        __SDLT_STATIC_ASSERT(false == std::has_virtual_destructor<STRUCT_NAME>::value, #STRUCT_NAME " cannot have a virtual desctructor"); \
        __SDLT_STATIC_ASSERT(false == std::is_polymorphic<STRUCT_NAME>::value, #STRUCT_NAME " cannot be polymorphic"); \
        __SDLT_STATIC_ASSERT(false == std::is_abstract<STRUCT_NAME>::value, #STRUCT_NAME " cannot be abstract");
#elif defined(__GLIBCXX__) && (__GLIBCXX__ == 20110509)
    // Reduced full C++11 library support for specific GCC release
    #define __SDLT_PRIMITIVE_ASSERT_REQUIREMENTS(STRUCT_NAME) \
        __SDLT_STATIC_ASSERT(false == internal::is_builtin<STRUCT_NAME>::value, "SDLT_BUILTIN_PRIMITIVE not declared for a builtin type of" #STRUCT_NAME); \
        __SDLT_STATIC_ASSERT(std::is_class<STRUCT_NAME>::value, #STRUCT_NAME " must be a class or struct type"); \
        __SDLT_STATIC_ASSERT(std::is_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be constructible"); \
        __SDLT_STATIC_ASSERT(std::is_default_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be default constructible"); \
        __SDLT_STATIC_ASSERT(std::is_copy_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be copy constructible"); \
        __SDLT_STATIC_ASSERT(std::is_move_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be move constructible"); \
        __SDLT_STATIC_ASSERT(std::is_destructible<STRUCT_NAME>::value, #STRUCT_NAME " must have a no throw destructor"); \
        __SDLT_STATIC_ASSERT(false == std::is_empty<STRUCT_NAME>::value, #STRUCT_NAME " cannot be empty"); \
        __SDLT_STATIC_ASSERT(false == std::has_virtual_destructor<STRUCT_NAME>::value, #STRUCT_NAME " cannot have a virtual desctructor"); \
        __SDLT_STATIC_ASSERT(false == std::is_polymorphic<STRUCT_NAME>::value, #STRUCT_NAME " cannot be polymorphic"); \
        __SDLT_STATIC_ASSERT(false == std::is_abstract<STRUCT_NAME>::value, #STRUCT_NAME " cannot be abstract"); \
        __SDLT_STATIC_ASSERT(std::is_standard_layout<STRUCT_NAME>::value, #STRUCT_NAME " must meet the standard layout requirements");
#else
    // Assume full C++11 library support
    #define __SDLT_PRIMITIVE_ASSERT_REQUIREMENTS(STRUCT_NAME) \
        __SDLT_STATIC_ASSERT(false == internal::is_builtin<STRUCT_NAME>::value, "SDLT_BUILTIN_PRIMITIVE not declared for a builtin type of" #STRUCT_NAME); \
        __SDLT_STATIC_ASSERT(std::is_class<STRUCT_NAME>::value, #STRUCT_NAME " must be a class or struct type"); \
        __SDLT_STATIC_ASSERT(std::is_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be constructible"); \
        __SDLT_STATIC_ASSERT(std::is_default_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be default constructible"); \
        __SDLT_STATIC_ASSERT(std::is_copy_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be copy constructible"); \
        __SDLT_STATIC_ASSERT(std::is_move_constructible<STRUCT_NAME>::value, #STRUCT_NAME " must be move constructible"); \
        __SDLT_STATIC_ASSERT(std::is_copy_assignable<STRUCT_NAME>::value, #STRUCT_NAME " must be copy assignable"); \
        __SDLT_STATIC_ASSERT(std::is_move_assignable<STRUCT_NAME>::value, #STRUCT_NAME " must be move assignable"); \
        __SDLT_STATIC_ASSERT(std::is_destructible<STRUCT_NAME>::value, #STRUCT_NAME " must have a no throw destructor"); \
        __SDLT_STATIC_ASSERT(false == std::is_empty<STRUCT_NAME>::value, #STRUCT_NAME " cannot be empty"); \
        __SDLT_STATIC_ASSERT(false == std::has_virtual_destructor<STRUCT_NAME>::value, #STRUCT_NAME " cannot have a virtual desctructor"); \
        __SDLT_STATIC_ASSERT(false == std::is_polymorphic<STRUCT_NAME>::value, #STRUCT_NAME " cannot be polymorphic"); \
        __SDLT_STATIC_ASSERT(false == std::is_abstract<STRUCT_NAME>::value, #STRUCT_NAME " cannot be abstract"); \
        __SDLT_STATIC_ASSERT(std::is_standard_layout<STRUCT_NAME>::value, #STRUCT_NAME " must meet the standard layout requirements");
#endif


#define __SDLT_PRIMITIVE_INDIRECT(STRUCT_NAME, ...) \
namespace sdlt { \
namespace __SDLT_IMPL { \
template <> \
class primitive<STRUCT_NAME> {   \
    __SDLT_PRIMITIVE_ASSERT_REQUIREMENTS(STRUCT_NAME) \
public: \
    \
    static const bool declared = true; \
    static constexpr bool is_declared() { return true; } \
    \
    typedef STRUCT_NAME struct_type;   \
    \
    static char const * name() { return #STRUCT_NAME; } \
    \
    __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_SIMD_FOR_MEMBER, __VA_ARGS__)) \
    \
    typedef typename internal::template find_largest_builtin< \
        __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH_SEPARATED_BY_COMMA(__SDLT_LARGEST_BUILTIN_TYPE_FOR_MEMBER, __VA_ARGS__)) \
        >::type largest_builtin_type; \
    \
    typedef typename internal::find_smallest_builtin< \
        __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH_SEPARATED_BY_COMMA(__SDLT_SMALLEST_BUILTIN_TYPE_FOR_MEMBER, __VA_ARGS__)) \
        >::type smallest_builtin_type; \
    \
    template<int SimdLaneCountT, int ByteAlignmentT = internal::simd_vector_register<largest_builtin_type>::width_in_bytes> \
    struct simd_type \
    : public internal::aligned_base<ByteAlignmentT> \
    { \
        typedef STRUCT_NAME primitive_type; \
        static int const simdlane_count = SimdLaneCountT;\
        \
        __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_SIMD_TYPE_FOR_MEMBER, __VA_ARGS__)) \
        \
        simd_type() {} \
        simd_type(const simd_type & other) \
        { \
            SDLT_INTEL_PRAGMA("ivdep") \
            for (int simdlane_index=0; simdlane_index < SimdLaneCountT; ++simdlane_index) \
            { \
                sdlt::primitive<primitive_type>::template simd_copy<simd_type, typename internal::simd::root_access<simd_type>::type>(other, simdlane_index, *this); \
            } \
        } \
        \
        simd_type(const simd_type && other) \
        { \
            SDLT_INTEL_PRAGMA("ivdep") \
            for (int simdlane_index=0; simdlane_index < SimdLaneCountT; ++simdlane_index) \
            { \
                sdlt::primitive<primitive_type>::template simd_copy<simd_type, typename internal::simd::root_access<simd_type>::type>(other, simdlane_index, *this); \
            } \
        } \
        \
        simd_type & operator = (const simd_type & other) \
        { \
            SDLT_INTEL_PRAGMA("ivdep") \
            for (int simdlane_index=0; simdlane_index < SimdLaneCountT; ++simdlane_index) \
            { \
                sdlt::primitive<primitive_type>::template simd_copy<simd_type, typename internal::simd::root_access<simd_type>::type>(other, simdlane_index, *this); \
            } \
            return *this; \
        } \
        \
        simd_type & operator = (const simd_type && other) \
        { \
            SDLT_INTEL_PRAGMA("ivdep") \
            for (int simdlane_index=0; simdlane_index < SimdLaneCountT; ++simdlane_index) \
            { \
                sdlt::primitive<primitive_type>::template simd_copy<simd_type, typename internal::simd::root_access<simd_type>::type>(other, simdlane_index, *this); \
            } \
            return *this; \
        } \
        \
        \
        internal::simd::const_element<simd_type> operator[] (const int a_simdlane_index) const\
        { \
            return internal::simd::const_element<simd_type>(*this, a_simdlane_index); \
        } \
        \
        internal::simd::element<simd_type> operator[] (const int a_simdlane_index) \
        { \
            return internal::simd::element<simd_type>(*this, a_simdlane_index); \
        } \
    }; \
    \
    template<typename DerivedT, \
             typename RootSimdDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT> \
    class const_uniform_interface { \
    public: \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_CONST_UNIFORM_MEMBER, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, \
             typename RootScalarDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT> \
    class const_scalar_uniform_interface { \
    public: \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_CONST_SCALAR_UNIFORM_MEMBER, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, \
             typename RootSimdDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT, \
             template <typename, typename, typename> class ProxyT> \
    class uniform_interface { \
    public: \
      typedef CompositeAccessT composite_access_type; \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_UNIFORM_MEMBER, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, \
             typename RootScalarDataT, \
             typename CompositeAccessT, \
             template <typename, typename, typename> class ConstProxyT, \
             template <typename, typename, typename> class ProxyT> \
     class scalar_uniform_interface  { \
    public: \
      typedef CompositeAccessT composite_access_type; \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_SCALAR_UNIFORM_MEMBER, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, int InterfaceOffsetT, template <typename, int> class ProxyT> \
    class member_interface { \
    public: \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_MEMBER, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, template <typename> class MemberFactoryT> \
    class member_interface_alt { \
    public: \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_MEMBER_ALT, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, int InterfaceOffsetT, int AlignD1OnIndexT, typename IndexD1T, template <typename, int, int, typename> class ProxyT> \
    class interface2 { \
    public: \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_INTERFACE_FOR_MEMBER2, __VA_ARGS__)) \
    };    \
    \
    template<typename DerivedT, template <typename> class ProxyT> \
    class aos_interface { \
    public: \
      __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_AOS_INTERFACE_FOR_MEMBER, __VA_ARGS__)) \
    };    \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    static    \
    SDLT_INLINE \
    void simd_extract(const RootSimdDataT &a_simd_data, const int a_simdlane_index, STRUCT_NAME &an_object); \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    static    \
    SDLT_INLINE \
    void simd_import(RootSimdDataT &a_simd_data, const int a_simdlane_index, const STRUCT_NAME &an_object); \
    \
    template<typename RootSimdDataT, \
             typename CompositeAccessT> \
    static    \
    SDLT_INLINE \
    void simd_copy(const RootSimdDataT &a_simd_data, const int a_simdlane_index, RootSimdDataT &oSimdData); \
    \
    template <class ElementAgentT>  \
    static    \
    SDLT_INLINE \
    void transfer(ElementAgentT & an_element_agent, STRUCT_NAME &an_object); \
};  \
\
template <class ElementAgentT>  \
void    \
primitive<STRUCT_NAME>::transfer(ElementAgentT & an_element_agent, STRUCT_NAME &an_object) { \
    __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_MOVE_PRIMITIVE_MEMBER, __VA_ARGS__)) \
} \
template<typename RootSimdDataT, \
         typename CompositeAccessT> \
void    \
primitive<STRUCT_NAME>::simd_extract(const RootSimdDataT & a_root_simd_data, const int a_simdlane_index, STRUCT_NAME &an_object) { \
    __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_EXTRACT_PRIMITIVE_MEMBER, __VA_ARGS__)) \
} \
\
template<typename RootSimdDataT, \
         typename CompositeAccessT> \
void    \
primitive<STRUCT_NAME>::simd_import(RootSimdDataT & a_root_simd_data, const int a_simdlane_index, const STRUCT_NAME &an_object) { \
    __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_IMPORT_PRIMITIVE_MEMBER, __VA_ARGS__)) \
} \
\
template<typename RootSimdDataT, \
         typename CompositeAccessT> \
void    \
primitive<STRUCT_NAME>::simd_copy(const RootSimdDataT & a_root_simd_data, const int a_simdlane_index, RootSimdDataT &oRootSimdData) { \
    __SDLT_EXPAND_TOKEN(__SDLT_EMIT_FOR_EACH(__SDLT_COPY_PRIMITIVE_MEMBER, __VA_ARGS__)) \
} \
\
} /* namespace __SDLT_IMPL */ \
} // namespace sdlt




#define SDLT_PRIMITIVE_FRIEND template <typename> friend class sdlt::primitive;

#define SDLT_PRIMITIVE(STRUCT_NAME, ...) __SDLT_EXPAND_TOKEN(__SDLT_PRIMITIVE_INDIRECT(STRUCT_NAME, __VA_ARGS__))


#endif // SDLT_PRIMITIVE_H
