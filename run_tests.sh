max=8
maxw=$(( $max * 320 ))
maxh=$(( $max * 240 ))
echo "" > results/performance.txt
for i in `seq 1 $max`;
do
	w=$(( $i * 320 ))
	h=$(( $i * 240))
	echo "Doing resolution ${w}x$h of ${maxw}x$maxh"
	./research-project -w $w -h $h >> results/performance.txt
done
