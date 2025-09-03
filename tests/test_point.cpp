#include <gtest/gtest.h>
#include <polygon_offset/point.hpp>
#include <cmath>

using namespace polygon_offset;

class PointTest : public ::testing::Test {
protected:
    static constexpr double EPSILON = 1e-9;
};

TEST_F(PointTest, DefaultConstructor) {
    Point p;
    EXPECT_DOUBLE_EQ(p.x(), 0.0);
    EXPECT_DOUBLE_EQ(p.y(), 0.0);
}

TEST_F(PointTest, ParameterizedConstructor) {
    Point p(3.0, 4.0);
    EXPECT_DOUBLE_EQ(p.x(), 3.0);
    EXPECT_DOUBLE_EQ(p.y(), 4.0);
}

TEST_F(PointTest, CopyConstructor) {
    Point p1(1.0, 2.0);
    Point p2(p1);
    EXPECT_DOUBLE_EQ(p2.x(), 1.0);
    EXPECT_DOUBLE_EQ(p2.y(), 2.0);
}

TEST_F(PointTest, Assignment) {
    Point p1(1.0, 2.0);
    Point p2;
    p2 = p1;
    EXPECT_DOUBLE_EQ(p2.x(), 1.0);
    EXPECT_DOUBLE_EQ(p2.y(), 2.0);
}

TEST_F(PointTest, ArrayAccess) {
    Point p(3.0, 4.0);
    EXPECT_DOUBLE_EQ(p[0], 3.0);
    EXPECT_DOUBLE_EQ(p[1], 4.0);
    
    p[0] = 5.0;
    p[1] = 6.0;
    EXPECT_DOUBLE_EQ(p.x(), 5.0);
    EXPECT_DOUBLE_EQ(p.y(), 6.0);
}

TEST_F(PointTest, SettersGetters) {
    Point p;
    p.set_x(7.0);
    p.set_y(8.0);
    EXPECT_DOUBLE_EQ(p.x(), 7.0);
    EXPECT_DOUBLE_EQ(p.y(), 8.0);
}

TEST_F(PointTest, Addition) {
    Point p1(1.0, 2.0);
    Point p2(3.0, 4.0);
    Point result = p1 + p2;
    EXPECT_DOUBLE_EQ(result.x(), 4.0);
    EXPECT_DOUBLE_EQ(result.y(), 6.0);
    
    p1 += p2;
    EXPECT_DOUBLE_EQ(p1.x(), 4.0);
    EXPECT_DOUBLE_EQ(p1.y(), 6.0);
}

TEST_F(PointTest, Subtraction) {
    Point p1(5.0, 6.0);
    Point p2(2.0, 3.0);
    Point result = p1 - p2;
    EXPECT_DOUBLE_EQ(result.x(), 3.0);
    EXPECT_DOUBLE_EQ(result.y(), 3.0);
    
    p1 -= p2;
    EXPECT_DOUBLE_EQ(p1.x(), 3.0);
    EXPECT_DOUBLE_EQ(p1.y(), 3.0);
}

TEST_F(PointTest, ScalarMultiplication) {
    Point p(2.0, 3.0);
    Point result1 = p * 2.0;
    Point result2 = 2.0 * p;
    
    EXPECT_DOUBLE_EQ(result1.x(), 4.0);
    EXPECT_DOUBLE_EQ(result1.y(), 6.0);
    EXPECT_DOUBLE_EQ(result2.x(), 4.0);
    EXPECT_DOUBLE_EQ(result2.y(), 6.0);
    
    p *= 3.0;
    EXPECT_DOUBLE_EQ(p.x(), 6.0);
    EXPECT_DOUBLE_EQ(p.y(), 9.0);
}

TEST_F(PointTest, ScalarDivision) {
    Point p(6.0, 8.0);
    Point result = p / 2.0;
    EXPECT_DOUBLE_EQ(result.x(), 3.0);
    EXPECT_DOUBLE_EQ(result.y(), 4.0);
    
    p /= 2.0;
    EXPECT_DOUBLE_EQ(p.x(), 3.0);
    EXPECT_DOUBLE_EQ(p.y(), 4.0);
}

TEST_F(PointTest, Negation) {
    Point p(3.0, -4.0);
    Point result = -p;
    EXPECT_DOUBLE_EQ(result.x(), -3.0);
    EXPECT_DOUBLE_EQ(result.y(), 4.0);
}

TEST_F(PointTest, DotProduct) {
    Point p1(2.0, 3.0);
    Point p2(4.0, 5.0);
    double dot = p1.dot(p2);
    EXPECT_DOUBLE_EQ(dot, 23.0);  // 2*4 + 3*5 = 8 + 15 = 23
}

TEST_F(PointTest, CrossProduct) {
    Point p1(2.0, 3.0);
    Point p2(4.0, 5.0);
    double cross = p1.cross(p2);
    EXPECT_DOUBLE_EQ(cross, -2.0);  // 2*5 - 3*4 = 10 - 12 = -2
}

TEST_F(PointTest, Length) {
    Point p(3.0, 4.0);
    EXPECT_DOUBLE_EQ(p.length_squared(), 25.0);
    EXPECT_DOUBLE_EQ(p.length(), 5.0);
}

TEST_F(PointTest, Normalization) {
    Point p(3.0, 4.0);
    Point normalized = p.normalized();
    EXPECT_NEAR(normalized.length(), 1.0, EPSILON);
    EXPECT_NEAR(normalized.x(), 0.6, EPSILON);
    EXPECT_NEAR(normalized.y(), 0.8, EPSILON);
}

TEST_F(PointTest, ZeroVectorNormalization) {
    Point p(0.0, 0.0);
    Point normalized = p.normalized();
    EXPECT_DOUBLE_EQ(normalized.x(), 0.0);
    EXPECT_DOUBLE_EQ(normalized.y(), 0.0);
}

TEST_F(PointTest, Perpendicular) {
    Point p(1.0, 0.0);
    Point perp = p.perpendicular();
    EXPECT_DOUBLE_EQ(perp.x(), 0.0);
    EXPECT_DOUBLE_EQ(perp.y(), 1.0);
    
    Point p2(0.0, 1.0);
    Point perp2 = p2.perpendicular();
    EXPECT_DOUBLE_EQ(perp2.x(), -1.0);
    EXPECT_DOUBLE_EQ(perp2.y(), 0.0);
}

TEST_F(PointTest, Equality) {
    Point p1(1.0, 2.0);
    Point p2(1.0, 2.0);
    Point p3(1.1, 2.0);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_FALSE(p1 != p2);
    EXPECT_TRUE(p1 != p3);
}

TEST_F(PointTest, ApproximateEquality) {
    Point p1(1.0, 2.0);
    Point p2(1.0000000001, 2.0000000001);
    Point p3(1.1, 2.0);
    
    EXPECT_TRUE(p1.equals(p2));
    EXPECT_FALSE(p1.equals(p3));
    EXPECT_TRUE(p1.equals(p3, 0.2));
}