#include <stdio.h>
#include <unistd.h>
int main(){
	while(1){
		FILE *f = fopen("/sys/class/leds/led0/shot", "w");
		if (f == NULL)
		{
    		printf("Error opening file!\n");
    		//exit(1);
		}
		const char *text = "1";
		fprintf(f, "%s", text);
		fclose(f);
    usleep(1000000);
	}
}
