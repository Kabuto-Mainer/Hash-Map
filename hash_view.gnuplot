set terminal pngcairo size 1400,800
set output 'hash_view.png'

set title "Столбчатая диаграмма"
set xlabel "Индекс"
set ylabel "Значение"

set grid ytics
set boxwidth 0.8
set style fill solid 0.8 border -1

plot 'dump2.dat' using 1:2 with boxes notitle
