#!/bin/bash

ARQUIVOS=(bcsstk01.rsa bcsstk07.rsa bcsstk11.rsa bcsstk16.rsa bcsstk17.rsa)
MAQ=NOTE
NNODOS=4

mkdir tempos

timestamp=$(date +%s)

dir_tempos="tempos/exec_${MAQ}_${timestamp}"
mkdir ${dir_tempos}
for arq in ${ARQUIVOS[@]}; do
	echo "executando ${arq}"
	dir_arq="${dir_tempos}/${arq}"
	mkdir $dir_arq
	for nproc in `seq 1 ${NNODOS}`; do
		make run ARQ=arquivos/bcsstruc/${arq} VALORB=5 NPROCS=${nproc} RES=0 TEMPO=1 ITER=1 > ${dir_arq}/tempo_${nproc}
	done
done
