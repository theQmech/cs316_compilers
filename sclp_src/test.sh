for i in testcases/*; do
	./sclp -ast -symtab -icode -d $i >temp.out 2>&1
	./sclp16 -ast -symtab -icode -d $i >temp_ref.out 2>&1
	diff temp.out temp_ref.out
	if [ $? -ne 0 ]; then
		echo "[FAIL]\t"$i
		break
	fi
	echo "[PASS]\t"$i
done

# rm temp.out temp_ref.out
