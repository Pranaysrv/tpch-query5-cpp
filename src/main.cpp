#include "../include/data_types.h"
#include "../include/data_loader.h"
#include "../include/query_processor.h"
#include "../include/thread_pool.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>

// Function to print usage information
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n"
              << "Options:\n"
              << "  --customer-path PATH     Path to customer.tbl file\n"
              << "  --orders-path PATH       Path to orders.tbl file\n"
              << "  --lineitem-path PATH     Path to lineitem.tbl file\n"
              << "  --supplier-path PATH     Path to supplier.tbl file\n"
              << "  --nation-path PATH       Path to nation.tbl file\n"
              << "  --region-path PATH       Path to region.tbl file\n"
              << "  --region-name NAME       Region name filter (default: ASIA)\n"
              << "  --date-from DATE         Start date filter (format: YYYY-MM-DD, default: 1994-01-01)\n"
              << "  --date-to DATE           End date filter (format: YYYY-MM-DD, default: 1995-01-01)\n"
              << "  --threads NUM            Number of threads to use (default: number of CPU cores)\n"
              << "  --output PATH            Path to output file (default: stdout)\n"
              << "  --help                   Display this help message\n";
}

int main(int argc, char* argv[]) {
    // Default parameter values
    std::string customerPath;
    std::string ordersPath;
    std::string lineitemPath;
    std::string supplierPath;
    std::string nationPath;
    std::string regionPath;
    std::string regionName = "ASIA";
    std::string dateFromStr = "1994-01-01";
    std::string dateToStr = "1995-01-01";
    std::string outputPath;
    size_t numThreads = std::thread::hardware_concurrency();
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--customer-path" && i + 1 < argc) {
            customerPath = argv[++i];
        } else if (arg == "--orders-path" && i + 1 < argc) {
            ordersPath = argv[++i];
        } else if (arg == "--lineitem-path" && i + 1 < argc) {
            lineitemPath = argv[++i];
        } else if (arg == "--supplier-path" && i + 1 < argc) {
            supplierPath = argv[++i];
        } else if (arg == "--nation-path" && i + 1 < argc) {
            nationPath = argv[++i];
        } else if (arg == "--region-path" && i + 1 < argc) {
            regionPath = argv[++i];
        } else if (arg == "--region-name" && i + 1 < argc) {
            regionName = argv[++i];
        } else if (arg == "--date-from" && i + 1 < argc) {
            dateFromStr = argv[++i];
        } else if (arg == "--date-to" && i + 1 < argc) {
            dateToStr = argv[++i];
        } else if (arg == "--threads" && i + 1 < argc) {
            numThreads = std::stoul(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputPath = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Check required parameters
    if (customerPath.empty() || ordersPath.empty() || lineitemPath.empty() || 
        supplierPath.empty() || nationPath.empty() || regionPath.empty()) {
        std::cerr << "Error: Missing required file paths" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse dates
    Date dateFrom = Date::fromString(dateFromStr);
    Date dateTo = Date::fromString(dateToStr);
    
    std::cout << "Loading data..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Load data
    auto customers = DataLoader::loadCustomers(customerPath);
    std::cout << "Loaded " << customers.size() << " customers" << std::endl;
    
    auto orders = DataLoader::loadOrders(ordersPath, dateFrom, dateTo);
    std::cout << "Loaded " << orders.size() << " orders" << std::endl;
    
    auto lineItems = DataLoader::loadLineItems(lineitemPath);
    std::cout << "Loaded " << lineItems.size() << " line items" << std::endl;
    
    auto suppliers = DataLoader::loadSuppliers(supplierPath);
    std::cout << "Loaded " << suppliers.size() << " suppliers" << std::endl;
    
    auto nations = DataLoader::loadNations(nationPath);
    std::cout << "Loaded " << nations.size() << " nations" << std::endl;
    
    auto regions = DataLoader::loadRegions(regionPath, regionName);
    std::cout << "Loaded " << regions.size() << " regions" << std::endl;
    
    auto loadTime = std::chrono::high_resolution_clock::now();
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadTime - startTime);
    std::cout << "Data loading completed in " << loadDuration.count() << " ms" << std::endl;
    
    // Process query
    std::cout << "Processing query with " << numThreads << " threads..." << std::endl;
    QueryProcessor processor(numThreads);
    auto results = processor.processQuery(customers, orders, lineItems, suppliers, nations, regions);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto queryDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - loadTime);
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Query processing completed in " << queryDuration.count() << " ms" << std::endl;
    std::cout << "Total execution time: " << totalDuration.count() << " ms" << std::endl;
    
    // Output results
    std::ostream* out = &std::cout;
    std::ofstream outFile;
    
    if (!outputPath.empty()) {
        outFile.open(outputPath);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open output file: " << outputPath << std::endl;
            return 1;
        }
        out = &outFile;
    }
    
    // Print results
    *out << "n_name,revenue" << std::endl;
    for (const auto& result : results) {
        *out << "'" << result.nation << "'," << std::fixed << std::setprecision(4) << result.revenue << std::endl;
    }
    
    if (!outputPath.empty()) {
        outFile.close();
        std::cout << "Results written to " << outputPath << std::endl;
    }
    
    return 0;
}