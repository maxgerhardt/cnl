
//          Copyright John McFarlane 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_OVERFLOW_TRAPPING_H)
#define CNL_IMPL_OVERFLOW_TRAPPING_H

#include "common.h"
#include "overflow_operator.h"
#include "../terminate.h"

/// compositional numeric library
namespace cnl {
    // terminate program with diagnostic when overflow is detected
    struct trapping_overflow_tag {
    };

    namespace _impl {
        template<typename Operator>
        struct overflow_operator<Operator, trapping_overflow_tag, polarity::positive> {
            template<typename Destination, typename Source>
            constexpr Destination operator()(Source const&) const
            {
                return terminate<Destination>("positive overflow");
            }

            template<class ... Operands>
            constexpr op_result<Operator, Operands...> operator()(Operands const&...) const
            {
                return terminate<op_result<Operator, Operands...>>("positive overflow");
            }
        };

        template<typename Operator>
        struct overflow_operator<Operator, trapping_overflow_tag, polarity::negative> {
            template<typename Destination, typename Source>
            constexpr Destination operator()(Source const&) const
            {
                return terminate<Destination>("negative overflow");
            }

            template<class ... Operands>
            constexpr op_result<Operator, Operands...> operator()(Operands const&...) const
            {
                return terminate<op_result<Operator, Operands...>>("negative overflow");
            }
        };

        template<typename Destination, typename Source>
        struct tagged_convert_operator<trapping_overflow_tag, Destination, Source>
                : tagged_convert_overflow_operator<trapping_overflow_tag, Destination, Source> {
        };

        template<class Operator>
        struct tagged_unary_operator<trapping_overflow_tag, Operator>
                : tagged_unary_overflow_operator<trapping_overflow_tag, Operator> {
        };

        template<class Operator>
        struct tagged_binary_operator<trapping_overflow_tag, Operator>
                : tagged_binary_overflow_operator<trapping_overflow_tag, Operator> {
        };
    }
}

#endif  // CNL_IMPL_OVERFLOW_TRAPPING_H
