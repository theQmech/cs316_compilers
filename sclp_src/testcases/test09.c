void main();
main()
{
	int a;
	float b;

	a = -3;
	b = 3;

	// Arith on int
	a++;
	a--;
	--a;
	++a;
	a += 3;
	a = a++ + --a / -a;
	a = ++a - --a / (-a);
	a = a + -a++;

	// Arith on float
	b++;
	b--;
	--b;
	++b;
	b -= 4;
	b = ++b + ++b + ++b - --b;
	b = b / -(b--);
}
