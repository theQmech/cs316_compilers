// Right hand side can only be an arithmetic expression
void main();
float fact(float n);
float num(float n);
main()
{
	float a ;
	float b ;

	a=fact(3.00);

	print(a);

	return;
}

fact(float n)
{
	float a,b;
	print("fact ");
	print(n);
	print("\n");

	if(n<1.50)
	{
		return 1.00;
	}
	else
	{
		a=num(n);
		b=fact(n-1.00);

		return a*b;
	}
	return 1.00;
}

num(float n)
{
	float a,b;
	print("num ");
	print(n);
	print("\n");
	
	if(n<1.50)
	{
		return 1.00;
	}
	else
	{
		a=1.00;
		b=num(n-1.00);

		return a+b;
	}

	return 1.00;
}