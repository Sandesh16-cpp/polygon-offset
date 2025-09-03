#pragma once

/**
 * @file polygon_offset.hpp
 * @brief Modern C++ polygon offset library
 * 
 * This header-only library provides polygon offsetting operations including:
 * - Polygon margin (outward offset) and padding (inward offset)
 * - Support for simple polygons and polygons with holes
 * - Configurable arc segments for smooth curve approximation  
 * - Point offset (circle generation)
 * - Polyline offsetting
 * 
 * Features modern C++20 with RAII, concepts, ranges, and SIMD optimizations.
 * 
 * @version 1.0.0
 * @author C++ Implementation
 */

#include "point.hpp"
#include "edge.hpp" 
#include "offset.hpp"

/**
 * @brief Main namespace for polygon offset library
 */
namespace polygon_offset {

/**
 * @brief Library version information
 */
struct Version {
    static constexpr int major = 1;
    static constexpr int minor = 0;
    static constexpr int patch = 0;
    
    [[nodiscard]] static constexpr const char* string() noexcept {
        return "1.0.0";
    }
};

} // namespace polygon_offset