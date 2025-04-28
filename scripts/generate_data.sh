#!/bin/bash

# Script to generate TPCH data for testing

# Default values
SCALE_FACTOR=1
OUTPUT_DIR="/app/data"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --scale|-s)
            SCALE_FACTOR="$2"
            shift
            shift
            ;;
        --output|-o)
            OUTPUT_DIR="$2"
            shift
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --scale, -s FACTOR    Scale factor for data generation (default: 1)"
            echo "  --output, -o DIR      Output directory (default: /app/data)"
            echo "  --help, -h            Display this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Change to dbgen directory
cd /app/tpch-kit/dbgen || {
    echo "Error: dbgen directory not found"
    exit 1
}

# Generate data
echo "Generating TPCH data with scale factor $SCALE_FACTOR..."
./dbgen -s "$SCALE_FACTOR" -f

# Move generated files to output directory
echo "Moving data files to $OUTPUT_DIR..."
mv *.tbl "$OUTPUT_DIR/"

echo "Data generation complete. Files are in $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"