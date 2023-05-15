
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
batNum=100

output="Exp-II.csv"

header=",Phase I (ms),Phase II (ms)"
echo $header > $output

for dataset in ${datasets[@]}
do
	ourIncT=`./avgOursInc.sh ../data/$dataset"_sample.myG" ../data/$dataset"_query/" $batNum`

	# save
	resultLine="$dataset,$ourIncT"
	echo $resultLine >> $output
done
