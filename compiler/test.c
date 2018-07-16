const int a = 10;
const char b = 'c';
const int C = 11, d = 20;
const char e = 'a', f = '+', g = '3';

int count;
char i;
int j, k;
char l, m, n[4];
int O[100], p;

void hanoi(int n) {
    if(n == 1) {
        count = count + 1;
        return;
    }
    else {
        hanoi(n-1);
        count = count + 1;
        hanoi(n-1);
    }
}

int gcd(int a, int b) {
    int temp;
    if(a <= b) {
        temp = a;
        a = b;
        b = temp;
    }
    else {

    }
    if(b == 0) {
        return(a);
    }
    else {
        return(gcd(b, a-a/b*b));
    }
}

int fib(int n) {
    int i;         

    if(n >= 100)
        return(-1);
    else {

    }
    O[0] = 0;
	if(n >= 1) {
		O[1] = 1;
	}
	else {
	}
    i = 2;
    while(i <= n) {
        O[i] = O[i-1] + O[i-2];
        i = i + 1;
    }
    return(O[n]);
}

int sum_O {
    const int len = 100; 
    int sum;             
    int i;
    i = 0;
    sum = 0;
    while(i < len) {
        sum = sum + O[i];
        i = i + 1;
    }
    return(sum);
}

void print_n {
    const char a = 'a';  
    const char k = 'k';
    int n;              
    n = k - a;
    printf(n);
}

void odd_or_even(int a) {
    int flag;            
    int temp;
    temp = a;
    if(temp < 0)
        temp = -temp;
    else {

    }
    flag = temp - temp/2*2;
    switch(flag) {
        case 0: {
            printf(a);
            printf(" is even number");
        }
        case 1: {
            printf(a);
            printf(" is odd number");
        }
        default: {
            printf("error");
        }
    }
}

void main() {
    int num1;
    int num2;
    int num3;
    int fib_num;
    int num4;

	num1 = -1;
    count = 0;
    printf("input hanoi number:");
    scanf(num1);
    hanoi(num1);
    printf("move ");
    printf(count);
    printf(" steps");

    printf("input two numbers:");
    scanf(num2, num3);
    printf("greatest common divisor is: ");
    printf(gcd(num2, num3));

    printf("input fibonacci number:");
    scanf(fib_num);
    printf(fib_num);
    printf("-th fibonacci number is ");
    printf(fib(fib_num));
    printf("sum of the first ");
    printf(fib_num);
    printf(" fibonacci number is ");
    printf(sum_O);

    printf("input a number to judge odd or even:");
    scanf(num4);
    odd_or_even(num4);
	
	num1 = 1 + 3;
	num2 = 2 * 6;
	num3 = num1 * num2 + num2 / num1 + num1 * num2;
	printf(num3);
}