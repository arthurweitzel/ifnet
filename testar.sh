#!/bin/bash

echo "Iniciando 5 clientes em background..."

for i in {1..5}
do
   ./cliente &
done

echo "Todos os clientes foram iniciados."

wait
echo "Todos os clientes terminaram."
