# Define a variable to track whether to continue the loop or quit
continue_plotting = 0

# Bind the 'q' key to the quit_plotting function
bind 'k' 'continue_plotting = 1'

# Main plotting loop
while (!continue_plotting) {
    set xrange [0:50000]
    set yrange [0:200]
    set cbrange [100:0]
    set pm3d map
    set palette defined (0.002 "blue", 0.5 "dark-blue", 1 "yellow", 3 "orange", 100 "red")
    splot "plot.dat" using 1:2:3 with lines palette title 'Evolution du cout dans le recuit simule'
    pause 0.1
    reread
}

# Cleanup and exit
unset pm3d
unset palette
