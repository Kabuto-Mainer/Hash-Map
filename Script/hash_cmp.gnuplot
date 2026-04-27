set terminal pngcairo size 1200,700
set output 'hash_cmp.png'

set style data histograms
set style histogram errorbars gap 1
set style fill solid border -1
set boxwidth 0.8
set bars fullwidth

set grid ytics
set xtics rotate by -45
set key off
set ylabel "Dispersion"

plot 'hash_data.dat' using 2:3:xtic(1)
