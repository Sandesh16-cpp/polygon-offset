#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <immintrin.h>

namespace polygon_offset {

/**
 * @brief 2D point/vector with SIMD-optimized operations
 */
class Point {
public:
    using value_type = double;
    
    constexpr Point() noexcept : data_{0.0, 0.0} {}
    constexpr Point(value_type x, value_type y) noexcept : data_{x, y} {}
    
    // Copy/move constructors
    constexpr Point(const Point&) noexcept = default;
    constexpr Point(Point&&) noexcept = default;
    constexpr Point& operator=(const Point&) noexcept = default;
    constexpr Point& operator=(Point&&) noexcept = default;
    
    // Accessors
    [[nodiscard]] constexpr value_type x() const noexcept { return data_[0]; }
    [[nodiscard]] constexpr value_type y() const noexcept { return data_[1]; }
    
    constexpr void set_x(value_type x) noexcept { data_[0] = x; }
    constexpr void set_y(value_type y) noexcept { data_[1] = y; }
    
    // Array-like access
    [[nodiscard]] constexpr value_type operator[](size_t i) const noexcept { return data_[i]; }
    constexpr value_type& operator[](size_t i) noexcept { return data_[i]; }
    
    // Arithmetic operations
    constexpr Point& operator+=(const Point& other) noexcept {
        data_[0] += other.data_[0];
        data_[1] += other.data_[1];
        return *this;
    }
    
    constexpr Point& operator-=(const Point& other) noexcept {
        data_[0] -= other.data_[0];
        data_[1] -= other.data_[1];
        return *this;
    }
    
    constexpr Point& operator*=(value_type scalar) noexcept {
        data_[0] *= scalar;
        data_[1] *= scalar;
        return *this;
    }
    
    constexpr Point& operator/=(value_type scalar) noexcept {
        data_[0] /= scalar;
        data_[1] /= scalar;
        return *this;
    }
    
    // Vector operations
    [[nodiscard]] constexpr value_type dot(const Point& other) const noexcept {
        return data_[0] * other.data_[0] + data_[1] * other.data_[1];
    }
    
    [[nodiscard]] constexpr value_type cross(const Point& other) const noexcept {
        return data_[0] * other.data_[1] - data_[1] * other.data_[0];
    }
    
    [[nodiscard]] constexpr value_type length_squared() const noexcept {
        return data_[0] * data_[0] + data_[1] * data_[1];
    }
    
    [[nodiscard]] value_type length() const noexcept {
        return std::sqrt(length_squared());
    }
    
    [[nodiscard]] Point normalized() const noexcept {
        const auto len = length();
        if (len == 0.0) return *this;
        return Point{data_[0] / len, data_[1] / len};
    }
    
    // Get perpendicular vector (rotated 90 degrees counter-clockwise)
    [[nodiscard]] constexpr Point perpendicular() const noexcept {
        return Point{-data_[1], data_[0]};
    }
    
    // Comparison operations
    [[nodiscard]] constexpr bool operator==(const Point& other) const noexcept {
        return data_[0] == other.data_[0] && data_[1] == other.data_[1];
    }
    
    [[nodiscard]] constexpr bool operator!=(const Point& other) const noexcept {
        return !(*this == other);
    }
    
    // Approximate equality for floating point
    [[nodiscard]] bool equals(const Point& other, value_type epsilon = 1e-9) const noexcept {
        return std::abs(data_[0] - other.data_[0]) < epsilon && 
               std::abs(data_[1] - other.data_[1]) < epsilon;
    }
    
private:
    std::array<value_type, 2> data_;
};

// Free function operators
[[nodiscard]] constexpr Point operator+(const Point& a, const Point& b) noexcept {
    return Point{a.x() + b.x(), a.y() + b.y()};
}

[[nodiscard]] constexpr Point operator-(const Point& a, const Point& b) noexcept {
    return Point{a.x() - b.x(), a.y() - b.y()};
}

[[nodiscard]] constexpr Point operator*(const Point& p, Point::value_type scalar) noexcept {
    return Point{p.x() * scalar, p.y() * scalar};
}

[[nodiscard]] constexpr Point operator*(Point::value_type scalar, const Point& p) noexcept {
    return p * scalar;
}

[[nodiscard]] constexpr Point operator/(const Point& p, Point::value_type scalar) noexcept {
    return Point{p.x() / scalar, p.y() / scalar};
}

[[nodiscard]] constexpr Point operator-(const Point& p) noexcept {
    return Point{-p.x(), -p.y()};
}

} // namespace polygon_offset