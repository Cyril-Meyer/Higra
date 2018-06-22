/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XSIMD_FALLBACK_HPP
#define XSIMD_FALLBACK_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

#include "xsimd_base.hpp"
#include "xsimd_utils.hpp"

namespace xsimd
{

    /***********************************************************
     * Generic fallback implementation of batch and batch_bool *
     *                                                         *
     * Basically, generate a scalar loop and cross fingers:    *
     * maybe the compiler will autovectorize, maybe not.       *
     ***********************************************************/

    /********************
     * batch_bool<T, N> *
     ********************/

    template <typename T, std::size_t N>
    struct simd_batch_traits<batch_bool<T, N>>
    {
        using value_type = T;
        static constexpr std::size_t size = N;
        using batch_type = batch<T, N>;
        static constexpr std::size_t align = XSIMD_DEFAULT_ALIGNMENT;
    };

    template <typename T, std::size_t N>
    class batch_bool : public simd_batch_bool<batch_bool<T, N>>
    {
    public:

        batch_bool();
        explicit batch_bool(bool b);

        // Constructor from N boolean parameters
        template <
            typename... Args,
            typename Enable = detail::is_array_initializer_t<bool, N, Args...>
        >
        batch_bool(Args... exactly_N_bools);

        batch_bool(const std::array<bool, N>& rhs);
        batch_bool& operator=(const std::array<bool, N>& rhs);

        operator std::array<bool, N>() const;

        const bool& operator[](std::size_t index) const;
        bool& operator[](std::size_t index);

    private:

        std::array<bool, N> m_value;
    };

