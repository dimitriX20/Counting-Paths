#!/bin/bash

# Execute 'make clean' command
make clean

# Check if 'make' command executed successfully
if [ $? -eq 0 ]; then
  # Execute 'make' command
  make
  
  # Check if 'make' command executed successfully
  if [ $? -eq 0 ]; then
    # Execute './myprogram' command
    ./myprogram
  else
    echo "Make command failed."
  fi
else
  echo "Make clean command failed."
fi
