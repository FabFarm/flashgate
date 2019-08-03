

//
//  main.cpp
//  gateOpener
//
//  Created by Philip Carstensen on 30.07.19.
//  Copyright © 2019 Philip Carstensen. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//global variables
//initialize some variables

    float v;
    
    float v_mean;
    float dvdt_mean;
    float dvdt;

    float lastdvdt[4];
    float lastv[7];
    
    bool cFlag = false;
    bool code[3] = {true, true, false}; //change also code size n in codeRec function call
    bool openGate = false;
    double pheight = 2; //min peak height 0.9 for test signal

    //time variables
    long start = 0;
    long fin = 0;
    long now = 0;
    long runstart = 0;
    long runtime = 0 ;
    long peaktime = 0;
    long t = millis();
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
float maximum(float array[], int n) {
    float maxi = 0;
    for (int i = 0; i < n; i++) {
        if (*(array+i) > maxi) {
            maxi = *(array+i);
        }
    }
    return maxi;
}

//function to find minimum
float minimum(float array[], int n) {
    float mini = array[0];
    for (int i = 1; i < n; i++) {
        if (*(array+i) < mini) {
            mini = *(array+i);
        }
    }
    return mini;
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
    deltat = ((double)(dx[0]-dx[1])/1000); // /CLOCKS_PER_SEC; for use of real time
    //build derivative
    if (deltat == 0) {
        return 0;
    }
    else return (deltaf/deltat);
}

//function to find peak in signal
struct fpeak_return fpeak(struct fpeak_return input, float sig)    {
    input.flag = false;
    //find peak
    if (sig > maximum(input.poldq.val, 10) && sig > pheight) {
        input.poldq = streaming(input.poldq, sig);
        input.counter = 1;
    }
    else    {
        //no new peak?
        input.counter++;
    }
    //when no new peak in 7 steps set peak
    if (input.counter == 15) {
        input.value = maximum(input.poldq.val, 10);
        input.flag = true;
        peaktime = millis();
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
bool codeRecognition(struct stream t, bool code[], int n)   {
    // code means short signal -> true, long signal -> false
    // n defines length of code
    bool codeFlag = true;
    float last[n];
    readLast(t, last, n);
    
    //set short/long definition
    double shortmin = minimum(last,n)*0.75;
    double shortmax = minimum(last,n)*1.25;

    double longmin = maximum(last,n)*0.75;
    double longmax = maximum(last,n)*1.25;
    
    //compare last n signals to code
    for (int i = 0; i<n; i++) {
        if (last[n-(i+1)] >= shortmin && last[n-(i+1)] <= shortmax && last[n-(i+1)] >0 && code[i] && codeFlag) {
            //n-(i+1) because of different order last/code
            //code demands short and signal is short
            codeFlag = true;
        }
        else if(last[n-(i+1)] >= longmin && last[n-(i+1)] <= longmax && last[n-(i+1)] >0 && !code[i] && codeFlag)   {
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

void setup() {
  Serial.begin(9600);

}

void loop() {
        
        //Read current voltage value
        v = ((float) analogRead(A3))/1023*5;
        Serial.println();
        Serial.println(v);
        Serial.println();
        
        //save to data stream
        vdataq = streaming(vdataq,v);
        
        /*Signal processing*/
       
        //selfmade med filter (mean value) - smooth signal!
        readLast(vdataq,lastv,5);
        v_mean = med(lastv, 7);
        
        //write to buffer
        v_medq = streaming(v_medq,v_mean);
        
        //save time for derivative
        
        t = millis(); //for later real time use?
        Serial.print("time in secs: ");
        Serial.print(t / 1000);
        Serial.println();
        timeq = streaming(timeq,t); //use t for now... change derivative function!!
        //take derivative
        dvdt = derivative(v_medq,timeq);
        // write to buffer
        dvdtq = streaming(dvdtq, dvdt);
        //selfmade med filter (mean value) - smoothe derivative!
        readLast(dvdtq,lastdvdt,4);
        dvdt_mean = med(lastdvdt, 4);
        Serial.print("Signal processed ");
        Serial.print(dvdt_mean);
        Serial.println();
        //find peaks
        high = fpeak(high, dvdt_mean);
        low = fpeak(low, -dvdt_mean);
        
        /*code recognition*/
        now = millis();
        if (high.flag) {
            start = millis();
            //led digital write high
          Serial.print("High peak at: ");
          Serial.print(high.value);
          Serial.println();
          //delay(3000);
          }
        else if (low.flag) {
            fin = millis();
            cTimesq = streaming(cTimesq,((float) (fin-start) / 1000));
            //led digital write low
          Serial.print("Low peak at: ");
          Serial.print(-low.value);
          Serial.println();
          //delay(3000);
        }
        else if ((float) ((now-start) / 1000) >= 10)  {
            start = 0;
            fin = 0;
        }
        cFlag = codeRecognition(cTimesq, code, 3);
        
        if (cFlag) {
            Serial.println("Code recognized");
            runstart = millis();
            runtime = runstart;
            while (!openGate && ((float) (runtime - runstart) / 1000 <= 10)) { //also time counter to avoid arduino getting stuck !openGate && runtime < maxRuntime
                //led green
                delay(3000);
                openGate = true;
                runtime = millis();
            }
            openGate = false;
            cFlag = false;
            //reset for new code search
            for (int i = 0; i < 10; i++) {
              cTimesq.val[i] = 0;
            }
            cTimesq.tail = 0;
            
        }
    delay(10); //circa 100 hertz, hopefully
    }

