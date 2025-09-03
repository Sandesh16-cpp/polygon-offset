#pragma once

#include "point.hpp"
#include "edge.hpp"
#include <vector>
#include <memory>
#include <ranges>
#include <concepts>
#include <numbers>
#include <cmath>
#include <algorithm>

namespace polygon_offset {

/**
 * @brief Type aliases for polygon representations
 */
using Ring = std::vector<Point>;
using Polygon = std::vector<Ring>;
using MultiPolygon = std::vector<Polygon>;

/**
 * @brief Concept for polygon-like containers
 */
template<typename T>
concept PolygonLike = std::ranges::range<T> && 
                      std::convertible_to<std::ranges::range_value_t<T>, Point>;

/**
 * @brief Main class for polygon offset operations with fluent API
 */
class Offset {
public:
    using value_type = Point::value_type;
    
    /**
     * @brief Default constructor
     */
    Offset() = default;
    
    /**
     * @brief Constructor with vertices and optional arc segments
     * @param vertices Input polygon vertices
     * @param arc_segments Number of segments for arc approximation (default: 5)
     */
    template<PolygonLike T>
    explicit Offset(const T& vertices, int arc_segments = 5) 
        : arc_segments_(arc_segments) {
        data(vertices);
    }
    
    /**
     * @brief Constructor for single point
     * @param point Input point
     * @param arc_segments Number of segments for circle approximation (default: 5)
     */
    explicit Offset(const Point& point, int arc_segments = 5)
        : vertices_({point}), arc_segments_(arc_segments), is_point_(true) {}
    
    // Move/copy constructors
    Offset(const Offset&) = default;
    Offset(Offset&&) = default;
    Offset& operator=(const Offset&) = default;
    Offset& operator=(Offset&&) = default;
    
    /**
     * @brief Set polygon data (fluent interface)
     * @param vertices Input polygon vertices
     * @return Reference to this object for chaining
     */
    template<PolygonLike T>
    Offset& data(const T& vertices) {
        vertices_.clear();
        edges_.clear();
        is_point_ = false;
        
        if (vertices.empty()) {
            throw std::invalid_argument("Offset requires at least one coordinate to work with");
        }
        
        // Convert input to our Point representation
        std::ranges::transform(vertices, std::back_inserter(vertices_),
                              [](const auto& v) { 
                                  if constexpr (std::convertible_to<decltype(v), Point>) {
                                      return Point{v};
                                  } else {
                                      return Point{v[0], v[1]};
                                  }
                              });
        
        // Check if it's a single point
        if (vertices_.size() == 1) {
            is_point_ = true;
            return *this;
        }
        
        // Process contour to create edges
        process_contour(vertices_, edges_);
        
        return *this;
    }
    
    /**
     * @brief Set number of arc segments (fluent interface)
     * @param segments Number of segments for arc approximation
     * @return Reference to this object for chaining
     */
    Offset& arc_segments(int segments) {
        arc_segments_ = segments;
        return *this;
    }
    
    /**
     * @brief Set offset distance (fluent interface)
     * @param dist Offset distance
     * @return Reference to this object for chaining
     */
    Offset& distance(value_type dist) {
        distance_ = dist;
        return *this;
    }
    
    /**
     * @brief Create margin (outward offset) polygon
     * @param dist Positive distance for outward offset
     * @return Resulting polygon(s)
     */
    [[nodiscard]] MultiPolygon margin(value_type dist) {
        distance(dist);
        
        if (is_point_) {
            return offset_point(distance_);
        }
        
        if (dist == 0.0) {
            return {{{vertices_}}};
        }
        
        auto offset_result = offset_lines(distance_);
        // TODO: Implement union operation with input polygon
        return orient_rings(offset_result);
    }
    
    /**
     * @brief Create padding (inward offset) polygon
     * @param dist Positive distance for inward offset
     * @return Resulting polygon(s)
     */
    [[nodiscard]] MultiPolygon padding(value_type dist) {
        distance(dist);
        
        if (distance_ == 0.0) {
            auto result = vertices_;
            ensure_last_point(result);
            return {{{result}}};
        }
        
        if (is_point_) {
            return {{{vertices_}}};  // Points cannot be padded
        }
        
        auto offset_result = offset_lines(distance_);
        // TODO: Implement difference operation between input and offset
        return orient_rings(offset_result);
    }
    
    /**
     * @brief Generic offset method (decides between margin/padding based on sign)
     * @param dist Distance (positive for margin, negative for padding)
     * @return Resulting polygon(s)
     */
    [[nodiscard]] MultiPolygon offset(value_type dist) {
        distance(dist);
        
        if (distance_ == 0.0) {
            if (is_point_) {
                return {{{vertices_}}};
            } else {
                return {{{vertices_}}};
            }
        }
        
        return distance_ > 0.0 ? margin(distance_) : padding(-distance_);
    }
    
    /**
     * @brief Create offset polyline (no fill operations)
     * @param dist Distance for offset
     * @return Resulting polygon(s)
     */
    [[nodiscard]] MultiPolygon offset_line(value_type dist) {
        if (dist == 0.0) {
            return {{{vertices_}}};
        }
        
        distance(dist);
        return orient_rings(offset_lines(dist));
    }
    
    // Accessors
    [[nodiscard]] const Ring& vertices() const noexcept { return vertices_; }
    [[nodiscard]] const std::vector<Edge>& edges() const noexcept { return edges_; }
    [[nodiscard]] int arc_segments() const noexcept { return arc_segments_; }
    [[nodiscard]] value_type distance() const noexcept { return distance_; }
    [[nodiscard]] bool is_point() const noexcept { return is_point_; }
    
private:
    Ring vertices_;
    std::vector<Edge> edges_;
    value_type distance_ = 0.0;
    int arc_segments_ = 5;
    bool is_point_ = false;
    
