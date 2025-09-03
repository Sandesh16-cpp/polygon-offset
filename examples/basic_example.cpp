#include <polygon_offset/polygon_offset.hpp>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace polygon_offset;

void print_ring(const Ring& ring, const std::string& label) {
    std::cout << label << ":\n";
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& point : ring) {
        std::cout << "  [" << point.x() << ", " << point.y() << "]\n";
    }
    std::cout << "\n";
}

void print_multipolygon(const MultiPolygon& multipoly, const std::string& label) {
    std::cout << label << " (containing " << multipoly.size() << " polygon(s)):\n";
    for (size_t i = 0; i < multipoly.size(); ++i) {
        std::cout << "Polygon " << i << ":\n";
        for (size_t j = 0; j < multipoly[i].size(); ++j) {
            print_ring(multipoly[i][j], "  Ring " + std::to_string(j));
        }
    }
}

int main() {
    std::cout << "Polygon Offset Library v" << Version::string() << "\n";
    std::cout << "==============================================\n\n";

    // Example 1: Basic square polygon
    std::cout << "Example 1: Square Polygon Offset\n";
    std::cout << "---------------------------------\n";
    
    Ring square = {
        {0.0, 0.0}, {0.0, 100.0}, {100.0, 100.0}, {100.0, 0.0}, {0.0, 0.0}
    };
    
    print_ring(square, "Original Square");
    
    Offset offset_processor;
    
    // Margin (outward offset)
    auto margin_result = offset_processor.data(square).margin(10.0);
    print_multipolygon(margin_result, "Margin (+10)");
    
    // Padding (inward offset)
    auto padding_result = offset_processor.data(square).padding(10.0);
    print_multipolygon(padding_result, "Padding (+10)");
    
    // Example 2: Point offset (circle generation)
    std::cout << "Example 2: Point Offset (Circle)\n";
    std::cout << "--------------------------------\n";
    
    Point center(50.0, 50.0);
    Offset point_processor(center);
    
    auto circle_result = point_processor.arc_segments(8).offset(25.0);
    print_multipolygon(circle_result, "Circle (radius 25)");
    
    // Example 3: Polyline offset
    std::cout << "Example 3: Polyline Offset\n";
    std::cout << "--------------------------\n";
    
    Ring line = {
        {0.0, 50.0}, {50.0, 50.0}, {100.0, 100.0}
    };
    
    print_ring(line, "Original Polyline");
    
    auto line_result = offset_processor.data(line).offset_line(5.0);
    print_multipolygon(line_result, "Offset Polyline (+5)");
    
    // Example 4: Fluent interface
    std::cout << "Example 4: Fluent Interface\n";
    std::cout << "---------------------------\n";
    
    auto fluent_result = Offset()
        .data(square)
        .arc_segments(3)
        .offset(5.0);
    
    print_multipolygon(fluent_result, "Fluent Interface Result");
    
    // Example 5: Different arc segment settings
    std::cout << "Example 5: Arc Segment Comparison\n";
    std::cout << "---------------------------------\n";
    
    auto low_detail = Offset(square).arc_segments(1).margin(15.0);
    auto high_detail = Offset(square).arc_segments(10).margin(15.0);
    
    std::cout << "Low detail (1 segment): " << 
        (low_detail.empty() ? 0 : low_detail[0][0].size()) << " vertices\n";
    std::cout << "High detail (10 segments): " << 
        (high_detail.empty() ? 0 : high_detail[0][0].size()) << " vertices\n\n";
    
    std::cout << "Examples completed successfully!\n";
    return 0;
}