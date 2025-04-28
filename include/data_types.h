#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

// Date structure for efficient date comparison
struct Date {
    int year;
    int month;
    int day;
    
    Date() : year(0), month(0), day(0) {}
    
    Date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    // Parse date from string in format YYYY-MM-DD
    static Date fromString(const std::string& dateStr) {
        Date date;
        if (dateStr.length() >= 10) {
            date.year = std::stoi(dateStr.substr(0, 4));
            date.month = std::stoi(dateStr.substr(5, 2));
            date.day = std::stoi(dateStr.substr(8, 2));
        }
        return date;
    }
    
    // Compare dates
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    bool operator>=(const Date& other) const {
        return !(*this < other);
    }
    
    bool operator<=(const Date& other) const {
        return (*this < other) || (*this == other);
    }
    
    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
};

// TPCH table structures
struct Customer {
    int32_t c_custkey;
    int32_t c_nationkey;
    
    Customer() : c_custkey(0), c_nationkey(0) {}
    Customer(int32_t custkey, int32_t nationkey) : c_custkey(custkey), c_nationkey(nationkey) {}
};

struct Order {
    int32_t o_orderkey;
    int32_t o_custkey;
    Date o_orderdate;
    
    Order() : o_orderkey(0), o_custkey(0) {}
    Order(int32_t orderkey, int32_t custkey, const Date& orderdate) 
        : o_orderkey(orderkey), o_custkey(custkey), o_orderdate(orderdate) {}
};

struct LineItem {
    int32_t l_orderkey;
    int32_t l_suppkey;
    double l_extendedprice;
    double l_discount;
    
    LineItem() : l_orderkey(0), l_suppkey(0), l_extendedprice(0.0), l_discount(0.0) {}
    LineItem(int32_t orderkey, int32_t suppkey, double extendedprice, double discount)
        : l_orderkey(orderkey), l_suppkey(suppkey), l_extendedprice(extendedprice), l_discount(discount) {}
    
    // Calculate revenue for this line item
    double revenue() const {
        return l_extendedprice * (1.0 - l_discount);
    }
};

struct Supplier {
    int32_t s_suppkey;
    int32_t s_nationkey;
    
    Supplier() : s_suppkey(0), s_nationkey(0) {}
    Supplier(int32_t suppkey, int32_t nationkey) : s_suppkey(suppkey), s_nationkey(nationkey) {}
};

struct Nation {
    int32_t n_nationkey;
    std::string n_name;
    int32_t n_regionkey;
    
    Nation() : n_nationkey(0), n_regionkey(0) {}
    Nation(int32_t nationkey, const std::string& name, int32_t regionkey)
        : n_nationkey(nationkey), n_name(name), n_regionkey(regionkey) {}
};

struct Region {
    int32_t r_regionkey;
    std::string r_name;
    
    Region() : r_regionkey(0) {}
    Region(int32_t regionkey, const std::string& name) : r_regionkey(regionkey), r_name(name) {}
};

// Result structure
struct QueryResult {
    std::string nation;
    double revenue;
    
    QueryResult() : revenue(0.0) {}
    QueryResult(const std::string& n, double r) : nation(n), revenue(r) {}
    
    bool operator<(const QueryResult& other) const {
        return revenue > other.revenue; // For descending order
    }
};

#endif // DATA_TYPES_H