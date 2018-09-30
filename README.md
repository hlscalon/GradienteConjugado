Para compilar basta entrar no diretório e rodar:

$ make

Para rodar, executar:

$ ./gradiente_conjugado arquivos/bcsstk01.rsa 1 5 > res_bcsstk01.txt

Parâmetros:

- <$1> = Caminho do arquivo.
- <$2> = Formato do arquivo (1 = Harwell-Boeing, 2 = MTX).
- <$3> = Valor do vetor b.

Saída:

Vetor com o resultado do gradiente conjugado.

Testado com:

- g++ 8.1.1 20180712 (Red Hat 8.1.1-5)
- Fedora 28
