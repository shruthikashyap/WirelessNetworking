#OUTPUT_STATS=out_stats.dat
#PLOT_THROUGHPUT=out_throughput.dat
THROUGHPUT_IMG=throughput_img_rate.png

rm out_stats_rate_*
rm plot_throughput_rate_*

for rate in 1 2 5 11
do
	for node in $(seq 5) 
	do
		echo -ne "Simulating for $node nodes with rate $rate ... "
		./waf --run "scratch/throughput --nWifi=$node --rate=$rate" >> out_stats_rate_$rate.dat
		echo "DONE"
	done

	sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_stats_rate_$rate.dat
	cut -f2,6 out_stats_rate_$rate.dat > plot_throughput_rate_$rate.dat
done

gnuplot <<- EOF
set title "Network Performance vs Data Rate"
set xrange [0:]
set xlabel "Number of Nodes"
set ylabel "Average Throughput (Mbps)"
set term png
set output "$THROUGHPUT_IMG"
set style data linespoints
plot 'plot_throughput_rate_11.dat' w l lc rgb 'yellow' title "11 Mbps", \
'plot_throughput_rate_5.dat' w l lc rgb 'green' title "5.5 Mbps",'plot_throughput_rate_2.dat' w l lc rgb 'blue' title "2 Mbps",'plot_throughput_rate_1.dat' w l lc rgb 'red' title "1 Mbps"
EOF
xdg-open $THROUGHPUT_IMG
