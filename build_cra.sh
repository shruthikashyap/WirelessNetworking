##OUTPUT_STATS=out_stats.dat
##PLOT_THROUGHPUT=out_throughput.dat
THROUGHPUT_IMG=throughput_img_cra.png

rm out_stats_cr_*
rm plot_throughput_cr_*

for cra in 1 2 3 4
do
	for node in $(seq 5) 
	do
		echo -ne "Simulating for $node nodes with control rate algorithm $cra ... "
		./waf --run "scratch/throughput --nWifi=$node --payloadSize=1472 --cRateAlgo=$cra" >> out_stats_cr_$cra.dat
		echo "DONE"
	done

	sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_stats_cr_$cra.dat
	cut -f2,6 out_stats_cr_$cra.dat > plot_throughput_cr_$cra.dat
done

gnuplot <<- EOF
set title "Network Performance vs Control Rate Algorithm"
set xlabel "Number of Nodes"
set ylabel "Average Throughput (Mbps)"
set term png
set output "$THROUGHPUT_IMG"
set style fill solid
plot [-0.5:][0:5000] 'plot_throughput_cr_1.dat' u 2: xtic(1) with histogram title "Constant rate", 'plot_throughput_cr_2.dat' u 2: xtic(1) with histogram title "Cara", 'plot_throughput_cr_3.dat' u 2: xtic(1) with histogram title "Aarf", 'plot_throughput_cr_4.dat' u 2: xtic(1) with histogram title "Arf"

EOF
xdg-open $THROUGHPUT_IMG
