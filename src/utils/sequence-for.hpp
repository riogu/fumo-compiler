#ifndef MACRO_SEQUENCE_FOR_H_ // Intentionally not a `#pragma once`, to be able to tolerate multiple copies of the file.
#define MACRO_SEQUENCE_FOR_H_

// "macro_sequence_for.h", v0.3
// Implements macro loops with unlimited number of iterations, over sequences of the form `(a)(b)(c)` (though the nesting level is limited).
// See `SF_FOR_EACH()` below for the usage explanation.

// BSD Zero Clause License
//
// Copyright (c) 2024 Egor Mikhailov
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

// A loop over a sequence of the form `(a)(b)(c)`. The elements can contain commas.
// `m` is the loop body, invoked as `m(n, d, ...)`, where `...` is one of the elements,
// `d` initially matches the argument `d` of `SF_FOR_EACH`, but is replaced with
//   the result of `s(n, d, ...)` after every iteration.
// After the loop finishes, `f(n, d)` is called with the final value of `d`.

// Additionally, if `s` returns something with a `,`, everything after the first comma
//   gets pasted to the output after `m(...)` on the same iteration. This is sometimes
//   useful in complex macros, where you'd otherwise repeat the same computation
//   in both `m` and `s`.
// But note that while `m` can return absolutely anything, `s` can't return mismatched parentheses.
//
// The `n` argument receives the next available loop nesting level. `SF_FOR_EACH`
//   sets it to `0`, while `SF_FOR_EACHi` sets it to `i+1`.
// Use `SF_FOR_EACHi(...)` for nested loops, where `i` can either be hardcoded,
//   or come from the `n` argument of the outer loop, in which case you
//   can use `SF_CAT(SF_FOR_EACH, n)(...)`.
#define SF_FOR_EACH(m, s, f, d, seq) IMPL_SEQFOR_FOR(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )

// Various useful macros, to be passed as arguments to `SF_FOR_EACH`.
#define SF_NULL(...) // A no-op.
#define SF_STATE(...) IMPL_SEQFOR_STATE(__VA_ARGS__,) // Returns the second argument. As the step and/or final function, returns the state unchanged.

// Concatenate two identifiers.
#define SF_CAT(x, y) SF_CAT_(x, y)
#define SF_CAT_(x, y) x##y

// The max loop nesting level. This is baked in at the code generation time, adjust it in `generate.sh`.
// The actual number of supporting nesting levels is `N+1`, with name suffixes ``, `0`, `1`, ..., `N-1`.
// The most nested loop will report `N` as the next available level.
#define SF_MAX_DEPTH 8

// Implementation:

#if defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL == 1)
    #error The standard-conformant MSVC preprocessor is required, enable it with `/Zc:preprocessor`.
#endif

// Same as `SF_FOR_EACH`, but the list is spelled differently, as described in `IMPL_SEQFOR_FOR_GUIDE_A`.
#define IMPL_SEQFOR_FOR(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY( IMPL_SEQFOR_FOR_GUIDE_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )

// Various helpers.
#define IMPL_SEQFOR_NULL(...)
#define IMPL_SEQFOR_IDENTITY(...) __VA_ARGS__
#define IMPL_SEQFOR_LPAREN (
#define IMPL_SEQFOR_CAT(x, y) IMPL_SEQFOR_CAT_(x,y)
#define IMPL_SEQFOR_CAT_(x, y) x##y
// Implementation of `SF_STATE`.
#define IMPL_SEQFOR_STATE(n, d, ...) d

// `IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)` converts `seq` from e.g. `(a)(b)(c)` to `()(a)()(b)()(c)(0)`.
#define IMPL_SEQFOR_ANNOTATE_SEQ_A(...) ()(__VA_ARGS__)IMPL_SEQFOR_ANNOTATE_SEQ_B
#define IMPL_SEQFOR_ANNOTATE_SEQ_B(...) ()(__VA_ARGS__)IMPL_SEQFOR_ANNOTATE_SEQ_A
#define IMPL_SEQFOR_ANNOTATE_SEQ_A_END
#define IMPL_SEQFOR_ANNOTATE_SEQ_B_END
#define IMPL_SEQFOR_ANNOTATE_SEQ_END(x) IMPL_SEQFOR_ANNOTATE_SEQ_END_(x)(0)
#define IMPL_SEQFOR_ANNOTATE_SEQ_END_(x) x##_END

// `IMPL_SEQFOR_TO_GUIDE_A seq` converts `seq` from e.g. `(a)(b)(c)` to `a)b)c)IMPL_SEQFOR_TO_GUIDE_A` (or ...`_B`).
// We call the resulting sequence `a)b)c)` a "guide".
#define IMPL_SEQFOR_TO_GUIDE_A(...) __VA_ARGS__)IMPL_SEQFOR_TO_GUIDE_B
#define IMPL_SEQFOR_TO_GUIDE_B(...) __VA_ARGS__)IMPL_SEQFOR_TO_GUIDE_A

// Usage: `IMPL_SEQFOR_FOR_GUIDE_A(m,s,d,`, followed by a sequence of 0+ blocks of the form `)x)`, followed by `0)`, finally followed by arbitrary junk.
// For each `)x)` in the sequence, expands to `m(r,d,x)`, then modifies `d` to contain `s(r,d,x)`, where `r` is the next available loop nesting level.
#define IMPL_SEQFOR_FOR_GUIDE_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE_A_0(m, s, f, d) f(0, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE_B(m, s, f, d, ...) m(0, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE_B_0(m, s, f, s(0, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE_B_0(...) IMPL_SEQFOR_FOR_GUIDE_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE_A(m, s, f, d,

