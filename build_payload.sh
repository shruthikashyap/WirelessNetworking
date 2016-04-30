##OUTPUT_STATS=out_stats.dat
##PLOT_THROUGHPUT=out_throughput.dat
THROUGHPUT_IMG=throughput_img_payload.png

rm out_stats_pl_*
rm plot_throughput_pl_*

for payload in 1472 1024 512
do
	for node in $(seq 5) 
	do
		echo -ne "Simulating for $node nodes with payload $payload ... "
		./waf --run "scratch/throughput --nWifi=$node --rate=11 --payloadSize=$payload" >> out_stats_pl_$payload.dat
		echo "DONE"
	done

	sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_stats_pl_$payload.dat
	cut -f2,6 out_stats_pl_$payload.dat > plot_throughput_pl_$payload.dat
done

gnuplot <<- EOF
set title "Network Performance vs Payload Size"
set xlabel "Number of Nodes"
set ylabel "Average Throughput (Mbps)"
set term png
set output "$THROUGHPUT_IMG"
set style fill solid
plot [-0.5:][0:5000] 'plot_throughput_pl_1472.dat' u 2: xtic(1) with histogram title "1472 bytes", 'plot_throughput_pl_1024.dat' u 2: xtic(1) with histogram title "1024 bytes", 'plot_throughput_pl_512.dat' u 2: xtic(1) with histogram title "512 bytes"

EOF
xdg-open $THROUGHPUT_IMG
