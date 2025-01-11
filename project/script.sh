#!/bin/bash

# Define file names
MAIN_FILE="main.c"
PARSE_FILE="parse.c"
HEADER_FILE="parse.h"
OUTPUT_FILE="program"

# Step 1: Compile the C files into object files
echo "Compiling $MAIN_FILE and $PARSE_FILE..."
gcc -c $MAIN_FILE -o main.o
gcc -c $PARSE_FILE -o parse.o

# Step 2: Link the object files into the final executable
echo "Linking object files into $OUTPUT_FILE..."
gcc main.o parse.o -o $OUTPUT_FILE

# Step 3: Run the resulting program
echo "Running the program..."
perf stat ./$OUTPUT_FILE

# Clean up object files (optional, uncomment to remove .o files)
echo "Cleaning up..."
rm *.o
rm *.txt
rm program
