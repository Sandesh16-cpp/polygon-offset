#include <gtest/gtest.h>
#include <polygon_offset/edge.hpp>

using namespace polygon_offset;

class EdgeTest : public ::testing::Test {
protected:
    static constexpr double EPSILON = 1e-9;
};

TEST_F(EdgeTest, BasicConstruction) {
    Point p1(0.0, 0.0);
    Point p2(0.0, 5.0);
    
    Edge edge(p1, p2);
    
    EXPECT_EQ(edge.current(), p1);
    EXPECT_EQ(edge.next(), p2);
    EXPECT_DOUBLE_EQ(edge.length(), 5.0);
}

TEST_F(EdgeTest, ZeroLengthEdgeThrows) {
    Point p1(0.0, 0.0);
    Point p2(0.0, 0.0);
    
    EXPECT_THROW(Edge(p1, p2), std::invalid_argument);
}

TEST_F(EdgeTest, NormalVectorsVerticalEdge) {
    // Vertical edge from (0,0) to (0,5)
    Point p1(0.0, 0.0);
    Point p2(0.0, 5.0);
    
    Edge edge(p1, p2);
    
    // For vertical edge going up, inward normal should point left (-1, 0)
    EXPECT_NEAR(edge.in_normal().x(), -1.0, EPSILON);
    EXPECT_NEAR(edge.in_normal().y(), 0.0, EPSILON);
    
    // Outward normal should point right (1, 0)
    EXPECT_NEAR(edge.out_normal().x(), 1.0, EPSILON);
    EXPECT_NEAR(edge.out_normal().y(), 0.0, EPSILON);
}

TEST_F(EdgeTest, NormalVectorsHorizontalEdge) {
    // Horizontal edge from (0,0) to (5,0)
    Point p1(0.0, 0.0);
    Point p2(5.0, 0.0);
    
    Edge edge(p1, p2);
    
    // For horizontal edge going right, inward normal should point up (0, 1)
    EXPECT_NEAR(edge.in_normal().x(), 0.0, EPSILON);
    EXPECT_NEAR(edge.in_normal().y(), 1.0, EPSILON);
    
    // Outward normal should point down (0, -1)
    EXPECT_NEAR(edge.out_normal().x(), 0.0, EPSILON);
    EXPECT_NEAR(edge.out_normal().y(), -1.0, EPSILON);
}

TEST_F(EdgeTest, Direction) {
    Point p1(1.0, 2.0);
    Point p2(4.0, 6.0);
    
    Edge edge(p1, p2);
    
    // Direction should be normalized (3, 4) -> (0.6, 0.8)
    EXPECT_NEAR(edge.direction().x(), 0.6, EPSILON);
    EXPECT_NEAR(edge.direction().y(), 0.8, EPSILON);
    EXPECT_NEAR(edge.direction().length(), 1.0, EPSILON);
}

TEST_F(EdgeTest, OffsetByVector) {
    Point p1(0.0, 0.0);
    Point p2(5.0, 0.0);
    Point offset_vec(2.0, 3.0);
    
    Edge original(p1, p2);
    Edge offset_edge = original.offset(offset_vec);
    
    EXPECT_EQ(offset_edge.current(), Point(2.0, 3.0));
    EXPECT_EQ(offset_edge.next(), Point(7.0, 3.0));
}

TEST_F(EdgeTest, OffsetByCoordinates) {
    Point p1(0.0, 0.0);
    Point p2(5.0, 0.0);
    
    Edge original(p1, p2);
    Edge offset_edge = original.offset(2.0, 3.0);
    
    EXPECT_EQ(offset_edge.current(), Point(2.0, 3.0));
    EXPECT_EQ(offset_edge.next(), Point(7.0, 3.0));
}

TEST_F(EdgeTest, InverseEdge) {
    Point p1(0.0, 0.0);
    Point p2(5.0, 0.0);
    
    Edge original(p1, p2);
    Edge inverse = original.inverse();
    
    EXPECT_EQ(inverse.current(), p2);
    EXPECT_EQ(inverse.next(), p1);
}

TEST_F(EdgeTest, InverseOffset) {
    Point p1(0.0, 0.0);
    Point p2(5.0, 0.0);
    Point offset_vec(1.0, 1.0);
    
    Edge original(p1, p2);
    Edge inverse_offset = original.inverse_offset(offset_vec);
    
    // Should be (5,0) + (1,1) = (6,1) to (0,0) + (1,1) = (1,1)
    EXPECT_EQ(inverse_offset.current(), Point(6.0, 1.0));
    EXPECT_EQ(inverse_offset.next(), Point(1.0, 1.0));
}

TEST_F(EdgeTest, StaticOffsetEdge) {
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);
    
    Edge offset_edge = Edge::offset_edge(p1, p2, 1.0, 1.0);
    
    EXPECT_EQ(offset_edge.current(), Point(1.0, 1.0));
    EXPECT_EQ(offset_edge.next(), Point(4.0, 5.0));
}

TEST_F(EdgeTest, NormalVectorsAreNormalized) {
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);  // Length = 5
    
    Edge edge(p1, p2);
    
    EXPECT_NEAR(edge.in_normal().length(), 1.0, EPSILON);
    EXPECT_NEAR(edge.out_normal().length(), 1.0, EPSILON);
}

TEST_F(EdgeTest, NormalVectorsArePerpendicular) {
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);
    
    Edge edge(p1, p2);
    
    // Check that normals are perpendicular to direction
    EXPECT_NEAR(edge.direction().dot(edge.in_normal()), 0.0, EPSILON);
    EXPECT_NEAR(edge.direction().dot(edge.out_normal()), 0.0, EPSILON);
}

TEST_F(EdgeTest, NormalVectorsAreOpposite) {
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);
    
    Edge edge(p1, p2);
    
    // Out normal should be negative of in normal
    EXPECT_NEAR(edge.out_normal().x(), -edge.in_normal().x(), EPSILON);
    EXPECT_NEAR(edge.out_normal().y(), -edge.in_normal().y(), EPSILON);
}