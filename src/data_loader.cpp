#include "../include/data_loader.h"

std::vector<Customer> DataLoader::loadCustomers(const std::string& filePath) {
    std::vector<Customer> customers;
    std::ifstream file(filePath);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open customer file: " << filePath << std::endl;
        return customers;
    }
    
    while (std::getline(file, line)) {
        auto fields = splitLine(line, '|');
        if (fields.size() >= 4) {
            int32_t custkey = std::stoi(fields[0]);
            int32_t nationkey = std::stoi(fields[3]);
            customers.emplace_back(custkey, nationkey);
        }
    }
    
    return customers;
}

std::vector<Order> DataLoader::loadOrders(const std::string& filePath, const Date& dateFrom, const Date& dateTo) {
    std::vector<Order> orders;
    std::ifstream file(filePath);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open orders file: " << filePath << std::endl;
        return orders;
    }
    
    while (std::getline(file, line)) {
        auto fields = splitLine(line, '|');
        if (fields.size() >= 5) {
            int32_t orderkey = std::stoi(fields[0]);
            int32_t custkey = std::stoi(fields[1]);
            Date orderdate = parseDate(fields[4]);
            
            // Filter by date range
            if (orderdate >= dateFrom && orderdate < dateTo) {
                orders.emplace_back(orderkey, custkey, orderdate);
            }
        }
    }
    
    return orders;
}

std::vector<LineItem> DataLoader::loadLineItems(const std::string& filePath) {
    std::vector<LineItem> lineItems;
    std::ifstream file(filePath);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open lineitem file: " << filePath << std::endl;
        return lineItems;
    }
    
    while (std::getline(file, line)) {
        auto fields = splitLine(line, '|');
        if (fields.size() >= 6) {
            int32_t orderkey = std::stoi(fields[0]);
            int32_t suppkey = std::stoi(fields[2]);
            double extendedprice = std::stod(fields[5]);
            double discount = std::stod(fields[6]);
            
            lineItems.emplace_back(orderkey, suppkey, extendedprice, discount);
        }
    }
    
    return lineItems;
}

std::vector<Supplier> DataLoader::loadSuppliers(const std::string& filePath) {
    std::vector<Supplier> suppliers;
    std::ifstream file(filePath);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open supplier file: " << filePath << std::endl;
        return suppliers;
    }
    
    while (std::getline(file, line)) {
        auto fields = splitLine(line, '|');
        if (fields.size() >= 4) {
            int32_t suppkey = std::stoi(fields[0]);
            int32_t nationkey = std::stoi(fields[3]);
            
            suppliers.emplace_back(suppkey, nationkey);
        }
    }
    
    return suppliers;
}

std::vector<Nation> DataLoader::loadNations(const std::string& filePath) {
    std::vector<Nation> nations;
    std::ifstream file(filePath);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open nation file: " << filePath << std::endl;
        return nations;
    }
    
    while (std::getline(file, line)) {
        auto fields = splitLine(line, '|');
        if (fields.size() >= 3) {
            int32_t nationkey = std::stoi(fields[0]);
            std::string name = trim(fields[1]);
            int32_t regionkey = std::stoi(fields[2]);
            
            nations.emplace_back(nationkey, name, regionkey);
        }
    }
    
    return nations;
}

std::vector<Region> DataLoader::loadRegions(const std::string& filePath, const std::string& regionName) {
    std::vector<Region> regions;
    std::ifstream file(filePath);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open region file: " << filePath << std::endl;
        return regions;
    }
    
    while (std::getline(file, line)) {
        auto fields = splitLine(line, '|');
        if (fields.size() >= 2) {
            int32_t regionkey = std::stoi(fields[0]);
            std::string name = trim(fields[1]);
            
            // Filter by region name if specified
            if (regionName.empty() || name == regionName) {
                regions.emplace_back(regionkey, name);
            }
        }
    }
    
    return regions;
}

std::vector<std::string> DataLoader::splitLine(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string DataLoader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, last - first + 1);
}

Date DataLoader::parseDate(const std::string& dateStr) {
    // Remove quotes if present
    std::string cleanDateStr = dateStr;
    if (!cleanDateStr.empty() && cleanDateStr.front() == '\'') {
        cleanDateStr = cleanDateStr.substr(1);
    }
    if (!cleanDateStr.empty() && cleanDateStr.back() == '\'') {
        cleanDateStr = cleanDateStr.substr(0, cleanDateStr.size() - 1);
    }
    
    return Date::fromString(cleanDateStr);
}