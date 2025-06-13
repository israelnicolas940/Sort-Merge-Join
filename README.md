
## Contagem de I/O
O sistema mantém um contador interno que registra:
- Número de leituras de página
- Número de escritas de página
- Total de operações de I/O

Estas estatísticas são exibidas ao final da execução do programa.

## Testes
O projeto inclui uma suite de testes unitários implementada com GTest, cobrindo:
- Operações de parse de CSV
- Gerenciamento de buffer
- Operações de I/O
- Funcionalidade de Join

## Limitações e Considerações
- O sistema trabalha apenas com joins de igualdade
- Máximo de 40 registros em memória simultaneamente (4 páginas × 10 registros)
- Arquivos de entrada são tratados como somente leitura
- Resultados intermediários são salvos em arquivos temporários
1. Clone o repositório
2. Crie o diretorio build:
   ```bash
   mkdir build
   ```
3. Configure e compile o projeto:
    ```bash
   cmake -S . -B build 
   cmake --build build
   ```
4. Execute o programa
   ```bash
   ./build/Sort-Merge-Join 
   ```
5. Veja os resultados no diretorio data/
   