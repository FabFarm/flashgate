#include <math.h>
#include <driver/adc.h>
#include <soc/adc_channel.h>

#define NR_SAMPLES_RB_ADC 100
#define NR_SAMPLES_SMA 10
#define nr_of_elements_of(x) (sizeof(x) / sizeof((x)[0]))

// VARIABLES =================================================================

int step_adc_last;
int step_adc_filt;
int ring_buffer_adc[NR_SAMPLES_RB_ADC];
int pointer_rb_adc = 0;
int ring_buffer_sma[NR_SAMPLES_SMA];
int pointer_rb_sma = 0;
int line_stdev_plus;
int line_stdev_min;

struct noise_struct {
  int mean;
  float stdev;
  int threshold;
};
noise_struct noise_profile = {0,0.0, 0};

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

//  int sum_test = sum(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc));
  noise_profile.mean = mean(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc));
  noise_profile.stdev = stdev(ring_buffer_adc, nr_of_elements_of(ring_buffer_adc));
}

void loop() {  
  step_adc_last = adc1_get_raw(ADC1_GPIO34_CHANNEL);  
  update_ring_buffer_adc();

  apply_sma_filter();
  update_noise_profile();

  line_stdev_plus = noise_profile.mean + (int) noise_profile.stdev;
  line_stdev_min = noise_profile.mean - noise_profile.stdev;

  Serial.println( (String) noise_profile.mean + "," + (String) line_stdev_plus + "," + (String) line_stdev_min);

  delay(100);
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
  int i_rb;

  for(int i = 0; i < NR_SAMPLES_SMA; i++){
    i_rb = NR_SAMPLES_RB_ADC - i;
    val_tmp += ((float) 1/NR_SAMPLES_SMA) * (float) ring_buffer_adc[i_rb];
  }
  step_adc_filt = (int) val_tmp;
}

void update_noise_profile(){
  noise_profile.mean = mean(ring_buffer_adc, NR_SAMPLES_RB_ADC);
  noise_profile.stdev = stdev(ring_buffer_adc, NR_SAMPLES_RB_ADC);
}

void update_ring_buffer_adc(){
  ring_buffer_adc[pointer_rb_adc] = step_adc_last;
  pointer_rb_adc = (pointer_rb_adc + 1) % NR_SAMPLES_RB_ADC;
}

void print_ring_buffer(){
  for(int i = 0; i < NR_SAMPLES_RB_ADC; i++){
    Serial.print((String) ring_buffer_adc[i] + " ");
  }
  Serial.println();  
}
