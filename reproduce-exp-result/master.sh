# compile
chmod +x *sh
chmod +x */*sh

./getExe.sh

chmod +x *out
chmod +x */*out

# download datasets
cd data
# Deezer
wget http://snap.stanford.edu/data/gemsec_deezer_dataset.tar.gz
tar zxvf gemsec_deezer_dataset.tar.gz
cat deezer_clean_data/RO_edges.csv | grep -v node | sed 's/,/ /g' > Deezer.txt
rm -rf deezer_clean_data gemsec_deezer_dataset.tar.gz
# Amazon
wget http://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz
gzip -d com-amazon.ungraph.txt.gz
cat com-amazon.ungraph.txt | grep -v '#' | sed 's/\t/ /g' > Amazon.txt
rm com-amazon.ungraph.txt
# DBLP
wget 'https://cloud.comp.hkbu.edu.hk/public.php?service=files&t=766bdb3611db7902982fa4b3150a275d&download' -O DBLP.tar.gz
tar zxvf DBLP.tar.gz
mv DBLP_simple.txt DBLP.txt
rm DBLP.tar.gz
# Skitter
wget http://snap.stanford.edu/data/as-skitter.txt.gz
gzip -d as-skitter.txt.gz
cat as-skitter.txt | grep -v '#' | sed 's/\t/ /g' > Skitter.txt
rm as-skitter.txt
# Patents
wget http://snap.stanford.edu/data/cit-Patents.txt.gz
gzip -d cit-Patents.txt.gz
cat cit-Patents.txt | grep -v '#' | sed 's/\t/ /g' > temp.txt
python ../filter.py temp.txt Patents.txt
rm cit-Patents.txt temp.txt
wget https://data.nber.org/patents/apat63_99.zip
unzip apat63_99.zip
rm apat63_99.zip
# Pokec
wget http://snap.stanford.edu/data/soc-pokec-relationships.txt.gz
gzip -d soc-pokec-relationships.txt.gz
cat soc-pokec-relationships.txt | grep -v '#' | sed 's/\t/ /g' > temp.txt
python ../filter.py temp.txt Pokec.txt
rm soc-pokec-relationships.txt temp.txt
# LJ
wget http://snap.stanford.edu/data/soc-LiveJournal1.txt.gz
gzip -d soc-LiveJournal1.txt.gz
cat soc-LiveJournal1.txt | grep -v '#' | sed 's/\t/ /g' > temp.txt
python ../filter.py temp.txt LJ.txt
rm soc-LiveJournal1.txt temp.txt
# Orkut
wget http://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz
gzip -d com-orkut.ungraph.txt.gz
cat com-orkut.ungraph.txt | grep -v '#' | sed 's/\t/ /g' > temp.txt
python ../filter.py temp.txt Orkut.txt
rm com-orkut.ungraph.txt temp.txt
# Wise
wget 'https://cloud.comp.hkbu.edu.hk/public.php?service=files&t=72e3936f559aedcdb6986c04c328d64e&download' -O Wise.tar.gz
tar zxvf Wise.tar.gz
mv wise_simple.txt Wise.txt
rm Wise.tar.gz
cd ..

# build index 15 h
cd data
./buildAllIndex.sh
cd ..

# Exp-I 600 h
cd Exp-I
./Exp-I.sh
cd ..

# Exp-II  8 h
cd Exp-II
./Exp-II.sh
cd ..

# Exp-III  8 h
cd Exp-III
./Exp-III.sh
cd ..

# Exp-IV 700 h
cd Exp-IV
./Exp-IV.sh
cd ..

# Exp-V 480 h
cd Exp-V
./Exp-V.sh
cd ..

# Exp-VI 10 min
cd Exp-VI
./Exp-VI.sh
cd ..

# Exp-VII 72 h
cd Exp-VII
./Exp-VII.sh
cd ..

# Exp-VIII 30 min
cd Exp-VIII
./Exp-VIII.sh
cd ..