    /**
     * @brief Process contour to create edges
     */
    void process_contour(const Ring& contour, std::vector<Edge>& edges) {
        if (contour.size() < 2) return;
        
        auto len = contour.size();
        
        // Check if first and last points are the same (closed polygon)
        if (contour.front().equals(contour.back())) {
            len -= 1;  // Skip duplicate last point
        }
        
        for (size_t i = 0; i < len; ++i) {
            const auto& current = contour[i];
            const auto& next = contour[(i + 1) % len];
            
            try {
                edges.emplace_back(current, next);
            } catch (const std::invalid_argument&) {
                // Skip zero-length edges
                continue;
            }
        }
    }
    
    /**
     * @brief Create offset lines without boolean operations
     */
    [[nodiscard]] MultiPolygon offset_lines(value_type dist) {
        if (dist < 0.0) {
            throw std::invalid_argument("Cannot apply negative margin to line");
        }
        
        if (vertices_.size() == 1) {
            return offset_point(dist);
        }
        
        return offset_contour(vertices_, edges_, dist);
    }
    
    /**
     * @brief Offset a single contour
     */
    [[nodiscard]] MultiPolygon offset_contour(const Ring& curve, 
                                              const std::vector<Edge>& edges, 
                                              value_type dist) {
        MultiPolygon result;
        
        for (size_t i = 0; i < edges.size(); ++i) {
            const auto& edge = edges[i];
            const auto& v1 = curve[i];
            const auto& v2 = curve[(i + 1) % curve.size()];
            
            auto segment = offset_segment(v1, v2, edge, dist);
            ensure_last_point(segment);
            
            result.push_back({{segment}});
        }
        
        // TODO: Union all segments together
        return result;
    }
    
    /**
     * @brief Offset a single segment
     */
    [[nodiscard]] Ring offset_segment(const Point& v1, const Point& v2, 
                                      const Edge& edge, value_type dist) {
        Ring vertices;
        
        // Create offset edges
        auto offset1 = edge.offset(edge.in_normal() * dist);
        auto offset2 = edge.inverse_offset(edge.out_normal() * dist);
        
        std::vector<Edge> offsets = {offset1, offset2};
        
        for (size_t i = 0; i < 2; ++i) {
            const auto& this_edge = offsets[i];
            const auto& prev_edge = offsets[(i + 1) % 2];
            
            create_arc(vertices,
                      i == 0 ? v1 : v2,
                      dist,
                      prev_edge.next(),
                      this_edge.current(),
                      arc_segments_,
                      true);
        }
        
        return vertices;
    }
    
    /**
     * @brief Create arc between two points
     */
    void create_arc(Ring& vertices, const Point& center, value_type radius,
                   const Point& start_vertex, const Point& end_vertex,
                   int segments, bool outwards) {
        
        if (segments <= 0) {
            vertices.push_back(start_vertex);
            vertices.push_back(end_vertex);
            return;
        }
        
        // Calculate angles
        const auto start_vec = start_vertex - center;
        const auto end_vec = end_vertex - center;
        
        auto start_angle = std::atan2(start_vec.y(), start_vec.x());
        auto end_angle = std::atan2(end_vec.y(), end_vec.x());
        
        // Ensure proper angle direction
        auto angle_diff = end_angle - start_angle;
        if (outwards) {
            if (angle_diff < 0) angle_diff += 2 * std::numbers::pi;
        } else {
            if (angle_diff > 0) angle_diff -= 2 * std::numbers::pi;
        }
        
        // Generate arc points
        vertices.push_back(start_vertex);
        
        for (int i = 1; i < segments; ++i) {
            const auto t = static_cast<value_type>(i) / segments;
            const auto angle = start_angle + angle_diff * t;
            const auto x = center.x() + radius * std::cos(angle);
            const auto y = center.y() + radius * std::sin(angle);
            vertices.emplace_back(x, y);
        }
        
        vertices.push_back(end_vertex);
    }
    
    /**
     * @brief Create circle around point
     */
    [[nodiscard]] MultiPolygon offset_point(value_type radius) {
        if (radius <= 0.0 || vertices_.empty()) {
            return {{{vertices_}}};
        }
        
        const auto& center = vertices_[0];
        Ring circle;
        
        const auto segments = std::max(3, arc_segments_ * 2);  // More segments for full circle
        
        for (int i = 0; i < segments; ++i) {
            const auto angle = 2.0 * std::numbers::pi * i / segments;
            const auto x = center.x() + radius * std::cos(angle);
            const auto y = center.y() + radius * std::sin(angle);
            circle.emplace_back(x, y);
        }
        
        // Close the circle
        if (!circle.empty()) {
            circle.push_back(circle.front());
        }
        
        return {{{circle}}};
    }
    
    /**
     * @brief Ensure polygon is closed (first point == last point)
     */
    void ensure_last_point(Ring& vertices) {
        if (vertices.size() >= 2 && !vertices.front().equals(vertices.back())) {
            vertices.push_back(vertices.front());
        }
    }
    
    /**
     * @brief Orient rings with proper winding order
     * TODO: Implement proper polygon orientation
     */
    [[nodiscard]] MultiPolygon orient_rings(const MultiPolygon& polygons) {
        // For now, just return as-is
        // TODO: Implement proper ring orientation based on area calculation
        return polygons;
    }
};

} // namespace polygon_offset