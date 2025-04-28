#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include "data_types.h"
#include "thread_pool.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

class QueryProcessor {
public:
    // Constructor
    QueryProcessor(size_t numThreads);
    
    // Destructor
    ~QueryProcessor();
    
    // Process TPCH Query 5
    std::vector<QueryResult> processQuery(
        const std::vector<Customer>& customers,
        const std::vector<Order>& orders,
        const std::vector<LineItem>& lineItems,
        const std::vector<Supplier>& suppliers,
        const std::vector<Nation>& nations,
        const std::vector<Region>& regions
    );
    
private:
    // Thread pool for parallel processing
    ThreadPool threadPool;
    
    // Process a chunk of line items
    std::unordered_map<int32_t, double> processChunk(
        const std::vector<LineItem>& lineItems,
        size_t start,
        size_t end,
        const std::unordered_map<int32_t, int32_t>& orderToCustomer,
        const std::unordered_map<int32_t, int32_t>& supplierToNation,
        const std::unordered_map<int32_t, bool>& validCustomerNations
    );
    
    // Build indexes for efficient joins
    std::unordered_map<int32_t, int32_t> buildOrderToCustomerIndex(const std::vector<Order>& orders);
    std::unordered_map<int32_t, int32_t> buildSupplierToNationIndex(const std::vector<Supplier>& suppliers);
    std::unordered_map<int32_t, bool> buildValidCustomerNationsIndex(
        const std::vector<Customer>& customers,
        const std::vector<Supplier>& suppliers
    );
    std::unordered_map<int32_t, std::string> buildNationNameIndex(const std::vector<Nation>& nations);
    
    // Merge partial results from different threads
    std::unordered_map<int32_t, double> mergeResults(
        const std::vector<std::unordered_map<int32_t, double>>& partialResults
    );
};

#endif // QUERY_PROCESSOR_H