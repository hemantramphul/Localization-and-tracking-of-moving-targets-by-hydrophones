// """
// Assignment Title: Localization and tracking of moving targets by hydrophones
// Purpose         : Implementation a sound triangulation system using three Hydrophones to locate the source of a sound. 
//                   The code defines the pins for each Hydrophone and their positions in space, then initializes the serial 
//                   communication and sets the pins as input. In the loop, the code reads the analog values from each Hydrophone, 
//                   calculates the time delay between them, and then the time difference of arrival (TDOA) for each pair of 
//                   Hydrophones. Using triangulation, the code then calculates the x and y coordinates of the sound source and 
//                   prints them to the serial monitor. Calculate the distance between the sound source and each Hydrophone using 
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

const int hydrophone1 = A0; // Hydrophone 1 connected to analog pin A0
const int hydrophone2 = A1; // Hydrophone 2 connected to analog pin A1
const int hydrophone3 = A2; // Hydrophone 3 connected to analog pin A2
int LED1 = 8; 
int LED2 = 9;
int LED3 = 10;

const float soundSpeed = 343.0; // Speed of sound in m/s

const float hydrophonePositions[3][2] = {
  {0.0, 0.0},   // Hydrophone 1 position (x, y) in meters
  {0.25, 0.0},  // Hydrophone 2 position (x, y) in meters
  {0.0, 0.25}   // Hydrophone 3 position (x, y) in meters
};

// Declare the calculateDelay function
int calculateDelay(int hydrophone1, int hydrophone2) {
  // Find the time at which hydrophone1's waveform crosses a threshold
  int threshold = 640; // Adjust this value as needed
  int time1 = 0;
  while (analogRead(hydrophone1) < threshold) {
    time1++;
    delay(1);
  }

  // Find the time at which hydrophone2's waveform crosses the same threshold
  int time2 = 0;
  while (analogRead(hydrophone2) < threshold) {
    time2++;
    delay(1);
  }
  
  // Return the time difference between the two waveforms
  return time2 - time1;
}

// Calculate the distance between a Hydrophone and the sound source
float calculateDistance(int hydrophoneIndex, float x, float y, const float hydrophonePositions[][2]) {
  float dx = x - hydrophonePositions[hydrophoneIndex][0];
  float dy = y - hydrophonePositions[hydrophoneIndex][1];
  return sqrt(dx * dx + dy * dy);
}

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

void setup() {
  Serial.begin(9600);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  pinMode(hydrophone1, INPUT);
  pinMode(hydrophone2, INPUT);
  pinMode(hydrophone3, INPUT);
}

void loop() {
  // Read the analog value from each Hydrophone
  int hydrophone1val = analogRead(hydrophone1);
  int hydrophone2val = analogRead(hydrophone2);
  int hydrophone3val = analogRead(hydrophone3);

  // Calculate the time delay between Hydrophones
  int delay12 = calculateDelay(hydrophone1val, hydrophone2val);
  int delay13 = calculateDelay(hydrophone1val, hydrophone3val);

  // Calculate the time difference of arrival (TDOA) between Hydrophones
  float tdoa12 = (float)delay12 / 1000000.0 * soundSpeed;
  float tdoa13 = (float)delay13 / 1000000.0 * soundSpeed;

  // Calculate the x and y coordinates of the sound source using triangulation
  float x = ((hydrophonePositions[0][0] - hydrophonePositions[1][0]) * (hydrophonePositions[0][0] + hydrophonePositions[1][0] + 2.0 * tdoa12) - (hydrophonePositions[0][0] - hydrophonePositions[2][0]) * (hydrophonePositions[0][0] + hydrophonePositions[2][0] + 2.0 * tdoa13)) / (4.0 * (hydrophonePositions[0][1] - hydrophonePositions[1][1]) * (hydrophonePositions[0][0] - hydrophonePositions[2][0]) - 4.0 * (hydrophonePositions[0][1] - hydrophonePositions[2][1]) * (hydrophonePositions[0][0] - hydrophonePositions[1][0]));
  float y = ((hydrophonePositions[0][1] - hydrophonePositions[1][1]) * (hydrophonePositions[0][1] + hydrophonePositions[1][1] + 2.0 * tdoa12) - (hydrophonePositions[0][1] - hydrophonePositions[2][1]) * (hydrophonePositions[0][1] + hydrophonePositions[2][1] + 2.0 * tdoa13)) / (4.0 * (hydrophonePositions[0][0] - hydrophonePositions[1][0]) * (hydrophonePositions[0][1] - hydrophonePositions[2][1]) - 4.0 * (hydrophonePositions[0][0] - hydrophonePositions[2][0]) * (hydrophonePositions[0][1] - hydrophonePositions[1][1]));

  // Calculate the distance between the sound source and each Hydrophone
  float distances[3];
  for (int i = 0; i < 3; i++) {
    distances[i] = calculateDistance(i, x, y, hydrophonePositions);
  }

  // Determine the nearest Hydrophone
  int nearestHydrophone = 0;
  float shortestDistance = distances[0];
  for (int i = 1; i < 3; i++) {
    if (distances[i] < shortestDistance) {
      shortestDistance = distances[i];
      nearestHydrophone = i;
    }
  }

  // Print the nearest Hydrophone
  Serial.print(nearestHydrophone + 1);
  Serial.print(" NH; x = ");
  // Print the x and y coordinates of the sound source
  Serial.print(x);
  Serial.print(", y = ");
  Serial.println(y);

  // Turn on the led for the coresponding Hydrophone 
  handleLeds(nearestHydrophone + 1);
}
