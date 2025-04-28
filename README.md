# TPCH Query 5 Multithreaded Implementation

This project implements TPCH Query 5 using multithreading in C++ without utilizing a database management system.

## Overview

The implementation processes TPCH data (customer, orders, lineitem, supplier, nation, region) to calculate the local supplier volume as specified in Query 5:

```sql
select
  n_name,
  sum(l_extendedprice * (1 - l_discount)) as revenue
from
  customer,
  orders,
  lineitem,
  supplier,
  nation,
  region
where
  c_custkey = o_custkey
  and l_orderkey = o_orderkey
  and l_suppkey = s_suppkey
  and c_nationkey = s_nationkey
  and s_nationkey = n_nationkey
  and n_regionkey = r_regionkey
  and r_name = 'ASIA'
  and o_orderdate >= '1994-01-01'
  and o_orderdate < '1995-01-01'
group by
  n_name
order by
  revenue desc
```

## Features

- Multithreaded processing for improved performance
- Command-line interface for specifying parameters
- Efficient memory usage and optimization
- Docker support for cross-platform compatibility

## Requirements

- C++17 compatible compiler
- CMake 3.10+ or GNU Make
- TPCH data files (generated using TPCH Data Generation Tool)

## Setup

### Manual Setup of TPCH Data Generation Tool

When cloning this repository, you'll need to set up the TPCH data generation tool:

```bash
# Clone the repository
git clone https://github.com/gregrahn/tpch-kit.git

# Build the data generation tool
cd tpch-kit/dbgen
make

# Generate sample data (Scale factor 1 = 1GB)
./dbgen -s 1

# Move the generated .tbl files to your data directory
mkdir -p ../data
mv *.tbl ../data/
cd ../..
```

## Running with Docker (Recommended)

The easiest way to run the implementation is using Docker:

1. Build the Docker image:
   ```bash
   docker build -t tpch-query5 .
   ```

2. Generate TPCH data:
   ```bash
   mkdir -p data
   docker run --entrypoint="/app/generate_data.sh" -v "$(pwd)/data:/app/data" tpch-query5 --scale 1
   ```

3. Run the query:
   ```bash
   docker run -v "$(pwd)/data:/app/data" tpch-query5 \
     --customer-path /app/data/customer.tbl \
     --orders-path /app/data/orders.tbl \
     --lineitem-path /app/data/lineitem.tbl \
     --supplier-path /app/data/supplier.tbl \
     --nation-path /app/data/nation.tbl \
     --region-path /app/data/region.tbl \
     --region-name ASIA \
     --date-from 1994-01-01 \
     --date-to 1995-01-01 \
     --threads 4
   ```

> **Note:** Always use quotes around the volume path if your directory path contains spaces or capital letters: `-v "$(pwd)/data:/app/data"`.

### Using the Helper Script

You can also use the included helper script for running the query:

```bash
docker run --entrypoint="/app/scripts/run_test.sh" -v "$(pwd)/data:/app/data" tpch-query5 \
  --data-dir /app/data \
  --threads 4 \
  --region ASIA \
  --date-from 1994-01-01 \
  --date-to 1995-01-01
```

## Building and Running Locally

### Using CMake

```bash
mkdir build && cd build
cmake ..
make
```

### Using Make

```bash
make
```

### Running Locally

```bash
./tpch_query5 \
  --customer-path data/customer.tbl \
  --orders-path data/orders.tbl \
  --lineitem-path data/lineitem.tbl \
  --supplier-path data/supplier.tbl \
  --nation-path data/nation.tbl \
  --region-path data/region.tbl \
  --region-name ASIA \
  --date-from 1994-01-01 \
  --date-to 1995-01-01 \
  --threads 4 \
  --output results.csv
```

## Command-line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--customer-path` | Path to customer.tbl file | (required) |
| `--orders-path` | Path to orders.tbl file | (required) |
| `--lineitem-path` | Path to lineitem.tbl file | (required) |
| `--supplier-path` | Path to supplier.tbl file | (required) |
| `--nation-path` | Path to nation.tbl file | (required) |
| `--region-path` | Path to region.tbl file | (required) |
| `--region-name` | Region name filter | ASIA |
| `--date-from` | Start date filter (YYYY-MM-DD) | 1994-01-01 |
| `--date-to` | End date filter (YYYY-MM-DD) | 1995-01-01 |
| `--threads` | Number of threads to use | (CPU cores) |
| `--output` | Path to output file | (stdout) |
| `--help` | Display help message | |

## Project Structure

- `src/` - Source code files
  - `main.cpp` - Entry point with CLI implementation
  - `data_loader.cpp` - Functions for loading TPCH data
  - `query_processor.cpp` - Implementation of Query 5 logic
  - `thread_pool.cpp` - Thread pool implementation
- `include/` - Header files
  - `data_types.h` - Data structures for TPCH schema
  - `data_loader.h` - Data loading interface
  - `query_processor.h` - Query processing interface
  - `thread_pool.h` - Thread pool interface
- `scripts/` - Helper scripts
  - `generate_data.sh` - Script to generate TPCH data
  - `run_test.sh` - Script to run a test with the implementation

## Implementation Details

The project implements TPC-H Query 5 without using a database system:

1. **Data Loading**: Reads and parses the TPC-H data files directly
2. **Parallel Processing**: Uses a thread pool to distribute the work across multiple threads
3. **Memory Efficiency**: Optimized data structures to reduce memory usage during processing
4. **Result Generation**: Sorts and formats the results according to the TPC-H specifications

## Expected Results

For scale factor 1 with region 'ASIA' and date range '1994-01-01' to '1995-01-01', you should see results similar to:

```
'INDONESIA',55502041.1697
'VIETNAM',55295086.9967
'CHINA',53724494.2566
'INDIA',52035512.0002
'JAPAN',45410175.6954
```

Your exact values might vary depending on the specific data generated.