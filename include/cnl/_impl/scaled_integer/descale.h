
//          Copyright John McFarlane 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file
/// \brief definition of `cnl::_impl::descale`

#if !defined(CNL_IMPL_SCALED_INTEGER_DESCALE_H)
#define CNL_IMPL_SCALED_INTEGER_DESCALE_H

#include "../../integer.h"
#include "../../wide_integer.h"
#include "../cnl_assert.h"
#include "../numbers/adopt_signedness.h"
#include "../power_value.h"
#include "../scaled/power.h"
#include "../used_digits.h"
#include "../wrapper/to_rep.h"
#include "definition.h"
#include "extras.h"
#include "num_traits.h"

namespace cnl::_impl {
    template<integer Rep, int Radix>
    struct descaled {
        [[nodiscard]] friend auto operator<=>(descaled const&, descaled const&) = default;

        Rep significand;
        int exponent;
    };

    // requires that OutRep is of sufficient capacity
    template<integer OutRep, int OutRadix, integer InRep, int InExponent, int InRadix>
    [[nodiscard]] constexpr auto descale(scaled_integer<InRep, power<InExponent, InRadix>> const& input)
    {
        descaled<OutRep, OutRadix> output{_impl::to_rep(input), 0};

        auto const oob{
                (input < 0.)
                ? []([[maybe_unused]] OutRep const& n) -> bool {
                      if constexpr (numbers::signedness_v<InRep>) {
                          return n < OutRep{-numeric_limits<InRep>::max()};
                      } else {
                          CNL_ASSERT(false);
                      }
                  }
                : [](OutRep const& n) {
                      return n > OutRep{numeric_limits<InRep>::max()};
                  }};

        if constexpr (InExponent < 0) {
            for (int in_exponent = InExponent; in_exponent != 0;) {
                if (output.significand % InRadix) {
                    // Some combinations of radixes guarantee lossless conversion,
                    // e.g. binary -> decimal. They require less 'supervision'.
                    if (oob(output.significand)) {
                        auto const might_be_oob{InRadix % OutRadix};
                        CNL_ASSERT(might_be_oob);
                    } else {
                        output.significand *= OutRadix;
                        output.exponent--;
                    }
                }

                output.significand /= InRadix;
                in_exponent++;
            }
        } else if constexpr (InExponent > 0) {
            for (int in_exponent = InExponent; in_exponent != 0;) {
                // Some combinations of radixes guarantee lossless conversion,
                // e.g. binary -> decimal. They require less 'supervision'.
                if (oob(output.significand)) {
                    auto const might_be_oob{OutRadix % InRadix};
                    CNL_ASSERT(might_be_oob);
                } else {
                    output.significand *= InRadix;
                    in_exponent--;
                }

                if (!(output.significand % OutRadix)) {
                    output.significand /= OutRadix;
                    output.exponent++;
                }
            }
        }

        return output;
    }

    template<int OutRadix, integer Rep, int Exponent, int Radix>
    [[nodiscard]] constexpr auto descale(scaled_integer<Rep, power<Exponent, Radix>> const& input)
    {
        // TODO: there are other cases where Rep can losslessly represent significand
        if constexpr (Exponent == 0) {
            return descaled<Rep, OutRadix>{_impl::to_rep(input), 0};
        }

        // Some scaling up and down is needed to get from, e.g. binary to decimal.
        // That takes up more capacity than is available in the input.
        constexpr auto power1{_impl::power_value<Rep, 1, OutRadix>()};
        constexpr auto room_to_grow{used_digits(power1)};  // power1-1 ?
        constexpr auto digits_needed = digits<Rep> + room_to_grow;

        using promoted_rep = wide_integer<digits_needed, _impl::adopt_signedness_t<int, Rep>>;
        return _impl::descale<promoted_rep, OutRadix>(input);
    }
}

#endif  // CNL_IMPL_SCALED_INTEGER_DESCALE_H
