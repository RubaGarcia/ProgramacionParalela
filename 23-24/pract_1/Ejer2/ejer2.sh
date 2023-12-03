for size in 1 2 4 8 16 32 64 128
do
	echo $size
        time ./MatMul 2 $size
done

