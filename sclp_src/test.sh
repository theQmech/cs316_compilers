mkdir -p testcases/err
mkdir -p testcases/spim
mkdir -p testcases/toks

for i in testcases/*; do
	./sclp -tokens -ast -symtab -icode -d $i >temp.out 2>&1
	./sclp16 -tokens -ast -symtab -icode -d $i >temp_ref.out 2>&1
	echo $i
	diff temp.out temp_ref.out > 1.txt
	if [ $$ -ne 0 ]; then
		break
	fi
done
