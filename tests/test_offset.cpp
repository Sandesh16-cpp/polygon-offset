#include <gtest/gtest.h>
#include <polygon_offset/offset.hpp>
#include <vector>
#include <algorithm>

using namespace polygon_offset;

class OffsetTest : public ::testing::Test {
protected:
    static constexpr double EPSILON = 1e-3;  // More tolerant for complex operations
    
    // Helper function to round coordinates for easier comparison
    static Ring round_coordinates(const Ring& ring, int precision = 3) {
        Ring result;
        for (const auto& point : ring) {
            const double factor = std::pow(10.0, precision);
            result.emplace_back(
                std::round(point.x() * factor) / factor,
                std::round(point.y() * factor) / factor
            );
        }
        return result;
    }
    
    // Helper to extract first polygon from multi-polygon result
    static Ring get_first_ring(const MultiPolygon& result) {
        if (!result.empty() && !result[0].empty()) {
            return result[0][0];
        }
        return {};
    }
    
    // Simple square polygon for testing
    const Ring square_points = {
        {0.0, 0.0}, {0.0, 100.0}, {100.0, 100.0}, {100.0, 0.0}, {0.0, 0.0}
    };
};

TEST_F(OffsetTest, DefaultConstructor) {
    Offset offset;
    EXPECT_EQ(offset.arc_segments(), 5);
    EXPECT_DOUBLE_EQ(offset.distance(), 0.0);
    EXPECT_TRUE(offset.vertices().empty());
}

TEST_F(OffsetTest, ConstructorWithVertices) {
    Offset offset(square_points);
    EXPECT_EQ(offset.vertices().size(), 5);
    EXPECT_EQ(offset.edges().size(), 4);  // 4 edges for a square
}

TEST_F(OffsetTest, ConstructorWithPoint) {
    Point center(10.0, 20.0);
    Offset offset(center);
    EXPECT_TRUE(offset.is_point());
    EXPECT_EQ(offset.vertices().size(), 1);
    EXPECT_EQ(offset.vertices()[0], center);
}

TEST_F(OffsetTest, FluentInterface) {
    Offset offset;
    auto& result = offset.data(square_points)
                         .arc_segments(3)
                         .distance(5.0);
    
    EXPECT_EQ(&result, &offset);  // Should return reference to self
    EXPECT_EQ(offset.arc_segments(), 3);
    EXPECT_DOUBLE_EQ(offset.distance(), 5.0);
}

TEST_F(OffsetTest, InvalidDataThrows) {
    std::vector<Point> empty_points;
    Offset offset;
    EXPECT_THROW(offset.data(empty_points), std::invalid_argument);
}

TEST_F(OffsetTest, ZeroMargin) {
    Offset offset(square_points);
    auto result = offset.margin(0.0);
    
    // Zero margin should return original polygon
    EXPECT_FALSE(result.empty());
    auto first_ring = get_first_ring(result);
    EXPECT_EQ(first_ring.size(), 5);
}

TEST_F(OffsetTest, ZeroPadding) {
    Offset offset(square_points);
    auto result = offset.padding(0.0);
    
    // Zero padding should return original polygon (closed)
    EXPECT_FALSE(result.empty());
}

TEST_F(OffsetTest, ZeroOffset) {
    Offset offset(square_points);
    auto result = offset.offset(0.0);
    
    EXPECT_FALSE(result.empty());
}

TEST_F(OffsetTest, PositiveOffsetCallsMargin) {
    Offset offset(square_points);
    auto margin_result = offset.margin(5.0);
    
    Offset offset2(square_points);
    auto offset_result = offset2.offset(5.0);
    
    // Should produce similar results (both call margin internally)
    EXPECT_EQ(margin_result.size(), offset_result.size());
}

TEST_F(OffsetTest, NegativeOffsetCallsPadding) {
    Offset offset(square_points);
    auto padding_result = offset.padding(5.0);
    
    Offset offset2(square_points);
    auto offset_result = offset2.offset(-5.0);
    
    // Should produce similar results (both call padding internally)
    EXPECT_EQ(padding_result.size(), offset_result.size());
}

