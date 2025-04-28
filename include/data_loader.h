#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "data_types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

class DataLoader {
public:
    // Load data from TPCH files
    static std::vector<Customer> loadCustomers(const std::string& filePath);
    static std::vector<Order> loadOrders(const std::string& filePath, const Date& dateFrom, const Date& dateTo);
    static std::vector<LineItem> loadLineItems(const std::string& filePath);
    static std::vector<Supplier> loadSuppliers(const std::string& filePath);
    static std::vector<Nation> loadNations(const std::string& filePath);
    static std::vector<Region> loadRegions(const std::string& filePath, const std::string& regionName);
    
private:
    // Helper function to split a string by delimiter
    static std::vector<std::string> splitLine(const std::string& line, char delimiter);
    
    // Helper function to trim whitespace from a string
    static std::string trim(const std::string& str);
    
    // Helper function to parse a date string
    static Date parseDate(const std::string& dateStr);
};

#endif // DATA_LOADER_H