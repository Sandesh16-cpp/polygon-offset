#include <polygon_offset/polygon_offset.hpp>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

using namespace polygon_offset;

#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_NEAR(a, b, eps) assert(std::abs((a) - (b)) < (eps))
#define ASSERT_TRUE(x) assert(x)
#define ASSERT_FALSE(x) assert(!(x))

void test_point() {
    std::cout << "Testing Point class...\n";
    
    // Default constructor
    Point p1;
    ASSERT_EQ(p1.x(), 0.0);
    ASSERT_EQ(p1.y(), 0.0);
    
    // Parameterized constructor
    Point p2(3.0, 4.0);
    ASSERT_EQ(p2.x(), 3.0);
    ASSERT_EQ(p2.y(), 4.0);
    
    // Addition
    Point p3 = p1 + p2;
    ASSERT_EQ(p3.x(), 3.0);
    ASSERT_EQ(p3.y(), 4.0);
    
    // Length
    ASSERT_NEAR(p2.length(), 5.0, 1e-9);
    
    // Normalized
    Point normalized = p2.normalized();
    ASSERT_NEAR(normalized.length(), 1.0, 1e-9);
    
    // Perpendicular
    Point p4(1.0, 0.0);
    Point perp = p4.perpendicular();
    ASSERT_EQ(perp.x(), 0.0);
    ASSERT_EQ(perp.y(), 1.0);
    
    std::cout << "Point tests passed!\n";
}

void test_edge() {
    std::cout << "Testing Edge class...\n";
    
    // Basic construction
    Point p1(0.0, 0.0);
    Point p2(0.0, 5.0);
    Edge edge(p1, p2);
    
    ASSERT_EQ(edge.current(), p1);
    ASSERT_EQ(edge.next(), p2);
    ASSERT_NEAR(edge.length(), 5.0, 1e-9);
    
    // Normal vectors for vertical edge
    ASSERT_NEAR(edge.in_normal().x(), -1.0, 1e-9);
    ASSERT_NEAR(edge.in_normal().y(), 0.0, 1e-9);
    ASSERT_NEAR(edge.out_normal().x(), 1.0, 1e-9);
    ASSERT_NEAR(edge.out_normal().y(), 0.0, 1e-9);
    
    // Offset
    Edge offset_edge = edge.offset(2.0, 3.0);
    ASSERT_EQ(offset_edge.current(), Point(2.0, 3.0));
    ASSERT_EQ(offset_edge.next(), Point(2.0, 8.0));
    
    std::cout << "Edge tests passed!\n";
}

void test_offset_basic() {
    std::cout << "Testing basic Offset functionality...\n";
    
    // Default constructor
    Offset offset;
    ASSERT_EQ(offset.arc_segments(), 5);
    ASSERT_EQ(offset.distance(), 0.0);
    ASSERT_TRUE(offset.vertices().empty());
    
    // Square polygon
    Ring square = {
        {0.0, 0.0}, {0.0, 100.0}, {100.0, 100.0}, {100.0, 0.0}, {0.0, 0.0}
    };
    
    Offset square_offset(square);
    ASSERT_EQ(square_offset.vertices().size(), 5);
    ASSERT_EQ(square_offset.edges().size(), 4);  // 4 edges for square
    
    // Fluent interface
    auto& result = square_offset.arc_segments(3).distance(5.0);
    ASSERT_EQ(&result, &square_offset);  // Should return reference to self
    ASSERT_EQ(square_offset.arc_segments(), 3);
    ASSERT_NEAR(square_offset.distance(), 5.0, 1e-9);
    
    std::cout << "Basic Offset tests passed!\n";
}

void test_offset_operations() {
    std::cout << "Testing Offset operations...\n";
    
    Ring square = {
        {0.0, 0.0}, {0.0, 100.0}, {100.0, 100.0}, {100.0, 0.0}, {0.0, 0.0}
    };
    
    Offset offset(square);
    
    // Zero operations should not crash
    auto margin_zero = offset.margin(0.0);
    auto padding_zero = offset.padding(0.0);
    auto offset_zero = offset.offset(0.0);
    
    ASSERT_FALSE(margin_zero.empty());
    ASSERT_FALSE(padding_zero.empty());
    ASSERT_FALSE(offset_zero.empty());
    
    // Non-zero operations should produce results
    auto margin_pos = offset.margin(5.0);
    auto padding_pos = offset.padding(5.0);
    auto offset_pos = offset.offset(5.0);
    auto offset_neg = offset.offset(-5.0);
    
    ASSERT_FALSE(margin_pos.empty());
    ASSERT_FALSE(padding_pos.empty());
    ASSERT_FALSE(offset_pos.empty());
    ASSERT_FALSE(offset_neg.empty());
    
    std::cout << "Offset operations tests passed!\n";
}

void test_point_offset() {
    std::cout << "Testing Point offset (circle generation)...\n";
    
    Point center(0.0, 0.0);
    Offset point_processor(center);
    
    ASSERT_TRUE(point_processor.is_point());
    ASSERT_EQ(point_processor.vertices().size(), 1);
    
    // Generate circle
    auto circle_result = point_processor.arc_segments(8).offset(25.0);
    ASSERT_FALSE(circle_result.empty());
    
    if (!circle_result.empty() && !circle_result[0].empty()) {
        auto& circle = circle_result[0][0];
        ASSERT_TRUE(circle.size() > 5);  // Should have multiple points
        
        // Check that points are approximately at correct distance
        for (size_t i = 0; i < circle.size() - 1; ++i) {  // Skip last (duplicate first)
            double distance = (circle[i] - center).length();
            ASSERT_NEAR(distance, 25.0, 1.0);  // Allow some tolerance
        }
    }
    
    // Zero radius
    auto zero_result = point_processor.offset(0.0);
    ASSERT_FALSE(zero_result.empty());
    
    // Negative radius
    auto neg_result = point_processor.offset(-5.0);
    ASSERT_FALSE(neg_result.empty());
    
    std::cout << "Point offset tests passed!\n";
}

void test_edge_cases() {
    std::cout << "Testing edge cases...\n";
    
    // Empty data should throw
    try {
        std::vector<Point> empty_points;
        Offset empty_offset;
        empty_offset.data(empty_points);
        ASSERT_FALSE(true);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Zero-length edge should throw  
    try {
        Point same_point(0.0, 0.0);
        Edge zero_edge(same_point, same_point);
        ASSERT_FALSE(true);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    std::cout << "Edge cases tests passed!\n";
}

int main() {
    std::cout << "Running simple polygon offset tests...\n";
    std::cout << "Library version: " << Version::string() << "\n\n";
    
    try {
        test_point();
        test_edge();
        test_offset_basic();
        test_offset_operations();
        test_point_offset();
        test_edge_cases();
        
        std::cout << "\nAll tests passed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
}