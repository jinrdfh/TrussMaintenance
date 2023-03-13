# README #

This folder contains all experimental scripts of paper "Efficient Star-based Truss Maintenance on Dynamic Graphs".
Due to the size limit, we only upload two small datasets. To try other datasets, you can download them and put them at this folder, and add the dataset name into each experimental script.

## Download links for all datasets: ## 
* **Deezer_RO**:   http://snap.stanford.edu/data/gemsec_deezer_dataset.tar.gz
* **Amazon**:      http://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz
* **DBLP**:        http://snap.stanford.edu/data/bigdata/communities/com-dblp.ungraph.txt.gz
* **As-skitter**:  http://snap.stanford.edu/data/as-skitter.txt.gz
* **Cit-Patents**: http://snap.stanford.edu/data/cit-Patents.txt.gz    time information https://data.nber.org/patents/apat63_99.zip
* **Soc-pokec**:   http://snap.stanford.edu/data/soc-pokec-relationships.txt.gz
* **LiveJournal**: http://snap.stanford.edu/data/soc-LiveJournal1.txt.gz
* **Orkut**:       http://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz
* **Wise**:        http://www.wise2012.cs.ucy.ac.cy/challenge.html

We put each experiment in a separate folder, which contains a demonstration.
## This folder contains the following: ## 
* **Exp-I**: Exp-I folder
* **Exp-II**: Exp-II folder
* **Exp-III**: Exp-III folder
* **Exp-IV**: Exp-IV folder
* **Exp-V**: Exp-V folder
* **Exp-VI**: Exp-VI folder
* **Exp-VII**: Exp-VII folder
* **deezer_RO.txt**: Dataset
* **amazon.txt**: Dataset
* **index.out**: The executable file to build the index of our algorithm
* **ballsIns.out**: The executable file of our incremental algorithm
* **ballDel.out**: The executable file of our decremental algorithm
* **randomPMulNode.py**: The python file to sample nodes and edges in our experiments
* **OrderIndex.out**: The executable file to build the index of algorithm Order
* **Order.out**: The executable file of algorithm Order
* **XHIndex.out**: The executable file to build the index of algorithm XH
* **XH.out**: The executable file of algorithm XH
* **nodePP.out**: The executable file of algorithm NodePP

## sample ##
If you want to get the table in Exp-I, you can jump to the folder "Exp-I" and run "./Exp-I.sh", then you can get the table saved in file "Exp-I.csv". More detailed README is in each subdirectory.
