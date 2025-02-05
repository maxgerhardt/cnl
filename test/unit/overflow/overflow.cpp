
//          Copyright John McFarlane 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cnl/_impl/type_traits/identical.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4308)
#endif
#include <cnl/overflow.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <gtest/gtest.h>

namespace {
    using cnl::_impl::identical;

    namespace test_native_overflow {

        // convert
        static_assert(
                identical(
                        cnl::uint8{3},
                        cnl::convert<cnl::native_overflow_tag, cnl::_impl::native_tag, cnl::uint8>(
                                259)),
                "cnl::convert test failed");
        static_assert(
                identical(
                        cnl::uint16{65413},
                        cnl::convert<cnl::native_overflow_tag, cnl::_impl::native_tag, cnl::uint16>(
                                -123)),
                "cnl::convert test failed");
        static_assert(
                identical(
                        55,
                        cnl::convert<cnl::native_overflow_tag, cnl::_impl::native_tag, cnl::int32>(
                                55)),
                "cnl::convert test failed");

        // add
        static_assert(
                identical(
                        cnl::add<cnl::native_overflow_tag>(
                                UINT32_C(0xFFFFFFFF), UINT32_C(0x12345678)),
                        UINT32_C(0xFFFFFFFF) + UINT32_C(0x12345678)),
                "cnl::add test failed");

        // subtract
        static_assert(
                identical(
                        cnl::custom_operator<
                                cnl::_impl::subtract_op,
                                cnl::op_value<cnl::int8, cnl::native_overflow_tag>,
                                cnl::op_value<cnl::int8, cnl::native_overflow_tag>>()(
                                INT8_C(0), INT8_C(0)),
                        0),
                "cnl::subtract test failed");
        static_assert(
                identical(cnl::subtract<cnl::native_overflow_tag>(INT8_C(0), INT8_C(0)), 0),
                "cnl::subtract test failed");

        // multiply
        static_assert(
                identical(
                        cnl::multiply<cnl::native_overflow_tag>(UINT16_C(576), INT32_C(22)),
                        decltype(UINT16_C(576) * INT32_C(22)){12672}),
                "cnl::multiply test failed");
    }

    namespace test_throwing_overflow {
        // subtract
        static_assert(
                identical(cnl::subtract<cnl::throwing_overflow_tag>(INT8_C(0), INT8_C(0)), 0),
                "cnl::subtract test failed");

        // multiply
        static_assert(
                identical(
                        cnl::multiply<cnl::throwing_overflow_tag>(UINT16_C(576), INT32_C(22)),
                        decltype(UINT16_C(576) * INT32_C(22)){12672}),
                "cnl::add test failed");
    }

    namespace test_saturated {
        // convert
        static_assert(
                identical(
                        cnl::uint8{255},
                        cnl::convert<
                                cnl::saturated_overflow_tag, cnl::_impl::native_tag, cnl::uint8>(
                                259)),
                "cnl::convert test failed");
        static_assert(
                identical(
                        cnl::uint16{0},
                        cnl::convert<
                                cnl::saturated_overflow_tag, cnl::_impl::native_tag, cnl::uint16>(
                                -123)),
                "cnl::convert test failed");
        static_assert(
                identical(
                        55,
                        cnl::convert<
                                cnl::saturated_overflow_tag, cnl::_impl::native_tag, cnl::int32>(
                                55)),
                "cnl::convert test failed");

        // add
        static_assert(
                identical(
                        cnl::custom_operator<
                                cnl::_impl::add_op,
                                cnl::op_value<signed, cnl::saturated_overflow_tag>,
                                cnl::op_value<unsigned, cnl::saturated_overflow_tag>>()(7, 23U),
                        7 + 23U));
        static_assert(
                identical(
                        std::numeric_limits<
                                decltype(UINT32_C(0xFFFFFFFF) + INT32_C(0x12345678))>::max(),
                        cnl::add<cnl::saturated_overflow_tag>(
                                UINT32_C(0xFFFFFFFF), INT32_C(0x12345678))),
                "cnl::add test failed");
        static_assert(
                identical(
                        cnl::numeric_limits<decltype(2 + 2U)>::max(),
                        cnl::add<cnl::saturated_overflow_tag>(
                                2, cnl::numeric_limits<unsigned>::max())),
                "cnl::add test failed");

        // subtract
        static_assert(
                identical(cnl::subtract<cnl::saturated_overflow_tag>(INT8_C(0), INT8_C(0)), 0),
                "cnl::subtract test failed");
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4308)
#endif
        static_assert(
                identical(cnl::subtract<cnl::saturated_overflow_tag>(0U, -1), 1U),
                "cnl::subtract test failed");
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
        static_assert(
                identical(
                        cnl::numeric_limits<int>::max(),
                        cnl::subtract<cnl::saturated_overflow_tag>(
                                0, cnl::numeric_limits<int>::min())));

        // multiply
        static_assert(
                identical(
                        cnl::multiply<cnl::saturated_overflow_tag>(UINT16_C(576), INT32_C(22)),
                        decltype(UINT16_C(576) * INT32_C(22)){12672}),
                "cnl::multiply test failed");
        static_assert(
                identical(
                        cnl::multiply<cnl::saturated_overflow_tag>(
                                cnl::numeric_limits<int32_t>::max(), INT32_C(2)),
                        cnl::numeric_limits<int32_t>::max()),
                "cnl::multiply test failed");

        // compare
        static_assert(
                identical(
                        cnl::numeric_limits<short>::max(),
                        cnl::convert<cnl::saturated_overflow_tag, cnl::_impl::native_tag, short>(
                                cnl::numeric_limits<double>::max())),
                "cnl::convert test failed");
        static_assert(
                identical(
                        cnl::numeric_limits<short>::lowest(),
                        cnl::convert<cnl::saturated_overflow_tag, cnl::_impl::native_tag, short>(
                                cnl::numeric_limits<double>::lowest())),
                "cnl::convert test failed");

        // shift_left
        static_assert(
                identical(
                        cnl::numeric_limits<cnl::int16>::max() << 1,
                        cnl::shift_left<cnl::saturated_overflow_tag>(
                                cnl::numeric_limits<cnl::int16>::max(), 1)),
                "cnl::shift_left test failed");
        static_assert(
                identical(
                        cnl::numeric_limits<cnl::int32>::max(),
                        cnl::shift_left<cnl::saturated_overflow_tag>(
                                cnl::numeric_limits<cnl::int32>::max(), 1)),
                "cnl::shift_left test failed");
        static_assert(
                identical(
                        cnl::numeric_limits<int>::max(),
                        cnl::custom_operator<
                                cnl::_impl::shift_left_op,
                                cnl::op_value<std::uint8_t, cnl::saturated_overflow_tag>,
                                cnl::op_value<unsigned, cnl::saturated_overflow_tag>>{}(
                                std::uint8_t{255}, 30U)));
    }

    namespace test_negative_shift_left {
#if !defined(CNL_UNREACHABLE_UB_ENABLED)
        TEST(overflow, trap)  // NOLINT
        {
            ASSERT_DEATH(
                    (void)cnl::shift_left<cnl::trapping_overflow_tag>(-1073741825, 1),
                    "negative overflow");
        }
#endif
    }
}
