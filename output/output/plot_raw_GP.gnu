
# Set the data file name
datafile = "sorted_raw_GP0000.dat"

# Set plot style
set style data linespoints
set pointsize 1.5 # Adjust point size if necessary
set style line 1 lc rgb 'blue' pt 1 lt 1 # Blue crosses with lines
set style line 2 lc rgb 'red' pt 1 lt 1  # Red points with lines (use pt 7 for red squares)

# Labels
set xlabel "Config"
set ylabel "GP"
set title 'GP 0 0 0 0, N_t = 10'
set grid

# Plot command
plot datafile using 1:2 with linespoints linestyle 1 title 'GP_L',\
     datafile using 1:3 with linespoints linestyle 2 title 'GP_T',


# Pause and wait for the user to close the window
pause -1 "Press Enter to close the plot..."

