rm test_files/*.s

for i in test_files/*.c; do
	./sclp -d $i >$i.my.s 2>&1
	./sclp_sumith -d $i >$i.sumith.s 2>&1
	spim -file $i.my.s >myout
	spim -file $i.sumith.s >refout
	diff myout refout
	if [ $? -ne 0 ]; then
		echo "[FAIL]\t"$i
		break
	fi
	echo "[PASS]\t"$i
done

# rm temp.s temp_ref.s
