// """
// Assignment Title: Localization and tracking of moving targets by hydrophones
// Purpose         : Implementation a sound triangulation system using three microphones to locate the source of a sound. 
//                   The code defines the pins for each microphone and their positions in space, then initializes the serial 
//                   communication and sets the pins as input. In the loop, the code reads the analog values from each microphone, 
//                   calculates the time delay between them, and then the time difference of arrival (TDOA) for each pair of 
//                   microphones. Using triangulation, the code then calculates the x and y coordinates of the sound source and 
//                   prints them to the serial monitor. Calculate the distance between the sound source and each microphone using 
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

const int mic1 = A0; // Microphone 1 connected to analog pin A0
const int mic2 = A1; // Microphone 2 connected to analog pin A1
const int mic3 = A2; // Microphone 3 connected to analog pin A2
int LED1 = 8; 
int LED2 = 9;
int LED3 = 10;

const float soundSpeed = 343.0; // Speed of sound in m/s

const float micPositions[3][2] = {
  {0.0, 0.0}, // Microphone 1 position (x, y) in meters
  {0.25, 0.0}, // Microphone 2 position (x, y) in meters
  {0.0, 0.25} // Microphone 3 position (x, y) in meters
};

// Declare the calculateDelay function
int calculateDelay(int mic1val, int mic2val) {
  // Find the time at which mic1's waveform crosses a threshold
  int threshold = 640; // Adjust this value as needed
  int time1 = 0;
  while (analogRead(mic1) < threshold) {
    time1++;
    delayMicroseconds(1);
  }

  // Find the time at which mic2's waveform crosses the same threshold
  int time2 = 0;
  while (analogRead(mic2) < threshold) {
    time2++;
    delayMicroseconds(1);
  }
  
  // Return the time difference between the two waveforms
  return time2 - time1;
}

// Calculate the distance between a microphone and the sound source
float calculateDistance(int micIndex, float x, float y, const float micPositions[][2]) {
  float dx = x - micPositions[micIndex][0];
  float dy = y - micPositions[micIndex][1];
  return sqrt(dx * dx + dy * dy);
}

void handleLeds(int mic) {
  if (mic == 1) {
    digitalWrite(LED1, HIGH);
  }
  else if (mic == 2) {
    digitalWrite(LED2, HIGH);
  }
  else if (mic == 3) {
    digitalWrite(LED3, HIGH);
  }

  // Wait for 100ms before taking another measurement
  delay(1000);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  pinMode(mic1, INPUT);
  pinMode(mic2, INPUT);
  pinMode(mic3, INPUT);
}

void loop() {
  // Read the analog value from each microphone
  int mic1val = analogRead(mic1);
  int mic2val = analogRead(mic2);
  int mic3val = analogRead(mic3);

  // Calculate the time delay between microphones
  int delay12 = calculateDelay(mic1val, mic2val);
  int delay13 = calculateDelay(mic1val, mic3val);

  // Calculate the time difference of arrival (TDOA) between microphones
  float tdoa12 = (float)delay12 / 1000000.0 * soundSpeed;
  float tdoa13 = (float)delay13 / 1000000.0 * soundSpeed;

  // Calculate the x and y coordinates of the sound source using triangulation
  float x = ((micPositions[0][0] - micPositions[1][0]) * (micPositions[0][0] + micPositions[1][0] + 2.0 * tdoa12) - (micPositions[0][0] - micPositions[2][0]) * (micPositions[0][0] + micPositions[2][0] + 2.0 * tdoa13)) / (4.0 * (micPositions[0][1] - micPositions[1][1]) * (micPositions[0][0] - micPositions[2][0]) - 4.0 * (micPositions[0][1] - micPositions[2][1]) * (micPositions[0][0] - micPositions[1][0]));
  float y = ((micPositions[0][1] - micPositions[1][1]) * (micPositions[0][1] + micPositions[1][1] + 2.0 * tdoa12) - (micPositions[0][1] - micPositions[2][1]) * (micPositions[0][1] + micPositions[2][1] + 2.0 * tdoa13)) / (4.0 * (micPositions[0][0] - micPositions[1][0]) * (micPositions[0][1] - micPositions[2][1]) - 4.0 * (micPositions[0][0] - micPositions[2][0]) * (micPositions[0][1] - micPositions[1][1]));

  // Calculate the distance between the sound source and each microphone
  float distances[3];
  for (int i = 0; i < 3; i++) {
    distances[i] = calculateDistance(i, x, y, micPositions);
  }

  // Determine the nearest microphone
  int nearestMic = 0;
  float shortestDistance = distances[0];
  for (int i = 1; i < 3; i++) {
    if (distances[i] < shortestDistance) {
      shortestDistance = distances[i];
      nearestMic = i;
    }
  }

  // Print the nearest microphone
  Serial.print(nearestMic + 1);
  Serial.print(" NM; x = ");
  // Print the x and y coordinates of the sound source
  Serial.print(x);
  Serial.print(", y = ");
  Serial.println(y);

  // Turn on the led for the coresponding microphone 
  handleLeds(nearestMic + 1);
}
