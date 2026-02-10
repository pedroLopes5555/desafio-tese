## Intrudução 

A maior parte dos STM (tecnica de programação concorrente para facilitar escritas e leituras concorrentes) são implementados a partir de timestamps

Em algoritmos como LSA e TL2, todas as transações com escrita incrementão um relogio global durante o seu commit. (depois para validar vão comparar o seu timestamo de inicio com o timestamp da ultima vez que os dados foram alterados)


## Motivação

Existem 2 problemas com __shared clocks__
  - Heuristica das tecnicas utilizadas
  - O uso de memoria partilhada é um __bootleneck__

Os CPUs mais recente já tem contador de ciclos de hardwware, portanto seria execuivel construir um STM em que as timestamps são retiradas de um contador de hardware

## Problema
Podemos utilizar relogios baseados em contadores de ciclos de hardware para imlementar STMs?

-> Existem processadores que teem esses registos como o OracleUltraSPARCT2 mas não são monotonicamente crescentes

-> rdtsc e rdtscp

O CPU pode reeordenar a execucao de rdtsc:
-> Preciding Storage
-> Preciding Loads

o rdtscp
-> Preciding storage

podemos nos proteger de preciding load se utilizarmo a FLAG LFENCE


## Soluções
### Single check

#### Begin
Guardar o começo da transação, inicializar as variaveis

#### Read
- Se esta nas escritas, devolve o que vai ser escritas
- Ir buscar a ultima vez em que o valor aquele aderesso foi alterado
- Se não foi alterado depois de comecar a transação podemos ler, se não é sinal de concorrencia

#### Write
- adicionar o adresso e o valor à lista de leituras

#### commit
- Se não houver escritas não ha nada para dar commit
- Adquire os locks dos adressos aos quais queremos escrever
- validar os timestamps
- escrever para a memória, uma vez que ja temos os locks
- atualizar os timestamps das leituras e libertar os locks


Podemos utilizar valores de 64 bits para guardar o timestamp, e utilizar o MSB para identificar se o valor está bloqueado ou não

check-once pode aceitar leituras inconsistentes

-> o check once confia apenas em __orec.timestamp <= stat_timestamp__, mas esta condição pode ser verdadeira, memso que a escrita tenha acontecido depois


### Check Twice




## Resultados






