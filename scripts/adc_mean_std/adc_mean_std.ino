#include <math.h>
#include <driver/adc.h>
#include <soc/adc_channel.h>

#define NR_SAMPLES_RB_ADC 5
#define nr_of_elements_of(x) (sizeof(x) / sizeof((x)[0]))

int step_adc_last;
//int ring_buffer_adc[NR_SAMPLES_RB_ADC];
int ring_buffer_adc[] = {1,2,3,4,5};
int step_adc_filt;
int pointer_ring_buffer = 0;

// SETUP + LOOP ==============================================================

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(500);
  
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_GPIO34_CHANNEL,ADC_ATTEN_DB_0);

  // Initial values of ring_buffer_adc
  for(int i = 0; i < NR_SAMPLES_RB_ADC; i++){
    step_adc_last = adc1_get_raw(ADC1_GPIO34_CHANNEL);  
    update_ring_buffer_adc();
  }


  int sum_test = sum(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc));
  int mean_test = mean(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc));
  float std_test = stdev(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc));
  
  Serial.println((String) mean_test + " " + (String) sum_test + " " + (String) std_test );
}

void loop() {  
  step_adc_last = adc1_get_raw(ADC1_GPIO34_CHANNEL);  
  update_ring_buffer_adc();
  Serial.println(step_adc_last - mean(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc)));

//  apply_sma_filter();
//  Serial.println( (String) step_adc_filt + "," + (String) step_adc_last);
  delay(5);
}

// FUNCTIONS ==============================================================

float stdev(int arr[], size_t arr_size){
  float val_sum_of_sq = 0;

  int val_mean = mean(arr, arr_size);
  for(int i = 0; i < arr_size; i++){
    val_sum_of_sq += pow( (float) arr[i] - val_mean ,2);
  }

  return sqrt( val_sum_of_sq / (float) (arr_size-1) );
}

int sum(int arr[], size_t arr_size){
  float val_sum = 0;
  for(int i = 0; i < arr_size; i++){
    val_sum += (float) arr[i];
  }
  return (int) val_sum;
}

int mean(int arr[], size_t arr_size){
  float sum_of_arr = (float) sum(arr, arr_size);
  return (int) sum_of_arr / arr_size;
}

void apply_sma_filter(){
  float val_tmp = 0;

  for(int i = 0; i < NR_SAMPLES_RB_ADC; i++){
    val_tmp += ((float) 1/NR_SAMPLES_RB_ADC) * (float) ring_buffer_adc[i];
  }
  step_adc_filt = (int) val_tmp;
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
