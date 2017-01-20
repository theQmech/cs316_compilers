for i in `seq 1 8`; do
	echo "Test:"$i
	echo "[sclp16]"
	echo >t1
	./sclp16 -d Examples/test$i.c >t1 2>&1;
	echo "[sclp]"
	echo >t2
	./sclp -d Examples/test$i.c >t2 2>&1;
	cmp t1 t2 >/dev/null 2>&1
	if [ $? -ne 0 ]; then
	   echo "Test Failed"
	else
       echo "Test Passed"
	fi
	rm -rf t1 t2
	echo
		
done

