Para compilar:

$ make all

Para rodar, executar:

$ make run NPROCS=4 ARQ=arquivos/bcsstk01.rsa VALORB=5 RES=1 > res_bcsstk01.txt

Parâmetros:

- <$1> = Caminho do arquivo.
- <$2> = Valor do vetor b.
- <$3> = Printar resultado (0 ou 1)

Saída:

Vetor com o resultado do gradiente conjugado.

Testado com:

- g++ 8.1.1 20180712 (Red Hat 8.1.1-5)
- Fedora 28

**Artigo formato SBC Template conference**
