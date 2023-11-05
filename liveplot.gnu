set xrange [0:10000]
set yrange [0:1000]
set cbrange [1000:0]
set pm3d map
set palette defined (0 "blue", 20 "yellow", 400 "orange", 1000 "red")
splot "plot.dat" using 1:2:3 with lines palette notitle
pause 0.1
reread