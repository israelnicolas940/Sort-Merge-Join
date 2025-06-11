1. Quantidade de IOs (vezes que leu uma pagina).
2. Quantidade de paginas gravadas em disco. 
3. Quantidade de tuplas geradas na juncao.

Esses sao os dados por operacao

Pagina: 10 tuplas no max. Armazenadas em arquivos de texto (disco).
4 paginas, no total, em memoria.

Classes: 

Tabela

- pags: List<Pagina>
- qtd_pags: int
- qtd_cols: int

Pagina

- tuplas: Tupla[10]
- qtd_tuplas_ocup: int

Tupla

- cols: String[qtd_cols]

As juncos sao todas por igualdade. Para qualquer duas tabelas.


How to build: 
In the root directory, run the following commands:

```bash
cmake -S . -B build
cmake --build build
```