// Generated boilerplate for nested loops:
// 0
#define SF_FOR_EACH0(m, s, f, d, seq) IMPL_SEQFOR_FOR0(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR0(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY0( IMPL_SEQFOR_FOR_GUIDE0_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY0(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE0_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE0_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE0_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE0_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE0_A_0(m, s, f, d) f(1, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE0_B(m, s, f, d, ...) m(1, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE0_B_0(m, s, f, s(1, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE0_B_0(...) IMPL_SEQFOR_FOR_GUIDE0_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE0_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE0_A(m, s, f, d,
// 1
#define SF_FOR_EACH1(m, s, f, d, seq) IMPL_SEQFOR_FOR1(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR1(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY1( IMPL_SEQFOR_FOR_GUIDE1_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY1(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE1_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE1_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE1_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE1_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE1_A_0(m, s, f, d) f(2, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE1_B(m, s, f, d, ...) m(2, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE1_B_0(m, s, f, s(2, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE1_B_0(...) IMPL_SEQFOR_FOR_GUIDE1_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE1_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE1_A(m, s, f, d,
// 2
#define SF_FOR_EACH2(m, s, f, d, seq) IMPL_SEQFOR_FOR2(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR2(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY2( IMPL_SEQFOR_FOR_GUIDE2_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY2(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE2_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE2_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE2_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE2_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE2_A_0(m, s, f, d) f(3, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE2_B(m, s, f, d, ...) m(3, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE2_B_0(m, s, f, s(3, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE2_B_0(...) IMPL_SEQFOR_FOR_GUIDE2_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE2_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE2_A(m, s, f, d,
// 3
#define SF_FOR_EACH3(m, s, f, d, seq) IMPL_SEQFOR_FOR3(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR3(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY3( IMPL_SEQFOR_FOR_GUIDE3_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY3(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE3_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE3_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE3_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE3_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE3_A_0(m, s, f, d) f(4, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE3_B(m, s, f, d, ...) m(4, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE3_B_0(m, s, f, s(4, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE3_B_0(...) IMPL_SEQFOR_FOR_GUIDE3_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE3_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE3_A(m, s, f, d,
// 4
#define SF_FOR_EACH4(m, s, f, d, seq) IMPL_SEQFOR_FOR4(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR4(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY4( IMPL_SEQFOR_FOR_GUIDE4_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY4(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE4_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE4_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE4_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE4_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE4_A_0(m, s, f, d) f(5, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE4_B(m, s, f, d, ...) m(5, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE4_B_0(m, s, f, s(5, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE4_B_0(...) IMPL_SEQFOR_FOR_GUIDE4_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE4_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE4_A(m, s, f, d,
// 5
#define SF_FOR_EACH5(m, s, f, d, seq) IMPL_SEQFOR_FOR5(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR5(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY5( IMPL_SEQFOR_FOR_GUIDE5_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY5(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE5_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE5_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE5_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE5_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE5_A_0(m, s, f, d) f(6, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE5_B(m, s, f, d, ...) m(6, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE5_B_0(m, s, f, s(6, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE5_B_0(...) IMPL_SEQFOR_FOR_GUIDE5_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE5_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE5_A(m, s, f, d,
// 6
#define SF_FOR_EACH6(m, s, f, d, seq) IMPL_SEQFOR_FOR6(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR6(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY6( IMPL_SEQFOR_FOR_GUIDE6_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY6(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE6_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE6_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE6_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE6_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE6_A_0(m, s, f, d) f(7, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE6_B(m, s, f, d, ...) m(7, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE6_B_0(m, s, f, s(7, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE6_B_0(...) IMPL_SEQFOR_FOR_GUIDE6_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE6_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE6_A(m, s, f, d,
// 7
#define SF_FOR_EACH7(m, s, f, d, seq) IMPL_SEQFOR_FOR7(m, s, f, d, IMPL_SEQFOR_TO_GUIDE_A IMPL_SEQFOR_ANNOTATE_SEQ_END(IMPL_SEQFOR_ANNOTATE_SEQ_A seq)) )
#define IMPL_SEQFOR_FOR7(m, s, f, d, guide) IMPL_SEQFOR_IDENTITY7( IMPL_SEQFOR_FOR_GUIDE7_A IMPL_SEQFOR_LPAREN m, s, f, d, guide )
#define IMPL_SEQFOR_IDENTITY7(...) __VA_ARGS__
#define IMPL_SEQFOR_FOR_GUIDE7_A(m, s, f, d, e) IMPL_SEQFOR_CAT(IMPL_SEQFOR_FOR_GUIDE7_A_, e)(m, s, f, d)
#define IMPL_SEQFOR_FOR_GUIDE7_A_(m, s, f, d) IMPL_SEQFOR_FOR_GUIDE7_B(m, s, f, d,
#define IMPL_SEQFOR_FOR_GUIDE7_A_0(m, s, f, d) f(8, d) IMPL_SEQFOR_NULL(
#define IMPL_SEQFOR_FOR_GUIDE7_B(m, s, f, d, ...) m(8, d, __VA_ARGS__) IMPL_SEQFOR_FOR_GUIDE7_B_0(m, s, f, s(8, d, __VA_ARGS__))
#define IMPL_SEQFOR_FOR_GUIDE7_B_0(...) IMPL_SEQFOR_FOR_GUIDE7_B_1(__VA_ARGS__)
#define IMPL_SEQFOR_FOR_GUIDE7_B_1(m, s, f, d, ...) __VA_ARGS__ IMPL_SEQFOR_FOR_GUIDE7_A(m, s, f, d,

#endif
