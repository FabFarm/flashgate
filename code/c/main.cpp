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
#include "/Applications/MATLAB_R2018b.app/extern/include/mat.h"

//global variables
double pheight = 0.9; //min peak height 0.9 for test signal

//struct for streaming data
struct stream {
    
    float val[10];
    int tail = 0;
};

//used streams
struct stream vdataq; //voltage signal
struct stream v_medq; //smoothed voltage signal
struct stream timeq; //evaluation time
struct stream cTimesq; //code signal times
struct stream dvdtq; //derivative of smoothed voltage signal
struct stream dvdt_meanq; //smoothed derivative

//struct for find peak function return
struct fpeak_return {
    bool flag = false;
    int counter = 0;
    struct stream poldq; //old peaks stream
    struct stream ptimeq; //peak times
    float value = 0; //only for checking
};
//returns used
struct fpeak_return high; //for high peaks
struct fpeak_return low; //for low peaks


//function to find maximum
float max(float array[], int n) {
    float max = 0;
    for (int i = 0; i < n; i++) {
        if (*(array+i) > max) {
            max = *(array+i);
        }
    }
    return max;
}

//function to find minimum
float min(float array[], int n) {
    float min = array[0];
    for (int i = 1; i < n; i++) {
        if (*(array+i) < min) {
            min = *(array+i);
        }
    }
    return min;
}

//function for adding value into datastream
struct stream streaming(struct stream dataq, float value)  {
    // adds data from stream into a ring buffer (kind of)
    // only 10 elements are stored with circular index
    dataq.val[dataq.tail] = value;
    dataq.tail = ((dataq.tail)+1)%(10);
    return dataq;
}

//smooth signal with med filter
float med(float array[], int n)   { //sign seems to be lost, needs editing!!!
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
    return;
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

//function to find peak in signal
struct fpeak_return fpeak(struct fpeak_return input, float signal)    {
    input.flag = false;
    time_t peaktime;
    //find peak
    if (signal > max(input.poldq.val, 10) && signal > pheight) {
        input.poldq = streaming(input.poldq, signal);
        input.counter = 1;
    }
    else    {
        //no new peak?
        input.counter++;
    }
    //when no new peak in 7 steps set peak
    if (input.counter == 7) {
        input.value = max(input.poldq.val, 10);
        input.flag = true;
        peaktime = clock();
        input.ptimeq = streaming(input.ptimeq, (float) peaktime); //only time needed for code recognition
        //reset for new search
        for (int i = 0; i < 10; i++) {
            input.poldq.val[i] = 0;
        }
        input.poldq.tail = 0;
    }
    return input;
}

//code recognition function
bool codeRecognition(struct stream time, bool code[], int n)   {
    // code means short signal -> true, long signal -> false
    // n defines length of code
    bool codeFlag = false;
    float last[n];
    readLast(time, last, n);
    
    //set short/long definition
    double shortmin = min(last,n)*0.75;
    double shortmax = min(last,n)*1.25;

    double longmin = max(last,n)*0.75;
    double longmax = max(last,n)*1.25;
    
    //compare last n signals to code
    for (int i = 0; i<n; i++) {
        if (last[n-(i+1)] >= shortmin && last[n-(i+1)] <= shortmax && last[n-(i+1)] >0 && code[i]) {
            //n-(i+1) because of different order last/code
            //code demands short and signal is short
            codeFlag = true;
        }
        else if(last[n-(i+1)] >= longmin && last[n-(i+1)] <= longmax && !code[i])   {
            //code demands long and signal is long
            codeFlag = true;
        }
        else {
            //any other case
            codeFlag = false;
        }
    }
    return codeFlag;
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
    double v;
    
    float v_mean;
    float dvdt_mean;
    float dvdt;

    float lastdvdt[3];
    float lastv[5];
    
    bool cFlag = false;
    bool code[3] = {true, true, false}; //change also code size n in codeRec function call
    bool openGate = false;
    
    time_T start = 0;
    time_T end = 0;
    time_T now = 0;
    
    //for noisy signal generation
    time_t trand;
    /* Intializes random number generator */
    srand((unsigned) time(&trand));
    
    // try to test with recorded signal
    /* read recorded signal from matlab file
    MATFile *pmat;
    const char* name=NULL;
    mxArray *pa;
    
    // open mat file and read it's content
    pmat = matOpen("day_car1.mat", "r");
    if (pmat == NULL)
    {
        printf("Error Opening File: \"%s\"\n", argv[1]);
    }
    
    // Read in each array.
    pa = matGetNextVariable(pmat, &name);
    while (pa!=NULL)
    {
        //Diagnose array pa
     
        printf("\nArray %s has %zu dimensions.", name, mxGetNumberOfDimensions(pa));
        
        //print matrix elements
        //printf("\ndata %d",pa);
        
        //get next variable
        pa = matGetNextVariable(pmat,&name);
        
        //printf("\ndata %d",pa);
        //destroy allocated matrix
        mxDestroyArray(pa);
    }
    
    matClose(pmat);
    */

    
    
    
    //the while resembles the arduino loop!!
    while (t <= 5) {
        
        //Read current voltage value
        v = sin(f*t)+0.0001*(rand() % 100);
        t = t+fs;
        printf("Signal: %lf volts \n",v);
        printf("Dataq index: %i \n",vdataq.tail+1);
        
        //save to data stream
        vdataq = streaming(vdataq,v);
        
        /*Signal processing*/
        
        //selfmade med filter (mean value) - smooth signal!
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
        //selfmade med filter (mean value) - smoothe derivative!
        readLast(dvdtq,lastdvdt,3);
        dvdt_mean = med(lastdvdt, 3);
        
        //find peaks
        high = fpeak(high, dvdt_mean);
        low = fpeak(low, -dvdt_mean);
        
        /*code recognition*/
        now = clock();
        if (high.flag) {
            start = clock();
            //led digital write high
        }
        else if (low.flag) {
            end = clock();
            cTimesq = streaming(cTimesq, (float) ((end-start) / CLOCKS_PER_SEC));
            //led digital write low
        }
        else if ((float) ((now-start) / CLOCKS_PER_SEC))  {
            start = 0;
            end = 0;
        }
        cFlag = codeRecognition(cTimesq, code, 3);
        
        if (cFlag) {
            while (openGate) { //also time counter to avoid arduino getting stuck !openGate && runtime < maxRuntime
                //led green
                delay(3000);
                openGate = true;
                // runtime = clock();
            }
            openGate = false;
            cFlag = false;
        }
        
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
        printf("Derivative of smoothed signal: %.2f\n",dvdt_mean);
        if (high.flag && high.value > 0) {
            printf("High peak found at: %.2f",high.value); //this will be delayed!
                   }
        if (low.flag && low.value > 0) {
            printf("Low peak found at %.2f",-low.value); //this will be delayed!
        }
        
        //timedelay!
        delay((int)(1/fs));
        }
    return 0;
    }
