# README #

This folder contains the experimental script of Exp-I

## This folder contains the following: ## 
* **Exp-I.sh**: main script, can generate the table in Exp-I
* **avgNodePP.sh**: will run algorithm NodePP 100 times and return the average running time
* **avgOrderInc.sh**: will run incremental algorithm Order 100 times and return the average running time
* **avgOrderDec.sh**: will run decremental algorithm Order 100 times and return the average running time
* **avgOursInc.sh**: will run our incremental algorithm 100 times and return the average running time
* **avgOursDec.sh**: will run our decremental algorithm 100 times and return the average running time
* **avgXHInc.sh**: will run incremental algorithm XH 100 times and return the average running time
* **avgXHDec.sh**: will run decremental algorithm XH 100 times and return the average running time
* **G2Order.sh**: generate index for algorithm Order
* **G2XH.sh**: generate index for algorithm XH
* **G2Ours.sh**: generate index for our algorithm
* **Q2XH.sh**: convert query file to the format for algorithm XH
* **Q2Order.sh**: convert query file to the format for algorithm Order
* **randomPs.sh**: randomly sample 100 star motifs and generate 100 query files
* **query**: empty folder, 100 query files will be put into it

## running ## 
* ./Exp-I.sh

## output ## 
* Exp-I.csv

Datasets and the number of repetitions can be modified in the file "Exp-I.sh"
