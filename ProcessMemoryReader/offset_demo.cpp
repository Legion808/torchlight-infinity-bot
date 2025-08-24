#include "OffsetExamples.h"
#include "Memory.h"
#include "Process.h"
#include <iostream>

int main() {
    std::cout << "DYNAMIC OFFSET IMPLEMENTATION TUTORIAL" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "This demonstrates how to implement: new_address = base_address + offset" << std::endl;
    std::cout << "And how to adjust offsets dynamically at runtime." << std::endl;
    std::cout << std::endl;
    
    // Create memory and process objects
    Process process;
    Memory memory(&process);
    
    // Create examples class
    OffsetExamples examples(&memory);
    
    // Run all examples
    examples.runAllExamples();
    
    std::cout << std::endl;
    std::cout << "KEY CONCEPTS DEMONSTRATED:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "1. Basic formula: new_address = base_address + offset" << std::endl;
    std::cout << "2. Dynamic offset adjustment for array elements" << std::endl;
    std::cout << "3. Multi-level pointer chains" << std::endl;
    std::cout << "4. Runtime offset updates" << std::endl;
    std::cout << "5. Address validation" << std::endl;
    
    return 0;
}
