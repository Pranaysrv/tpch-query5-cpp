FROM ubuntu:20.04

# Avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    unzip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy source code
COPY . /app/

# Build the application
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Create data directory
RUN mkdir -p /app/data

# Set up TPCH data generation tool
RUN cd /app && \
    rm -rf tpch-kit && \
    git clone https://github.com/gregrahn/tpch-kit.git && \
    cd tpch-kit/dbgen && \
    make

# Add script to generate data
COPY scripts/generate_data.sh /app/
RUN chmod +x /app/generate_data.sh

# Set entrypoint
ENTRYPOINT ["/app/build/tpch_query5"]

# Default command
CMD ["--help"]