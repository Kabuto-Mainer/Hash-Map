set terminal pngcairo size 1200,700
set output 'lines.png'

set title "Сравнение заполнения "
set xlabel "X"
set ylabel "Y"
set grid
set key top right

plot 'data1.dat' using 1:2 with linespoints title 'Кривая 1', \
     'data2.dat' using 1:2 with linespoints title 'Кривая 2'
