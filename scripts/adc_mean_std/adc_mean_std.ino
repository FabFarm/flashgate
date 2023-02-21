#include <driver/adc.h>
#include <soc/adc_channel.h>

#define NR_SAMPLES_RB_ADC 5


int step_adc_last;
int ring_buffer_adc[NR_SAMPLES_RB_ADC];
int step_adc_filt;
int pointer_ring_buffer = 0;

// SETUP + LOOP ==============================================================

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_GPIO34_CHANNEL,ADC_ATTEN_DB_0);

  // Initial values of ring_buffer_adc
  for(int i = 0; i < NR_SAMPLES_RB_ADC; i++){
    ring_buffer_adc[i] = 0;
  }
}

void loop() {  
  step_adc_last = adc1_get_raw(ADC1_GPIO34_CHANNEL);  

  update_ring_buffer_adc();

  calculate_mean(ring_buffer_adc);

  
//  Serial.println( (String) step_adc_filt + "," + (String) step_adc_last);
  delay(5);
}

// FUNCTIONS ==============================================================

int calculate_mean(int arr[]){
  int val_tmp;
  for(int i = 0; i < NR_SAMPLES_RB_ADC; i++){
    val_tmp = (float) ring_buffer_adc[i] / (NR_SAMPLES_RB_ADC;
  }
  Serial.println("val_tmp:" + (Strin));
}

void update_ring_buffer_adc(){
  ring_buffer_adc[pointer_ring_buffer] = step_adc_last;
  pointer_ring_buffer = (pointer_ring_buffer + 1) % NR_SAMPLES_RB_ADC;
}

void print_ring_buffer(){
  for(int i = 0; i < NR_SAMPLES_RB_ADC; i++){
    Serial.print((String) ring_buffer_adc[i] + " ");
  }
  Serial.println();  
}
