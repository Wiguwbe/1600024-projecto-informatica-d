#!/bin/bash
for NUM_WORKERS in 2 4 5 6; do
    make measurements NUM_WORKERS=$NUM_WORKERS
    make reports NUM_WORKERS=$NUM_WORKERS
done
