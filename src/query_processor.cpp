#include "../include/query_processor.h"
#include <algorithm>
#include <unordered_set>
#include <future>

QueryProcessor::QueryProcessor(size_t numThreads) : threadPool(numThreads) {
}

QueryProcessor::~QueryProcessor() {
}

std::vector<QueryResult> QueryProcessor::processQuery(
    const std::vector<Customer>& customers,
    const std::vector<Order>& orders,
    const std::vector<LineItem>& lineItems,
    const std::vector<Supplier>& suppliers,
    const std::vector<Nation>& nations,
    const std::vector<Region>& regions
) {
    // Check if we have valid data
    if (customers.empty() || orders.empty() || lineItems.empty() || 
        suppliers.empty() || nations.empty() || regions.empty()) {
        return {};
    }
    
    // Build indexes for efficient joins
    auto orderToCustomer = buildOrderToCustomerIndex(orders);
    auto supplierToNation = buildSupplierToNationIndex(suppliers);
    auto validCustomerNations = buildValidCustomerNationsIndex(customers, suppliers);
    auto nationNameIndex = buildNationNameIndex(nations);
    
    // Determine the number of threads and chunk size
    size_t numThreads = threadPool.size();
    size_t chunkSize = (lineItems.size() + numThreads - 1) / numThreads;
    
    // Process data in parallel
    std::vector<std::future<std::unordered_map<int32_t, double>>> futures;
    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = std::min(start + chunkSize, lineItems.size());
        
        if (start >= lineItems.size()) {
            break;
        }
        
        futures.push_back(
            threadPool.enqueue(
                &QueryProcessor::processChunk,
                this,
                std::ref(lineItems),
                start,
                end,
                std::ref(orderToCustomer),
                std::ref(supplierToNation),
                std::ref(validCustomerNations)
            )
        );
    }
    
    // Collect results from all threads
    std::vector<std::unordered_map<int32_t, double>> partialResults;
    for (auto& future : futures) {
        partialResults.push_back(future.get());
    }
    
    // Merge results
    auto nationRevenues = mergeResults(partialResults);
    
    // Convert to result format and sort
    std::vector<QueryResult> results;
    for (const auto& [nationKey, revenue] : nationRevenues) {
        if (nationNameIndex.find(nationKey) != nationNameIndex.end()) {
            results.emplace_back(nationNameIndex[nationKey], revenue);
        }
    }
    
    // Sort by revenue in descending order
    std::sort(results.begin(), results.end());
    
    return results;
}

std::unordered_map<int32_t, double> QueryProcessor::processChunk(
    const std::vector<LineItem>& lineItems,
    size_t start,
    size_t end,
    const std::unordered_map<int32_t, int32_t>& orderToCustomer,
    const std::unordered_map<int32_t, int32_t>& supplierToNation,
    const std::unordered_map<int32_t, bool>& validCustomerNations
) {
    std::unordered_map<int32_t, double> nationRevenues;
    
    for (size_t i = start; i < end; ++i) {
        const auto& lineItem = lineItems[i];
        
        // Check if this line item's order exists in our filtered orders
        auto orderIt = orderToCustomer.find(lineItem.l_orderkey);
        if (orderIt == orderToCustomer.end()) {
            continue;
        }
        
        int32_t custkey = orderIt->second;
        
        // Check if this line item's supplier exists and get its nation
        auto supplierIt = supplierToNation.find(lineItem.l_suppkey);
        if (supplierIt == supplierToNation.end()) {
            continue;
        }
        
        int32_t nationkey = supplierIt->second;
        
        // Check if customer and supplier are from the same nation
        // and if that nation is in the specified region
        auto validIt = validCustomerNations.find(custkey);
        if (validIt == validCustomerNations.end() || !validIt->second) {
            continue;
        }
        
        // Calculate revenue and add to the nation's total
        double revenue = lineItem.revenue();
        nationRevenues[nationkey] += revenue;
    }
    
    return nationRevenues;
}

std::unordered_map<int32_t, int32_t> QueryProcessor::buildOrderToCustomerIndex(const std::vector<Order>& orders) {
    std::unordered_map<int32_t, int32_t> orderToCustomer;
    for (const auto& order : orders) {
        orderToCustomer[order.o_orderkey] = order.o_custkey;
    }
    return orderToCustomer;
}

std::unordered_map<int32_t, int32_t> QueryProcessor::buildSupplierToNationIndex(const std::vector<Supplier>& suppliers) {
    std::unordered_map<int32_t, int32_t> supplierToNation;
    for (const auto& supplier : suppliers) {
        supplierToNation[supplier.s_suppkey] = supplier.s_nationkey;
    }
    return supplierToNation;
}

std::unordered_map<int32_t, bool> QueryProcessor::buildValidCustomerNationsIndex(
    const std::vector<Customer>& customers,
    const std::vector<Supplier>& suppliers
) {
    // First, build a set of valid nation keys (nations that have suppliers)
    std::unordered_set<int32_t> validNations;
    for (const auto& supplier : suppliers) {
        validNations.insert(supplier.s_nationkey);
    }
    
    // Then, mark customers from those nations
    std::unordered_map<int32_t, bool> validCustomers;
    for (const auto& customer : customers) {
        validCustomers[customer.c_custkey] = (validNations.find(customer.c_nationkey) != validNations.end());
    }
    
    return validCustomers;
}

std::unordered_map<int32_t, std::string> QueryProcessor::buildNationNameIndex(const std::vector<Nation>& nations) {
    std::unordered_map<int32_t, std::string> nationNames;
    for (const auto& nation : nations) {
        nationNames[nation.n_nationkey] = nation.n_name;
    }
    return nationNames;
}

std::unordered_map<int32_t, double> QueryProcessor::mergeResults(
    const std::vector<std::unordered_map<int32_t, double>>& partialResults
) {
    std::unordered_map<int32_t, double> mergedResults;
    
    for (const auto& partialResult : partialResults) {
        for (const auto& [nationKey, revenue] : partialResult) {
            mergedResults[nationKey] += revenue;
        }
    }
    
    return mergedResults;
}
