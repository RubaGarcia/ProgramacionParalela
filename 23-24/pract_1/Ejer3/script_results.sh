#!/bin/bash

# Compilar el código
make clean
make

# Dimension y tamaño de bloque fijos
dim=5000
block_size=128

# Archivo de resultados
result_file="results.txt"

# Encabezado de la tabla en el archivo de resultados
echo -e "Numero de hilos\tTiempo in\tTiempo out\tMax in\tMax out" > $result_file

# Número de hilos de 2 a 8
for num_threads in {2..8}; do
    # Ejecutar el código y capturar la salida y error en variables
    output=$(./MatMul_sec $dim $block_size $num_threads 2>&1)

    # Extraer la información de interés de la salida
    time_in=$(echo "$output" | grep "Tiempo in" | awk '{print $3}')
    max_in=$(echo "$output" | grep "El máximo es" | awk '{print $3}')
    time_out=$(echo "$output" | grep "Tiempo out" | awk '{print $3}')
    max_out=$(echo "$output" | grep "El máximo es" | awk '{print $3}')

    # Imprimir y guardar en el archivo results.txt
    echo -e "$num_threads\t$time_in\t$time_out\t$max_in\t$max_out" >> $result_file
done
