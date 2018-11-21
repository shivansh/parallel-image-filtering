#!/bin/bash
#
# This script compares the output of the convoluteizing program against the
# expected output.

i=1
while [[ $i -lt 4 ]]; do
    rm -f "my_output$i"
    cat "testcases/sample_input$i" | mpirun -n $1 -stdin all ./convolute > "my_output$i"
    git --no-pager diff --no-index --color-words "testcases/sample_output$i" "my_output$i"
    rm -f "my_output$i"
    ((i++))
done
