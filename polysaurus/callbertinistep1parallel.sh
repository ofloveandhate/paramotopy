#!/bin/bash

cd $1/step1 
mpirun -machinefile $2 -np $3 $HOME/./bertiniparallel