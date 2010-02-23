# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
set xlabel "Time (s)"
set ylabel "Throughput (Mb/s)"

plot [0:160][] "simple_omni_antenna.thr" using 1:( ($2+$3)/2 ) t 'Throughput' w lp
pause -1

