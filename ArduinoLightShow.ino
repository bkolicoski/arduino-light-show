/*
  Arduino Light Show

  Controls 5 pieces of 12V LED strips based on the sound levels from
  an attached microphone. The project is a part of the Makers Rock 2018
  Album Art Collab on YouTube, build by Taste The Code

  Circuit:
  https://easyeda.com/bkolicoski/Arduino_Light_Show-506c62032b0646ae8e1636519d0d17fc

  created: 
  February 2018
  by bkolicoski

  Build video:
  https://www.youtube.com/watch?v=iJpYhWXhLvk

  Instructable:
  https://www.instructables.com/id/Arduino-Based-Light-Show

  Parts of the code are taken from Adafruit sketch for measuring sound levels
  https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
  
*/

const int analogInPin = A0;  // Analog input pin that the microphone is attached to

#define NOISE          10    // Noise/hum/interference in mic signal
#define AVG_COUNTS     20    // Number of samples to be used in calculating an average volume level

int sensorValue = 0;         // value read from the microphone amp
int outputV = 0;             // used to store the output value of the processed signal
int factor = 1;              // the initial adjustment factor to work with different volume levels

const int sampleWindow = 25; // sample window width in mS
unsigned int sample;         // holds the current sample value
int avg[AVG_COUNTS];         // array to store the samples 
int cycle = 0;               // holds the current cycle step

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  // empty out the array
  memset(avg,0,sizeof(int)*AVG_COUNTS);

  //define digital output pins for the LED strips
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop() {
  // make sure to turn off the outputs on each cycle
  // this gives a shimmering effect on the strips
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);

  //set starting values 
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // read the analog in value
  sensorValue = analogRead(analogInPin);
  outputV = map(sensorValue, 350, 450, 0, 900);

  // collect data for 25 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(analogInPin);
    if (sample < 1024)  // toss out spurious readings
    {
       if (sample > signalMax) {
          signalMax = sample;  // save just the max levels
       }
       else if (sample < signalMin) {
          signalMin = sample;  // save just the min levels
       }
    }
  }

  //calculate peak amplitude in period
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  peakToPeak   = (peakToPeak <= NOISE) ? 0 : (peakToPeak - NOISE);

  //set an upper peak limit
  if (peakToPeak > 1024) {
    peakToPeak = 1024;
  }

  // assign the peak to peak difference to the averages array
  avg[cycle] = peakToPeak;
  cycle++;
  if (cycle == AVG_COUNTS) {
    int maximum = 0;
    for(int i = 0; i < AVG_COUNTS; i++) {
      if ( avg[i] > 1024) {
        continue;
      }
      if ( avg[i] > maximum) {
        maximum = avg[i];
      }
    }

    //calculate factor for next itteration based on maximum volume
    factor = 1024 / maximum;
    cycle = 0;
  }

  //set the output value
  outputV = peakToPeak * factor;

  // set outputs based on measured level
  if (outputV > 990) {
    digitalWrite(12, HIGH);
  } else if (outputV > 810) {
    digitalWrite(11, HIGH);
  }else if (outputV > 630) {
    digitalWrite(10, HIGH);
  }else if (outputV > 450) {
    digitalWrite(9, HIGH);
  }else if (outputV > 270) {
    digitalWrite(8, HIGH);
  }

  // print the results to the Serial Monitor:
  Serial.print("0");
  Serial.print("\t");
  Serial.print(outputV);
  Serial.print("\t");
  Serial.println("1024");


  // wait 20 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading and for the music to change significantly
  delay(20);
}
