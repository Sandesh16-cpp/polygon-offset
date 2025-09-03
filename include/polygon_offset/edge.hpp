#pragma once

#include "point.hpp"
#include <stdexcept>

namespace polygon_offset {

/**
 * @brief Represents an edge of a polygon with pre-calculated normals
 */
class Edge {
public:
    using value_type = Point::value_type;
    
    /**
     * @brief Construct an edge from two points
     * @param current Start point of the edge
     * @param next End point of the edge
     * @throws std::invalid_argument if points are identical (zero-length edge)
     */
    Edge(const Point& current, const Point& next) 
        : current_(current), next_(next) {
        
        const auto delta = next_ - current_;
        const auto length = delta.length();
        
        if (length == 0.0) {
            throw std::invalid_argument("Vertices overlap - cannot create edge from identical points");
        }
        
        // Calculate normalized direction vector
        direction_ = delta / length;
        
        // Inward normal is [-dy/length, dx/length] (rotated -90 degrees)
        in_normal_ = Point{-delta.y() / length, delta.x() / length};
        
        // Outward normal is opposite of inward normal
        out_normal_ = -in_normal_;
    }
    
    // Accessors
    [[nodiscard]] const Point& current() const noexcept { return current_; }
    [[nodiscard]] const Point& next() const noexcept { return next_; }
    [[nodiscard]] const Point& direction() const noexcept { return direction_; }
    [[nodiscard]] const Point& in_normal() const noexcept { return in_normal_; }
    [[nodiscard]] const Point& out_normal() const noexcept { return out_normal_; }
    
    /**
     * @brief Get the length of the edge
     */
    [[nodiscard]] value_type length() const noexcept {
        return (next_ - current_).length();
    }
    
    /**
     * @brief Create a new edge offset by the given vector
     * @param offset_vector Vector to offset both points by
     * @return New offset edge
     */
    [[nodiscard]] Edge offset(const Point& offset_vector) const {
        return Edge{current_ + offset_vector, next_ + offset_vector};
    }
    
    /**
     * @brief Create a new edge offset by dx, dy
     * @param dx X offset
     * @param dy Y offset
     * @return New offset edge
     */
    [[nodiscard]] Edge offset(value_type dx, value_type dy) const {
        return offset(Point{dx, dy});
    }
    
    /**
     * @brief Create a new edge with points in reverse order
     * @return Reversed edge
     */
    [[nodiscard]] Edge inverse() const {
        return Edge{next_, current_};
    }
    
    /**
     * @brief Create inverse offset edge (reversed direction)
     * @param offset_vector Vector to offset both points by
     * @return New inverse offset edge
     */
    [[nodiscard]] Edge inverse_offset(const Point& offset_vector) const {
        return Edge{next_ + offset_vector, current_ + offset_vector};
    }
    
    /**
     * @brief Create inverse offset edge (reversed direction)
     * @param dx X offset
     * @param dy Y offset
     * @return New inverse offset edge
     */
    [[nodiscard]] Edge inverse_offset(value_type dx, value_type dy) const {
        return inverse_offset(Point{dx, dy});
    }
    
    /**
     * @brief Static helper to create offset edge
     * @param current Start point
     * @param next End point  
     * @param dx X offset
     * @param dy Y offset
     * @return New offset edge
     */
    [[nodiscard]] static Edge offset_edge(const Point& current, const Point& next, 
                                          value_type dx, value_type dy) {
        const Point offset_vector{dx, dy};
        return Edge{current + offset_vector, next + offset_vector};
    }
    
private:
    Point current_;
    Point next_;
    Point direction_;
    Point in_normal_;
    Point out_normal_;
};

} // namespace polygon_offset