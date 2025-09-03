#include <polygon_offset/polygon_offset.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <functional>

using namespace polygon_offset;

Ring generate_random_polygon(size_t num_vertices, double radius = 100.0) {
    Ring polygon;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> angle_dist(0.0, 2.0 * std::numbers::pi);
    std::uniform_real_distribution<> radius_dist(radius * 0.5, radius);
    
    for (size_t i = 0; i < num_vertices; ++i) {
        double angle = angle_dist(gen);
        double r = radius_dist(gen);
        polygon.emplace_back(r * std::cos(angle), r * std::sin(angle));
    }
    
    // Close the polygon
    if (!polygon.empty()) {
        polygon.push_back(polygon.front());
    }
    
    return polygon;
}

void benchmark_operation(const std::string& name, 
                        const std::function<void()>& operation,
                        int iterations = 1000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        operation();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << name << ": " 
              << duration.count() / iterations << " μs/operation" 
              << " (total: " << duration.count() / 1000.0 << " ms for " 
              << iterations << " iterations)\n";
}

int main() {
    std::cout << "Polygon Offset Performance Benchmark\n";
    std::cout << "====================================\n\n";
    
    // Test with different polygon sizes
    std::vector<size_t> polygon_sizes = {10, 50, 100, 500, 1000};
    
    for (size_t size : polygon_sizes) {
        std::cout << "Testing with " << size << " vertex polygon:\n";
        std::cout << "----------------------------------------\n";
        
        auto polygon = generate_random_polygon(size);
        
        // Benchmark margin operation
        benchmark_operation(
            "Margin operation", 
            [&]() { 
                [[maybe_unused]] auto result = Offset(polygon).margin(10.0); 
            }, 
            100
        );
        
        // Benchmark padding operation
        benchmark_operation(
            "Padding operation", 
            [&]() { 
                [[maybe_unused]] auto result = Offset(polygon).padding(10.0); 
            }, 
            100
        );
        
        // Benchmark offset line operation
        benchmark_operation(
            "Offset line operation", 
            [&]() { 
                [[maybe_unused]] auto result = Offset(polygon).offset_line(10.0); 
            }, 
            100
        );
        
        // Benchmark with different arc segments
        benchmark_operation(
            "High detail (20 arc segments)", 
            [&]() { 
                [[maybe_unused]] auto result = Offset(polygon).arc_segments(20).margin(10.0); 
            }, 
            50
        );
        
        std::cout << "\n";
    }
    
    // Test point offset performance
    std::cout << "Point offset performance:\n";
    std::cout << "------------------------\n";
    
    Point center(0.0, 0.0);
    
    benchmark_operation(
        "Point offset (8 segments)", 
        [&]() { 
            [[maybe_unused]] auto result = Offset(center).arc_segments(8).offset(50.0); 
        }, 
        1000
    );
    
    benchmark_operation(
        "Point offset (32 segments)", 
        [&]() { 
            [[maybe_unused]] auto result = Offset(center).arc_segments(32).offset(50.0); 
        }, 
        1000
    );
    
    // Memory usage test
    std::cout << "\nMemory usage test:\n";
    std::cout << "-----------------\n";
    
    const size_t large_polygon_size = 10000;
    auto large_polygon = generate_random_polygon(large_polygon_size);
    
    std::cout << "Large polygon (" << large_polygon_size << " vertices) created.\n";
    
    auto start_memory = std::chrono::high_resolution_clock::now();
    auto result = Offset(large_polygon).margin(5.0);
    auto end_memory = std::chrono::high_resolution_clock::now();
    
    auto memory_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_memory - start_memory);
    
    std::cout << "Large polygon offset completed in " 
              << memory_duration.count() << " ms\n";
    
    size_t total_vertices = 0;
    for (const auto& polygon : result) {
        for (const auto& ring : polygon) {
            total_vertices += ring.size();
        }
    }
    
    std::cout << "Result contains " << total_vertices << " total vertices across "
              << result.size() << " polygons\n";
    
    std::cout << "\nPerformance test completed!\n";
    return 0;
}