    template <typename T, std::size_t N>
    batch_bool<T, N> operator&(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator|(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator^(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator~(const batch_bool<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> bitwise_andnot(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs);

    template <typename T, std::size_t N>
    batch_bool<T, N> operator==(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator!=(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs);

    template <typename T, std::size_t N>
    bool all(const batch_bool<T, N>& rhs);
    template <typename T, std::size_t N>
    bool any(const batch_bool<T, N>& rhs);

    /***************
     * batch<T, N> *
     ***************/

    template <typename T, std::size_t N>
    struct simd_batch_traits<batch<T, N>>
    {
        using value_type = T;
        static constexpr std::size_t size = N;
        using batch_bool_type = batch_bool<T, N>;
        static constexpr std::size_t align = XSIMD_DEFAULT_ALIGNMENT;
    };

    template <typename T, std::size_t N>
    class batch : public simd_batch<batch<T, N>>
    {
    public:

        batch();
        explicit batch(T f);

        // Constructor from N scalar parameters
        template <
            typename... Args,
            typename Enable = typename detail::is_array_initializer<T, N, Args...>::type
        >
        batch(Args... exactly_N_scalars);

        explicit batch(const T* src);
        batch(const T* src, aligned_mode);
        batch(const T* src, unaligned_mode);
        batch(const std::array<T, N>& rhs);
        batch& operator=(const std::array<T, N>& rhs);

        operator std::array<T, N>() const;

        batch& load_aligned(const float* src);
        batch& load_unaligned(const float* src);

        batch& load_aligned(const double* src);
        batch& load_unaligned(const double* src);

        batch& load_aligned(const int32_t* src);
        batch& load_unaligned(const int32_t* src);

        batch& load_aligned(const int64_t* src);
        batch& load_unaligned(const int64_t* src);

        void store_aligned(float* dst) const;
        void store_unaligned(float* dst) const;

        void store_aligned(double* dst) const;
        void store_unaligned(double* dst) const;

        void store_aligned(int32_t* dst) const;
        void store_unaligned(int32_t* dst) const;

        void store_aligned(int64_t* dst) const;
        void store_unaligned(int64_t* dst) const;

        const T& operator[](std::size_t index) const;
        T& operator[](std::size_t index);


    private:

        template<typename U>
        batch& load_unaligned_impl(const U* src);
        template<typename U>
        void store_unaligned_impl(U* src) const;

        std::array<T, N> m_value;
    };

    template <typename T, std::size_t N>
    batch<T, N> operator-(const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator+(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator-(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator*(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator/(const batch<T, N>& lhs, const batch<T, N>& rhs);

    template <typename T, std::size_t N>
    batch_bool<T, N> operator==(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator!=(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator<(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch_bool<T, N> operator<=(const batch<T, N>& lhs, const batch<T, N>& rhs);

    template <typename T, std::size_t N>
    batch<T, N> operator&(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator|(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator^(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator~(const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> bitwise_andnot(const batch<T, N>& lhs, const batch<T, N>& rhs);

    template <typename T, std::size_t N>
    batch<T, N> min(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> max(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> fmin(const batch<T, N>& lhs, const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> fmax(const batch<T, N>& lhs, const batch<T, N>& rhs);

    template <typename T, std::size_t N>
    batch<T, N> abs(const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> fabs(const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> sqrt(const batch<T, N>& rhs);

    template <typename T, std::size_t N>
    batch<T, N> fma(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z);
    template <typename T, std::size_t N>
    batch<T, N> fms(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z);
    template <typename T, std::size_t N>
    batch<T, N> fnma(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z);
    template <typename T, std::size_t N>
    batch<T, N> fnms(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z);

    template <typename T, std::size_t N>
    T hadd(const batch<T, N>& rhs);
    template <typename T, std::size_t N>
    batch<T, N> haddp(const batch<T, N>* row);

    template <typename T, std::size_t N>
    batch<T, N> select(const batch_bool<T, N>& cond, const batch<T, N>& a, const batch<T, N>& b);

    template <typename T, std::size_t N>
    batch_bool<T, N> isnan(const batch<T, N>& x);

    template <typename T, std::size_t N>
    batch<T, N> operator<<(const batch<T, N>& lhs, int32_t rhs);
    template <typename T, std::size_t N>
    batch<T, N> operator>>(const batch<T, N>& lhs, int32_t rhs);

    /************************
     * conversion functions *
     ************************/

    template <std::size_t N>
    batch<int32_t, N> to_int(const batch<float, N>& x);
    template <std::size_t N>
    batch<int64_t, N> to_int(const batch<double, N>& x);

    template <std::size_t N>
    batch<float, N> to_float(const batch<int32_t, N>& x);
    template <std::size_t N>
    batch<double, N> to_float(const batch<int64_t, N>& x);

    /**************************
     * boolean cast functions *
     **************************/

    template <std::size_t N>
    batch_bool<int32_t, N> bool_cast(const batch_bool<float, N>& x);
    template <std::size_t N>
    batch_bool<int64_t, N> bool_cast(const batch_bool<double, N>& x);
    template <std::size_t N>
    batch_bool<float, N> bool_cast(const batch_bool<int32_t, N>& x);
    template <std::size_t N>
    batch_bool<double, N> bool_cast(const batch_bool<int64_t, N>& x);

    /**************************
     * Boilerplate generators *
     **************************/

// These macros all asume that T and N are in scope and have the meaning used in
// the definitions of batch and batch_bool.
#define XSIMD_FALLBACK_MAPPING_LOOP(RESULT_TYPE, EXPRESSION)  \
    RESULT_TYPE<T, N> result;  \
    for(std::size_t i = 0; i < N; ++i) {  \
        result[i] = (EXPRESSION);  \
    }  \
    return result;

#define XSIMD_FALLBACK_UNARY_OP(RESULT_TYPE, OPERATOR, X)  \
    XSIMD_FALLBACK_MAPPING_LOOP(RESULT_TYPE, (OPERATOR X[i]))

#define XSIMD_FALLBACK_BINARY_OP(RESULT_TYPE, OPERATOR, X, Y)  \
    XSIMD_FALLBACK_MAPPING_LOOP(RESULT_TYPE, (X[i] OPERATOR Y[i]))

#define XSIMD_FALLBACK_BATCH_BITWISE_UNARY_OP(OPERATOR, X)  \
    XSIMD_FALLBACK_MAPPING_LOOP(  \
        batch,  \
        detail::from_unsigned_integer<T>(  \
            OPERATOR detail::to_unsigned_integer(X[i])  \
        )  \
    )

#define XSIMD_FALLBACK_BATCH_BITWISE_BINARY_OP(OPERATOR, X, Y)  \
    XSIMD_FALLBACK_MAPPING_LOOP(  \
        batch,  \
        detail::from_unsigned_integer<T>(  \
            detail::to_unsigned_integer(X[i])  \
            OPERATOR  \
            detail::to_unsigned_integer(Y[i])  \
        )  \
    )

#define XSIMD_FALLBACK_BATCH_UNARY_FUNC(FUNCTION, X)  \
    XSIMD_FALLBACK_MAPPING_LOOP(batch, FUNCTION(X[i]))

#define XSIMD_FALLBACK_BATCH_BINARY_FUNC(FUNCTION, X, Y)  \
    XSIMD_FALLBACK_MAPPING_LOOP(batch, FUNCTION(X[i], Y[i]))

#define XSIMD_FALLBACK_BATCH_TERNARY_FUNC(FUNCTION, X, Y, Z)  \
    XSIMD_FALLBACK_MAPPING_LOOP(batch, FUNCTION(X[i], Y[i], Z[i]))

// NOTE: Static casting a vector is static casting every element
#define XSIMD_FALLBACK_BATCH_STATIC_CAST(T_OUT, X)  \
    batch<T_OUT, N> result;  \
    for(std::size_t i = 0; i < N; ++i) {  \
        result[i] = static_cast<T_OUT>(X[i]);  \
    }  \
    return result;

// NOTE: Casting between batch_bools of the same size is actually trivial!
#define XSIMD_FALLBACK_BOOL_CAST(T_OUT, X)  \
    return batch_bool<T_OUT, N>(static_cast<std::array<bool, N>>(X));

    /***********************************
     * batch_bool<T, N> implementation *
     ***********************************/

    template <typename T, std::size_t N>
    inline batch_bool<T, N>::batch_bool()
    {
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N>::batch_bool(bool b)
        : m_value(detail::array_from_scalar<bool, N>(b))
    {
    }

    template <typename T, std::size_t N>
    template <typename... Args, typename Enable>
    inline batch_bool<T, N>::batch_bool(Args... exactly_N_bools)
        : m_value{ exactly_N_bools... }
    {
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N>::batch_bool(const std::array<bool, N>& rhs)
        : m_value(rhs)
    {
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N>& batch_bool<T, N>::operator=(const std::array<bool, N>& rhs)
    {
        m_value = rhs;
        return *this;
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N>::operator std::array<bool, N>() const
    {
        return m_value;
    }

    template <typename T, std::size_t N>
    inline const bool& batch_bool<T, N>::operator[](std::size_t index) const
    {
        return m_value[index];
    }

    template <typename T, std::size_t N>
    inline bool& batch_bool<T, N>::operator[](std::size_t index)
    {
        return m_value[index];
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator&(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, &, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator|(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, |, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator^(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, ^, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator~(const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_UNARY_OP(batch_bool, ~, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> bitwise_andnot(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_MAPPING_LOOP(batch_bool, (~(lhs[i] & rhs[i])))
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator==(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, ==, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator!=(const batch_bool<T, N>& lhs, const batch_bool<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, !=, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline bool all(const batch_bool<T, N>& rhs)
    {
        for(std::size_t i = 0; i < N; ++i) {
            if(!rhs[i]) return false;
        }
        return true;
    }

    template <typename T, std::size_t N>
    inline bool any(const batch_bool<T, N>& rhs)
    {
        for(std::size_t i = 0; i < N; ++i) {
            if(rhs[i]) return true;
        }
        return false;
    }

    /**********************************
     * batch<T, N> implementation *
     **********************************/

    template <typename T, std::size_t N>
    inline batch<T, N>::batch()
    {
    }

    template <typename T, std::size_t N>
    inline batch<T, N>::batch(T f)
        : m_value(detail::array_from_scalar<T, N>(f))
    {
    }

    template <typename T, std::size_t N>
    template <typename... Args, typename Enable>
    inline batch<T, N>::batch(Args... exactly_N_scalars)
        : m_value{ exactly_N_scalars... }
    {
    }

    template <typename T, std::size_t N>
    inline batch<T, N>::batch(const T* src)
        : batch(src, unaligned_mode())
    {
    }

    template <typename T, std::size_t N>
    inline batch<T, N>::batch(const T* src, aligned_mode)
        : batch(src, unaligned_mode())
    {
    }

    template <typename T, std::size_t N>
    inline batch<T, N>::batch(const T* src, unaligned_mode)
        : m_value(detail::array_from_pointer<T, N>(src))
    {
    }

    template <typename T, std::size_t N>
    inline batch<T, N>::batch(const std::array<T, N>& rhs)
        : m_value(rhs)
    {
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::operator=(const std::array<T, N>& rhs)
    {
        m_value = rhs;
        return *this;
    }

    template <typename T, std::size_t N>
    inline batch<T, N>::operator std::array<T, N>() const
    {
        return m_value;
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_aligned(const float* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_unaligned(const float* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_aligned(const double* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_unaligned(const double* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_aligned(const int32_t* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_unaligned(const int32_t* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_aligned(const int64_t* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline batch<T, N>& batch<T, N>::load_unaligned(const int64_t* src)
    {
        return this->load_unaligned_impl(src);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_aligned(float* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_unaligned(float* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_aligned(double* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_unaligned(double* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_aligned(int32_t* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_unaligned(int32_t* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_aligned(int64_t* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline void batch<T, N>::store_unaligned(int64_t* dst) const
    {
        this->store_unaligned_impl(dst);
    }

    template <typename T, std::size_t N>
    inline const T& batch<T, N>::operator[](std::size_t index) const
    {
        return m_value[index];
    }

    template <typename T, std::size_t N>
    inline T& batch<T, N>::operator[](std::size_t index)
    {
        return m_value[index];
    }

    template <typename T, std::size_t N>
    template <typename U>
    inline batch<T, N>& batch<T, N>::load_unaligned_impl(const U* src)
    {
        for(std::size_t i = 0; i < N; ++i) {
            m_value[i] = static_cast<T>(src[i]);
        }
        return *this;
    }

    template <typename T, std::size_t N>
    template <typename U>
    inline void batch<T, N>::store_unaligned_impl(U* dst) const
    {
        for(std::size_t i = 0; i < N; ++i) {
            dst[i] = static_cast<U>(m_value[i]);
        }
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator-(const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_UNARY_OP(batch, -, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator+(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch, +, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator-(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch, -, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator*(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch, *, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator/(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch, /, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator==(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, ==, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator!=(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, !=, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator<(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, <, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> operator<=(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BINARY_OP(batch_bool, <=, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator&(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BITWISE_BINARY_OP(&, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator|(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BITWISE_BINARY_OP(|, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator^(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BITWISE_BINARY_OP(^, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator~(const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BITWISE_UNARY_OP(~, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> bitwise_andnot(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_MAPPING_LOOP(
            batch,
            detail::from_unsigned_integer<T>(
                ~(
                    detail::to_unsigned_integer(lhs[i])
                    &
                    detail::to_unsigned_integer(rhs[i])
                )
            )
        )
    }

    template <typename T, std::size_t N>
    inline batch<T, N> min(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BINARY_FUNC(std::min, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> max(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BINARY_FUNC(std::max, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fmin(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BINARY_FUNC(std::fmin, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fmax(const batch<T, N>& lhs, const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_BINARY_FUNC(std::fmax, lhs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> abs(const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_UNARY_FUNC(std::abs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fabs(const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_UNARY_FUNC(std::fabs, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> sqrt(const batch<T, N>& rhs)
    {
        XSIMD_FALLBACK_BATCH_UNARY_FUNC(std::sqrt, rhs)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fma(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z)
    {
        XSIMD_FALLBACK_BATCH_TERNARY_FUNC(std::fma, x, y, z)
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fms(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z)
    {
        return fma(x, y, -z);
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fnma(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z)
    {
        return fma(-x, y, z);
    }

    template <typename T, std::size_t N>
    inline batch<T, N> fnms(const batch<T, N>& x, const batch<T, N>& y, const batch<T, N>& z)
    {
        return fma(-x, y, -z);
    }

    template <typename T, std::size_t N>
    inline T hadd(const batch<T, N>& rhs)
    {
        T result = 0;
        for(std::size_t i = 0; i < N; ++i) {
            result += rhs[i];
        }
        return result;
    }

    template <typename T, std::size_t N>
    inline batch<T, N> haddp(const batch<T, N>* row)
    {
        XSIMD_FALLBACK_MAPPING_LOOP(batch, hadd(row[i]))
    }

    template <typename T, std::size_t N>
    inline batch<T, N> select(const batch_bool<T, N>& cond, const batch<T, N>& a, const batch<T, N>& b)
    {
        XSIMD_FALLBACK_MAPPING_LOOP(batch, (cond[i] ? a[i] : b[i]))
    }

    template <typename T, std::size_t N>
    inline batch_bool<T, N> isnan(const batch<T, N>& x)
    {
        XSIMD_FALLBACK_MAPPING_LOOP(batch_bool, std::isnan(x[i]))
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator<<(const batch<T, N>& lhs, int32_t rhs) {
        XSIMD_FALLBACK_MAPPING_LOOP(batch, (lhs[i] << rhs))
    }

    template <typename T, std::size_t N>
    inline batch<T, N> operator>>(const batch<T, N>& lhs, int32_t rhs) {
        XSIMD_FALLBACK_MAPPING_LOOP(batch, (lhs[i] >> rhs))
    }

    /***************************************
     * conversion functions implementation *
     ***************************************/

    template <std::size_t N>
    inline batch<int32_t, N> to_int(const batch<float, N>& x)
    {
        XSIMD_FALLBACK_BATCH_STATIC_CAST(int32_t, x)
    }

    template <std::size_t N>
    inline batch<int64_t, N> to_int(const batch<double, N>& x)
    {
        XSIMD_FALLBACK_BATCH_STATIC_CAST(int64_t, x)
    }

    template <std::size_t N>
    inline batch<float, N> to_float(const batch<int32_t, N>& x)
    {
        XSIMD_FALLBACK_BATCH_STATIC_CAST(float, x)
    }

    template <std::size_t N>
    inline batch<double, N> to_float(const batch<int64_t, N>& x)
    {
        XSIMD_FALLBACK_BATCH_STATIC_CAST(double, x)
    }

    /**************************
     * boolean cast functions *
     **************************/

    template <std::size_t N>
    inline batch_bool<int32_t, N> bool_cast(const batch_bool<float, N>& x)
    {
        XSIMD_FALLBACK_BOOL_CAST(int32_t, x)
    }

    template <std::size_t N>
    inline batch_bool<int64_t, N> bool_cast(const batch_bool<double, N>& x)
    {
        XSIMD_FALLBACK_BOOL_CAST(int64_t, x)
    }

    template <std::size_t N>
    inline batch_bool<float, N> bool_cast(const batch_bool<int32_t, N>& x)
    {
        XSIMD_FALLBACK_BOOL_CAST(float, x)
    }

    template <std::size_t N>
    inline batch_bool<double, N> bool_cast(const batch_bool<int64_t, N>& x)
    {
        XSIMD_FALLBACK_BOOL_CAST(double, x)
    }

    /*****************************************
     * bitwise cast functions implementation *
     *****************************************/

    template <class T_in, class T_out, std::size_t N_in>
    struct bitwise_cast_impl<batch<T_in, N_in>,
                             batch<T_out, sizeof(T_in)*N_in/sizeof(T_out)>>
    {
    private:
        static_assert(sizeof(T_in)*N_in % sizeof(T_out) == 0,
                      "The input and output batches must have the same size");
        static constexpr size_t N_out = sizeof(T_in)*N_in/sizeof(T_out);

        union Converter {
            std::array<T_in, N_in> in;
            std::array<T_out, N_out> out;
        };

    public:
        static batch<T_out, N_out> run(const batch<T_in, N_in>& x) {
            Converter caster;
            caster.in = static_cast<std::array<T_in, N_in>>(x);
            return batch<T_out, N_out>(caster.out);
        }
    };
}

#endif
