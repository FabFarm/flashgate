//
//  main.cpp
//  gateOpener
//
//  Created by Philip Carstensen on 30.07.19.
//  Copyright © 2019 Philip Carstensen. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct stream {
    
    float val[10];
    int tail = 0;
};

struct stream vdataq;
struct stream v_medq;
struct stream timeq;
struct stream dvdtq;

struct stream streaming(struct stream dataq, float value)  {
    // adds data from stream into a ring buffer (kind of)
    // only 10 elements are stored with circular index
    dataq.val[dataq.tail] = value;
    dataq.tail = ((dataq.tail)+1)%(10);
    return dataq;
}

float med(float array[], int n)   {
    float mean = 0;
    for (int i = 0; i<n; i++) {
        mean = mean + *(array+i);
    }
    return (float)mean/(float)n;
}

void readLast(struct stream dataq, float arr[], int ar_length) {
    //reads the last n elements of data stream and saves in an array
    for (int i = 0; i<ar_length; i++) {
        if ((dataq.tail-(i+1)) >= 0)  {
            arr[i] = dataq.val[dataq.tail-(i+1)]; //arr[0] newest to arr[n] oldest
        }
        else {
            //important here fixed size of stream is 10!!
            arr[i] = dataq.val[10+(dataq.tail-(i+1))]; // do the wrap araound because of ring buffer
        }
    }
    return; //return is a pointer, maybe there is a better way?
}

// basic numeric derivative of signal
float derivative(struct stream f, struct stream x)    {
    //initialize
    float df[2];
    float dx[2];
    float deltaf;
    float deltat;
    //read last 2 elements of streams, each
    readLast(f,df,2);
    readLast(x,dx,2);
    //built differences
    deltaf = df[0]-df[1];
    deltat = ((double)(dx[0]-dx[1])); // /CLOCKS_PER_SEC; for use of real time
    //build derivative
    if (deltat == 0) {
        return 0;
    }
    else return (deltaf/deltat);
}

//time delay function
void delay(int milliseconds)    {
    long pause;
    clock_t now,then;
    
    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

int main(int argc, const char * argv[]) {
    //initialize some variables
    double t = 0; //fake time
    double fs = 0.01; //steptime
    double f = 1; //2*3.1415/5; //frequency
    
    float v_mean;
    float dvdt;
    double v;
    
    //for noisy signal generation
    time_t trand;
    /* Intializes random number generator */
    srand((unsigned) time(&trand));
    
    //the while resembles the arduino loop!!
    while (t <= 5) {
        
        //Read current voltage value
        v = sin(f*t)+0.001*(rand() % 100);
        t = t+fs;
        printf("Signal: %lf volts \n",v);
        printf("Dataq index: %i \n",vdataq.tail+1);
        
        //save to data stream
        vdataq = streaming(vdataq,v);
        
        //selfmade med filter (mean value) - smoothe signal!
        float lastv[5];
        readLast(vdataq,lastv,5);
        v_mean = med(lastv, 5);
        
        //write to buffer
        v_medq = streaming(v_medq,v_mean);
        
        //save time for derivative
        
        // clock_t time = clock(); for later real time use?
        
        timeq = streaming(timeq,t); //use t for now... change derivative function!!
        //take derivative
        dvdt = derivative(v_medq,timeq);
        // write to buffer
        dvdtq = streaming(dvdtq, dvdt);
        
        //print for checking
        printf("Data queue: ");
        for(int i = 0; i < 10; i++)
        {
            printf("%.2f",vdataq.val[i]);
            printf("\t");
        }
        printf("\n");
        printf("Last n Elements:");
        for(int i = 0; i < 5; i++)
        {
            printf("%.2f",lastv[i]);
            printf("\t");
        }
        printf("newest to oldest\n");
        printf("Derivative of smoothed signal: %.2f",dvdt);
        printf("\n\n");
        //timedelay!
        delay((int)(1/fs));
        }
    return 0;
    }

