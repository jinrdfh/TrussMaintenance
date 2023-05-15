# README #

This folder contains all experimental scripts of paper "Efficient Star-based Truss Maintenance on Dynamic Graphs".

We put each experiment in a separate folder, which contains a demonstration.
## This folder contains the following: ## 
* **Exp-I**: Exp-I folder
* **Exp-II**: Exp-II folder
* **Exp-III**: Exp-III folder
* **Exp-IV**: Exp-IV folder
* **Exp-V**: Exp-V folder
* **Exp-VI**: Exp-VI folder
* **Exp-VII**: Exp-VII folder
* **Exp-VIII**: Exp-VIII folder
* **index.out**: The executable file to build the index of our algorithm
* **ballsIns.out**: The executable file of our incremental algorithm
* **ballDel.out**: The executable file of our decremental algorithm
* **randomPMulNode.py**: The python file to sample nodes and edges in our experiments
* **OrderIndex.out**: The executable file to build the index of algorithm Order
* **Order.out**: The executable file of algorithm Order
* **XHIndex.out**: The executable file to build the index of algorithm XH
* **XH.out**: The executable file of algorithm XH
* **nodePP.out**: The executable file of algorithm NodePP
* **filter.py**: The Python file that can change a directed graph into a simple graph.
* **master.sh**: The main script file that generate experimental results in corresponding folders.

## sample ##
Run 

chmod +x master.sh
./master.sh

in this folder, which takes about 900 hours.
The script will download datasets, build indexes, and run experiments.
Each experiment is independent, and if you want to obtain experimental results faster, you can run each experiment separately in the background.
If you want to get the table in Exp-I, you can jump to the folder "Exp-I", then you can get the table saved in file "Exp-I.csv". More detailed README is in each subdirectory.