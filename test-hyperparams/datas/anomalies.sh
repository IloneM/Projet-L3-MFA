for k in $(seq 1 30); do
		for d in $(seq 1 50); do
				if [ ! `ls -l "k"$k"_d"$d".json" | grep -v ^l | wc -l` ]; then
						echo "k"$k"_d"$d".json missing\n"
				fi
		done
done
