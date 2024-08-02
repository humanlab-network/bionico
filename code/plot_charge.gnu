#!/usr/bin/gnuplot -persist
set xrange [0:]
set y2range [0:3]

set multiplot
set size 1, 0.33
set boxwidth 1
#set style fill solid 0.1

color(x) = \
    x < 1 ? 255 * 65536 + 240 * 256 + 240 : (\
    x < 2 ? 255 * 65536 + 247 * 256 + 240 : (\
    x < 3 ? 255 * 65536 + 255 * 256 + 240 : \
            240 * 65536 + 255 * 256 + 240))

set origin 0, 0.66
plot "out.csv" u 0:(3):(color($4)) w boxes fc rgb var notitle axis x1y2, "" u 1 w l lw 2 lc rgb "red" title "Voltage (V)"

set origin 0, 0.33
plot "out.csv" u 0:(3):(color($4)) w boxes fc rgb var notitle axis x1y2, "" u 2 w l lw 2 lc rgb "green" title "Current (A)"

set origin 0, 0
plot "out.csv" u 0:(3):(color($4)) w boxes fc rgb var notitle axis x1y2, "" u 3 w l lw 2 lc rgb "blue" title "Battery remaining capacity (mAh)"

unset multiplot