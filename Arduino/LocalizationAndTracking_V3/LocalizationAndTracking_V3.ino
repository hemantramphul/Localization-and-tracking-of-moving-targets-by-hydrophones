// """
// Assignment Title: Localization and tracking of moving targets by hydrophones
// Purpose         : Implementation a sound triangulation system using three hydrophones to locate the source of a sound. 
//                   The code defines the pins for each hydrophone and their positions in space, then initializes the serial 
//                   communication and sets the pins as input. In the loop, the code reads the analog values from each hydrophone, 
//                   calculates the time delay betw+6een them, and then the time difference of arrival (TDOA) for each pair of 
//                   hydrophones. Using triangulation, the code then calculates the x and y coordinates of the sound source and 
//                   prints them to the serial monitor. Calculate the distance between the sound source and each hydrophone using 
//                   the Pythagorean theorem.
// Language        : Arduino and Matlab
// Author          : Hemant Ramphul
// Github          : https://github.com/hemantramphul/Localization-and-tracking-of-moving-targets-by-hydrophones/
// Date            : 05 May 2023

// Université des Mascareignes (UdM)
// Faculty of Information and Communication Technology
// Master Artificial Intelligence and Robotics
// Official Website: https://udm.ac.mu
// """

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
//define colors
#define BLUE    0x001F

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int hydrophone1 = A0; // hydrophone 1 connected to analog pin A0
const int hydrophone2 = A1; // hydrophone 2 connected to analog pin A1
const int hydrophone3 = A2; // hydrophone 3 connected to analog pin A2
int LED1 = 8; 
int LED2 = 9;
int LED3 = 10;

// Speed of sound in m/s 
// Air sound speed 343.0 m/s 
// Underwater sound speed 1480.0 m/s 
const float soundSpeed = 1480.0; 

const float hPositions[3][2] = {
  {0.0, 0.0}, // hydrophone 1 position (x, y) in meters
  {0.25, 0.0}, // hydrophone 2 position (x, y) in meters
  {0.0, 0.25} // hydrophone 3 position (x, y) in meters
};

// Declare the calculateDelay function
int calculateDelay(int hydroVal1, int hydroVal2) {
  // Find the time at which hydrophone1's waveform crosses a threshold
  int threshold = 640; // Adjust this value as needed
  int time1 = 0;
  while (analogRead(hydroVal1) < threshold) {
    time1++;
    delay(1);
  }

  // Find the time at which hydrophone2's waveform crosses the same threshold
  int time2 = 0;
  while (analogRead(hydroVal2) < threshold) {
    time2++;
    delay(1);
  }
  
  // Return the time difference between the two waveforms
  return time2 - time1;
}

// Calculate the distance between a hydrophone and the sound source
float calculateDistance(int micIndex, float x, float y, const float hPositions[][2]) {
  float dx = x - hPositions[micIndex][0];
  float dy = y - hPositions[micIndex][1];
  return sqrt(dx * dx + dy * dy);
}

// Indicate the nearest hydrophone [1, 2, 3]
void handleLeds(int hydrophone) {
  if (hydrophone == 1) {
    digitalWrite(LED1, HIGH);
  }
  else if (hydrophone == 2) {
    digitalWrite(LED2, HIGH);
  }
  else if (hydrophone == 3) {
    digitalWrite(LED3, HIGH);
  }

  // Wait for 100ms before taking another measurement
  delay(1000);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

// Display function 
void displaySPI(int hydro, float x, float y) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Sound Localization");
    // Print the nearest hydrophone [NH]
  display.print("Hydrophone ");
  display.println(hydro + 1);
  // Print the x and y coordinates of the sound source
  display.print("x = ");
  display.println(x);
  display.print("y = ");
  display.println(y);
  display.display();
}

void setup() {
  Serial.begin(9600);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  pinMode(hydrophone1, INPUT);
  pinMode(hydrophone2, INPUT);
  pinMode(hydrophone3, INPUT);
}

void loop() {
  // Read the analog value from each hydrophone
  int hydrophone1val = analogRead(hydrophone1);
  int hydrophone2val = analogRead(hydrophone2);
  int hydrophone3val = analogRead(hydrophone3);

  // Calculate the time delay between hydrophones
  int delay12 = calculateDelay(hydrophone1val, hydrophone2val);
  int delay13 = calculateDelay(hydrophone1val, hydrophone3val);

  // Calculate the time difference of arrival (TDOA) between hydrophones
  float tdoa12 = (float)delay12 / 1000000.0 * soundSpeed;
  float tdoa13 = (float)delay13 / 1000000.0 * soundSpeed;

  // Calculate the x and y coordinates of the sound source using triangulation
  float x = ((hPositions[0][0] - hPositions[1][0]) * (hPositions[0][0] + hPositions[1][0] + 2.0 * tdoa12) - (hPositions[0][0] - hPositions[2][0]) * (hPositions[0][0] + hPositions[2][0] + 2.0 * tdoa13)) / (4.0 * (hPositions[0][1] - hPositions[1][1]) * (hPositions[0][0] - hPositions[2][0]) - 4.0 * (hPositions[0][1] - hPositions[2][1]) * (hPositions[0][0] - hPositions[1][0]));
  float y = ((hPositions[0][1] - hPositions[1][1]) * (hPositions[0][1] + hPositions[1][1] + 2.0 * tdoa12) - (hPositions[0][1] - hPositions[2][1]) * (hPositions[0][1] + hPositions[2][1] + 2.0 * tdoa13)) / (4.0 * (hPositions[0][0] - hPositions[1][0]) * (hPositions[0][1] - hPositions[2][1]) - 4.0 * (hPositions[0][0] - hPositions[2][0]) * (hPositions[0][1] - hPositions[1][1]));

  // Calculate the distance between the sound source and each hydrophone
  float distances[3];
  for (int i = 0; i < 3; i++) {
    distances[i] = calculateDistance(i, x, y, hPositions);
  }

  // Determine the nearest hydrophone
  int nearestHydrophone = 0;
  float shortestDistance = distances[0];
  for (int i = 1; i < 3; i++) {
    if (distances[i] < shortestDistance) {
      shortestDistance = distances[i];
      nearestHydrophone = i;
    }
  }

  // Display on LCD
  displaySPI (nearestHydrophone, x, y);

  // Print the nearest hydrophone [NH]
  Serial.print(nearestHydrophone + 1);
  Serial.print(" NH; x = ");
  // Print the x and y coordinates of the sound source
  Serial.print(x);
  Serial.print(", y = ");
  Serial.println(y);

  // Turn on the led for the coresponding hydrophone 
  handleLeds(nearestHydrophone + 1);
}
