#include <stdio.h>

long add(long a, long b){
	printf("fun add\n");
	return a + b;
}

int main(int argc, char** argv){
	long ret = 0;
	ret = add(1,2);
	printf("fun main: %ld\n", ret);
	return ret;
}