TEST_F(OffsetTest, ArcSegments) {
    Offset offset(square_points);
    
    // Test with different arc segment counts
    auto result1 = offset.arc_segments(1).margin(5.0);
    auto result3 = offset.arc_segments(3).margin(5.0);
    
    EXPECT_FALSE(result1.empty());
    EXPECT_FALSE(result3.empty());
    
    // More arc segments should generally produce more vertices
    // (though exact count depends on implementation details)
}

TEST_F(OffsetTest, PointOffset) {
    Point center(0.0, 0.0);
    Offset offset(center);
    
    auto result = offset.offset(5.0);
    EXPECT_FALSE(result.empty());
    
    auto circle = get_first_ring(result);
    EXPECT_GT(circle.size(), 5);  // Should create a circle with multiple points
    
    // Check that all points are approximately at distance 5 from center
    for (size_t i = 0; i < circle.size() - 1; ++i) {  // Skip last point (duplicate of first)
        double distance = (circle[i] - center).length();
        EXPECT_NEAR(distance, 5.0, EPSILON);
    }
}

TEST_F(OffsetTest, PointNegativeOffset) {
    Point center(0.0, 0.0);
    Offset offset(center);
    
    auto result = offset.offset(-5.0);
    
    // Negative offset of a point should return the point itself
    EXPECT_FALSE(result.empty());
    auto first_ring = get_first_ring(result);
    EXPECT_EQ(first_ring.size(), 1);
    EXPECT_EQ(first_ring[0], center);
}

TEST_F(OffsetTest, PointZeroOffset) {
    Point center(10.0, 20.0);
    Offset offset(center);
    
    auto result = offset.offset(0.0);
    
    EXPECT_FALSE(result.empty());
    auto first_ring = get_first_ring(result);
    EXPECT_EQ(first_ring.size(), 1);
    EXPECT_EQ(first_ring[0], center);
}

TEST_F(OffsetTest, OffsetLineZeroDistance) {
    Ring line_points = {{0.0, 100.0}, {100.0, 100.0}};  // Simple line
    Offset offset(line_points);
    
    auto result = offset.offset_line(0.0);
    
    EXPECT_FALSE(result.empty());
    auto first_ring = get_first_ring(result);
    EXPECT_EQ(first_ring.size(), 2);
}

TEST_F(OffsetTest, OffsetLinePositiveDistance) {
    Ring line_points = {{0.0, 100.0}, {100.0, 100.0}};  // Horizontal line
    Offset offset(line_points);
    
    auto result = offset.offset_line(5.0);
    
    EXPECT_FALSE(result.empty());
    // Should create a polygon around the line
}

TEST_F(OffsetTest, NegativeMarginThrows) {
    Offset offset(square_points);
    
    // offset_line with negative distance should throw (from offset_lines)
    EXPECT_THROW(offset.offset_line(-5.0), std::invalid_argument);
}

TEST_F(OffsetTest, EdgeCreationFromPolygon) {
    Ring simple_triangle = {{0.0, 0.0}, {10.0, 0.0}, {5.0, 10.0}, {0.0, 0.0}};
    Offset offset(simple_triangle);
    
    // Triangle should create 3 edges
    EXPECT_EQ(offset.edges().size(), 3);
}

TEST_F(OffsetTest, ClosedPolygonHandling) {
    // Test with polygon that has explicit closing point
    Ring closed_square = square_points;  // Already has closing point
    Offset offset(closed_square);
    
    // Should still create 4 edges (not 5)
    EXPECT_EQ(offset.edges().size(), 4);
}

TEST_F(OffsetTest, OpenPolygonHandling) {
    // Test with polygon without explicit closing point
    Ring open_square = {
        {0.0, 0.0}, {0.0, 100.0}, {100.0, 100.0}, {100.0, 0.0}  // No closing point
    };
    Offset offset(open_square);
    
    // Should still create 4 edges
    EXPECT_EQ(offset.edges().size(), 4);
}