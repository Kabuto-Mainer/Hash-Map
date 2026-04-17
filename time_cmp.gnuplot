set terminal pngcairo size 1800,900
set output 'hist_time.png'


set title "Итоговое ускорение"
set ylabel "Ускорение"
set xlabel "Реализация"

set grid ytics
set key off

set style data histograms
set style fill solid border -1
set boxwidth 0.85

set xtics rotate by -70 right offset 0,-3
set xtics font ",8"

set lmargin 5
set bmargin 10

plot 'time.dat' using 4:xtic(1)
