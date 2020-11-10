/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_FIXED_POINT_H
#define BTN_FIXED_POINT_H

/**
 * @file
 * btn::fixed_point header file.
 *
 * @ingroup math
 */

#include "btn_fixed.h"
#include "btn_point.h"

namespace btn
{

/**
 * @brief Defines a two-dimensional point using fixed point precision.
 *
 * @ingroup math
 */
class fixed_point
{

public:
    /**
     * @brief Default constructor.
     */
    constexpr fixed_point() = default;

    /**
     * @brief Constructor.
     * @param x Horizontal coordinate.
     * @param y Vertical coordinate.
     */
    constexpr fixed_point(fixed x, fixed y) :
        _x(x),
        _y(y)
    {
    }

    /**
     * @brief Constructor.
     * @param point Integer point.
     */
    constexpr fixed_point(const point& point) :
        _x(point.x()),
        _y(point.y())
    {
    }

    /**
     * @brief Returns the horizontal coordinate.
     */
    [[nodiscard]] constexpr fixed x() const
    {
        return _x;
    }

    /**
     * @brief Sets the horizontal coordinate.
     */
    constexpr void set_x(fixed x)
    {
        _x = x;
    }

    /**
     * @brief Returns the vertical coordinate.
     */
    [[nodiscard]] constexpr fixed y() const
    {
        return _y;
    }

    /**
     * @brief Sets the vertical coordinate.
     */
    constexpr void set_y(fixed y)
    {
        _y = y;
    }

    /**
     * @brief Returns a fixed_point that is formed by changing the sign of both coordinates.
     */
    [[nodiscard]] constexpr fixed_point operator-() const
    {
        return fixed_point(-_x, -_y);
    }

    /**
     * @brief Adds the given fixed_point to this one.
     * @param other fixed_point to add.
     * @return Reference to this.
     */
    constexpr fixed_point& operator+=(const fixed_point& other)
    {
        _x += other._x;
        _y += other._y;
        return *this;
    }

    /**
     * @brief Subtracts the given fixed_point to this one.
     * @param other fixed_point to subtract.
     * @return Reference to this.
     */
    constexpr fixed_point& operator-=(const fixed_point& other)
    {
        _x -= other._x;
        _y -= other._y;
        return *this;
    }

    /**
     * @brief Multiplies both coordinates by the given factor.
     * @param value Integer multiplication factor.
     * @return Reference to this.
     */
    constexpr fixed_point& operator*=(int value)
    {
        _x *= value;
        _y *= value;
        return *this;
    }

    /**
     * @brief Multiplies both coordinates by the given factor.
     * @param value Unsigned integer multiplication factor.
     * @return Reference to this.
     */
    constexpr fixed_point& operator*=(unsigned value)
    {
        _x *= value;
        _y *= value;
        return *this;
    }

    /**
     * @brief Multiplies both coordinates by the given factor.
     * @param value Fixed point multiplication factor.
     * @return Reference to this.
     */
    constexpr fixed_point& operator*=(fixed value)
    {
        _x *= value;
        _y *= value;
        return *this;
    }

    /**
     * @brief Divides both coordinates by the given divisor.
     * @param value Valid integer divisor (!= 0).
     * @return Reference to this.
     */
    constexpr fixed_point& operator/=(int value)
    {
        _x /= value;
        _y /= value;
        return *this;
    }

    /**
     * @brief Divides both coordinates by the given divisor.
     * @param value Valid unsigned integer divisor (!= 0).
     * @return Reference to this.
     */
    constexpr fixed_point& operator/=(unsigned value)
    {
        _x /= value;
        _y /= value;
        return *this;
    }

    /**
     * @brief Divides both coordinates by the given divisor.
     * @param value Valid fixed point divisor (!= 0).
     * @return Reference to this.
     */
    constexpr fixed_point& operator/=(fixed value)
    {
        _x /= value;
        _y /= value;
        return *this;
    }

    /**
     * @brief Returns the sum of a and b.
     */
    [[nodiscard]] constexpr friend fixed_point operator+(const fixed_point& a, const fixed_point& b)
    {
        return fixed_point(a._x + b._x, a._y + b._y);
    }

    /**
     * @brief Returns b subtracted from a.
     */
    [[nodiscard]] constexpr friend fixed_point operator-(const fixed_point& a, const fixed_point& b)
    {
        return fixed_point(a._x - b._x, a._y - b._y);
    }

    /**
     * @brief Returns a multiplied by b.
     */
    [[nodiscard]] constexpr friend fixed_point operator*(const fixed_point& a, int b)
    {
        return fixed_point(a._x * b, a._y * b);
    }

    /**
     * @brief Returns a multiplied by b.
     */
    [[nodiscard]] constexpr friend fixed_point operator*(const fixed_point& a, unsigned b)
    {
        return fixed_point(a._x * b, a._y * b);
    }

    /**
     * @brief Returns a multiplied by b.
     */
    [[nodiscard]] constexpr friend fixed_point operator*(const fixed_point& a, fixed b)
    {
        return fixed_point(a._x * b, a._y * b);
    }

    /**
     * @brief Returns a divided by b.
     */
    [[nodiscard]] constexpr friend fixed_point operator/(const fixed_point& a, int b)
    {
        return fixed_point(a._x / b, a._y / b);
    }

    /**
     * @brief Returns a divided by b.
     */
    [[nodiscard]] constexpr friend fixed_point operator/(const fixed_point& a, unsigned b)
    {
        return fixed_point(a._x / b, a._y / b);
    }

    /**
     * @brief Returns a divided by b.
     */
    [[nodiscard]] constexpr friend fixed_point operator/(const fixed_point& a, fixed b)
    {
        return fixed_point(a._x / b, a._y / b);
    }

    /**
     * @brief Default equal operator.
     */
    [[nodiscard]] constexpr friend bool operator==(const fixed_point& a, const fixed_point& b) = default;

private:
    fixed _x = 0;
    fixed _y = 0;
};


/**
 * @brief Hash support for fixed_point.
 *
 * @ingroup math
 * @ingroup functional
 */
template<>
struct hash<fixed_point>
{
    /**
     * @brief Returns the hash of the given fixed_point.
     */
    [[nodiscard]] constexpr unsigned operator()(const fixed_point& value) const
    {
        unsigned result = make_hash(value.x());
        hash_combine(value.y(), result);
        return result;
    }
};

}

#endif
