##OUTPUT_STATS=out_stats.dat
##PLOT_THROUGHPUT=out_throughput.dat
THROUGHPUT_IMG=throughput_img_mob.png
THROUGHPUT_IMG_DEL=throughput_img_del.png

rm out_stats_mob_*
rm plot_throughput_mob_*

for mob in 1 2 3
do
	for node in 5 
	do
		echo -ne "Simulating for $node nodes with control rate algorithm $mob ... "
		./waf --run "scratch/throughput --rate=11 --nWifi=$node --payloadSize=1472 --mobMod=$mob" >> out_stats_mob_$mob.dat
		echo "DONE"
	done

	sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_stats_mob_$mob.dat
	cut -f2,6 out_stats_mob_$mob.dat > plot_throughput_mob_$mob.dat
	cut -f2,7 out_stats_mob_$mob.dat > plot_throughput_mob_del_$mob.dat
done

gnuplot <<- EOF
set title "Network Performance vs Node Mobility"
set xlabel "Number of Nodes"
set ylabel "Average Throughput (Mbps)"
set term png
set output "$THROUGHPUT_IMG"
set style fill solid
plot [-0.5:][0:1000] 'plot_throughput_mob_1.dat' u 2: xtic(1) with histogram title "RandomDirection2d", 'plot_throughput_mob_2.dat' u 2: xtic(1) with histogram title "RandomWalk2d", 'plot_throughput_mob_3.dat' u 2: xtic(1) with histogram title "ConstantPosition"

EOF
xdg-open $THROUGHPUT_IMG

gnuplot <<- EOF
set title "Network Performance vs Delay"
set xlabel "Number of Nodes"
set ylabel "Time (ms)"
set term png
set output "$THROUGHPUT_IMG_DEL"
set style fill solid
plot [-0.5:][0:480] 'plot_throughput_mob_del_1.dat' u 2: xtic(1) with histogram title "RandomDirection2d", 'plot_throughput_mob_del_2.dat' u 2: xtic(1) with histogram title "RandomWalk2d", 'plot_throughput_mob_del_3.dat' u 2: xtic(1) with histogram title "ConstantPosition"

EOF
xdg-open $THROUGHPUT_IMG_DEL
