#! /bin/bash
NUMOFPROC=${1:-6}
NODELIST=${2:-3,4}
NOW=`date +"%Y%m%d-%H:%M:%S"`
NUMOFNODES=$(grep -o "," <<< "$NODELIST" | wc -l)
NUMOFNODES=$((${NUMOFNODES} + 1))
CURRPATH=`pwd`

echo "Number of processes: ${NUMOFPROC}"
echo "Node List: ${NODELIST}"
echo "FileName: baseline-0-n${NUMOFNODES}-${NUMOFPROC}p-${NOW}.csv"


for i in `seq 1 ${NUMOFPROC}`;
do
	srun --mpi=pmi2 -n${NUMOFPROC} -w "node[${NODELIST}]" ${CURRPATH}/a.out star 1 1 ${i} > baseline-${i}-n${NUMOFNODES}-${NUMOFPROC}p-${NOW}.csv
done

exit 0
