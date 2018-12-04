#!/bin/bash

ARQUIVOS=(bcsstk14.rsa bcsstk15.rsa bcsstk16.rsa bcsstk17.rsa bcsstk18.rsa)
MAQ=NOTE
NNODOS=8

mkdir tempos

timestamp=$(date +%s)

dir_tempos="tempos/exec_${MAQ}_${timestamp}"
mkdir ${dir_tempos}
for arq in ${ARQUIVOS[@]}; do
	echo "executando ${arq}"
	dir_arq="${dir_tempos}/${arq}"
	mkdir $dir_arq
	for nproc in `seq 1 ${NNODOS}`; do
		# precisa {} pra funcionar
		{ time make run ARQ=arquivos/bcsstruc2/${arq} VALORB=5 NPROCS=${nproc} RES=0 ; } 2> ${dir_arq}/tempo_${nproc}
	done
done
