#!/bin/bash

# Script to run a test of the TPCH Query 5 implementation

# Default values
DATA_DIR="/app/data"
THREADS=$(nproc)
REGION_NAME="ASIA"
DATE_FROM="1994-01-01"
DATE_TO="1995-01-01"
OUTPUT_FILE=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --data-dir|-d)
            DATA_DIR="$2"
            shift
            shift
            ;;
        --threads|-t)
            THREADS="$2"
            shift
            shift
            ;;
        --region|-r)
            REGION_NAME="$2"
            shift
            shift
            ;;
        --date-from|-f)
            DATE_FROM="$2"
            shift
            shift
            ;;
        --date-to|-to)
            DATE_TO="$2"
            shift
            shift
            ;;
        --output|-o)
            OUTPUT_FILE="$2"
            shift
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --data-dir, -d DIR     Directory containing TPCH data files (default: /app/data)"
            echo "  --threads, -t NUM      Number of threads to use (default: number of CPU cores)"
            echo "  --region, -r NAME      Region name filter (default: ASIA)"
            echo "  --date-from, -f DATE   Start date filter (default: 1994-01-01)"
            echo "  --date-to, -to DATE    End date filter (default: 1995-01-01)"
            echo "  --output, -o FILE      Output file (default: stdout)"
            echo "  --help, -h             Display this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check if data directory exists
if [ ! -d "$DATA_DIR" ]; then
    echo "Error: Data directory $DATA_DIR does not exist"
    exit 1
fi

# Check if data files exist
for file in customer.tbl orders.tbl lineitem.tbl supplier.tbl nation.tbl region.tbl; do
    if [ ! -f "$DATA_DIR/$file" ]; then
        echo "Error: Data file $DATA_DIR/$file does not exist"
        exit 1
    fi
done

# Build command
CMD="/app/build/tpch_query5"
if [ ! -f "$CMD" ]; then
    CMD="./tpch_query5"
    if [ ! -f "$CMD" ]; then
        echo "Error: Executable not found"
        exit 1
    fi
fi

# Build arguments
ARGS="--customer-path $DATA_DIR/customer.tbl"
ARGS="$ARGS --orders-path $DATA_DIR/orders.tbl"
ARGS="$ARGS --lineitem-path $DATA_DIR/lineitem.tbl"
ARGS="$ARGS --supplier-path $DATA_DIR/supplier.tbl"
ARGS="$ARGS --nation-path $DATA_DIR/nation.tbl"
ARGS="$ARGS --region-path $DATA_DIR/region.tbl"
ARGS="$ARGS --region-name $REGION_NAME"
ARGS="$ARGS --date-from $DATE_FROM"
ARGS="$ARGS --date-to $DATE_TO"
ARGS="$ARGS --threads $THREADS"

if [ -n "$OUTPUT_FILE" ]; then
    ARGS="$ARGS --output $OUTPUT_FILE"
fi

# Run the command
echo "Running: $CMD $ARGS"
$CMD $ARGS

# Check exit status
if [ $? -eq 0 ]; then
    echo "Test completed successfully"
else
    echo "Test failed"
    exit 1
fi