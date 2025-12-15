# ifnet - Servidor TCP Multi-threaded

Servidor TCP concorrente em C++17 com pool de threads.

## Compilar

```bash
make
```

Isso gera dois executáveis:
- `server` — o servidor TCP
- `load_test` — ferramenta de teste de carga

## Rodar

**Terminal 1** — inicia o servidor:
```bash
./server
```

**Terminal 2** — testa com o cliente simples ou com o load_test:
```bash
make client
./client

./load_test
```

## Opções do load_test

```bash
./load_test --clients 100 --messages 10 --threads 10
```

| Opção | Descrição | Default |
|-------|-----------|---------|
| `--clients N` | Número de clientes simulados | 100 |
| `--messages N` | Mensagens por cliente | 10 |
| `--threads N` | Threads do teste | 10 |
| `--host IP` | IP do servidor | 127.0.0.1 |
| `--port P` | Porta | 1337 |
| `--delay MS` | Delay entre mensagens (ms) | 0 |

## Limpar

```bash
make clean
```
