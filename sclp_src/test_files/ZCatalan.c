float catalan(float n);
float main();

catalan(float n)
{
    // Base case
    float res;
    float i;
    float a, b;
    i = 0.0;
    res = 0.0;
    if (n <= 1.1) return 1.0;
 
    // catalan(n) is sum of catalan(i)*catalan(n-i-1)
    // print(i);
    // print("\n");
    while (i < n-0.1){
        // print("call ");
        // print(i);
        a = catalan(i);
        // print("retn ");
        // print(a);
        // print("\n");

        // print("call ");
        // print(n-i-1.0);
        b = catalan(n-i-1.0);
        // print("retn ");
        // print(b);
        // print("\n");

        // print("res");
        // print(a*b+res);
        res = res + a*b;
        // print("res");
        // print(res);
        // print("\n");

        i = i+1.0;
    }
 
    return res;
}
 
// Driver program to test above function
main()
{
    // for (float i=0; i<10; i++)
    //     catalan(i) << " ";
    float p;
    float x;
    p = 1.0;
    while (p<10.0){
        x = catalan(p);
        print (x);
        print("\n");
        p = p + 1.0;
    }
    return 0.0;
}