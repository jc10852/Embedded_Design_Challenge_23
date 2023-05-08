// Include the Adafruit Circuit Playground library
#include <Adafruit_CircuitPlayground.h>
// Constants and global variables
const int MAX_SEQUENCE_LENGTH = 20;
float recordedSequence[MAX_SEQUENCE_LENGTH][3]; // Array to store the motion sequence
int sequenceLength = 0; // Keeps track of the sequence length
const float alpha = 0.5; //alpha vlaue for low pass filter
void indicateRecording();
void lowPass(float data, int index, int dim);
// Setup function runs once at the beginning
void setup() {
  Serial.begin(9600); // Initialize serial communication
  CircuitPlayground.begin(); // Initialize Circuit Playground
}

// Function to record the motion sequence
void recordSequence() {
  Serial.println("Recording..."); // Inform the user that recording has started
  delay(1000); // Wait for 1 second
  // Record motion data until the maximum sequence length is reached
  while(sequenceLength < MAX_SEQUENCE_LENGTH) {
    // Record X, Y, and Z values of motion
    recordedSequence[sequenceLength][0] = CircuitPlayground.motionX();
    recordedSequence[sequenceLength][1] = CircuitPlayground.motionY();
    recordedSequence[sequenceLength][2] = CircuitPlayground.motionZ();
    sequenceLength++; // Increment the sequence length
    delay(100); // Wait for 100ms between readings
  }
  CircuitPlayground.clearPixels();
  Serial.println("Sequence recorded."); // Inform the user that the recording is done
}

// Function to compare the recorded sequence with a new one
bool compareSequence() {
  Serial.println("Enter sequence..."); // Ask the user to enter a new sequence
  delay(1000); // Wait for 1 second

  // Apply low pass filter to the recorded data
  for(int i = 1; i < sequenceLength; i++) {
    lowPass(recordedSequence[i][0], i, 0); // Filter X data
    lowPass(recordedSequence[i][1], i, 1); // Filter Y data
    lowPass(recordedSequence[i][2], i, 2); // Filter Z data
  }
  // Record motion data until the maximum sequence length is reached
  float inputSequence[MAX_SEQUENCE_LENGTH][3]; // Array to store the input motion sequence
  int inputSequenceLength = 0; // Keeps track of the input sequence length
  while(inputSequenceLength < MAX_SEQUENCE_LENGTH) {
    // Record X, Y, and Z values of motion
    inputSequence[inputSequenceLength][0] = CircuitPlayground.motionX();
    inputSequence[inputSequenceLength][1] = CircuitPlayground.motionY();
    inputSequence[inputSequenceLength][2] = CircuitPlayground.motionZ();
    inputSequenceLength++; // Increment the input sequence length
    delay(100); // Wait for 100ms between readings
  }
  CircuitPlayground.clearPixels();
  delay(200);
  // Apply low pass filter to the input sequence
  for(int i = 1; i < inputSequenceLength; i++) {
    lowPass(inputSequence[i][0], i, 0); // Filter X data
    lowPass(inputSequence[i][1], i, 1); // Filter Y data
    lowPass(inputSequence[i][2], i, 2); // Filter Z data
 }

  // Compare the new sequence with the recorded one
  for(int i = 0; i < sequenceLength; i++) {
    // Calculate the differences in X, Y, and Z values
    float deltaX = recordedSequence[i][0] - inputSequence[i][0];
    float deltaY = recordedSequence[i][1] - inputSequence[i][1];
    float deltaZ = recordedSequence[i][2] - inputSequence[i][2];
    // Calculate the distance between the two sets of values
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
    // Check if the distance is within the tolerance threshold
    Serial.println(distance);
    if(distance > 1.75) {
      return false;
    }
  }
  return true; // If the entire sequence matches, return true
}

// Function to indicate success with the Circuit Playground's LEDs
void indicateSuccess() {
  CircuitPlayground.clearPixels(); // Clear the LEDs

  // Light up each LED in green with a 50ms delay between them
  for(int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 255, 0);
    delay(50);
  }

  CircuitPlayground.clearPixels(); // Clear the LEDs again
}
void indicateFailure()
{
  CircuitPlayground.clearPixels(); // Clear the LEDs
  for(int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0); //Set Red LED to indicate the error
  }
  CircuitPlayground.playTone(1000,1000); // Alarm for error
  CircuitPlayground.clearPixels(); // Clear the LEDs again
}
void indicateRecording() {
  CircuitPlayground.clearPixels(); // Clear the LEDs
  for(int i = 0; i < 10; i++) { // Loop through the first 8 LEDs
    CircuitPlayground.setPixelColor(i, 122, 122, 0); //Set Yellow LED to indicate the recording process
    delay(75); // Wait for 100ms
  }
  CircuitPlayground.clearPixels(); // Clear the LEDs
}

/*
  @param data Input signal to be processed
  @param index Index of input signal
  @param data Dimention of input signal
  A software implementation of a simple low pass filter
*/
void lowPass(float data, int index, int dim)
{
  recordedSequence[index][dim] = alpha * data + (1-alpha) * recordedSequence[index-1][dim];
  recordedSequence[index-1][dim] = recordedSequence[index][dim];
}

// Main loop function
void loop() {
  // If the left button is pressed, record a new sequence
  if(CircuitPlayground.leftButton()) {
    sequenceLength = 0;
    indicateRecording();
    recordSequence();
  }
  // If the right button is pressed, compare the new sequence with the recorded one
  if(CircuitPlayground.rightButton()) {
    indicateRecording();
    if(compareSequence()) {
      Serial.println("Success!"); // If the sequences match, print "Success!" and indicate it with LEDs
      indicateSuccess();
    } else {
      Serial.println("Failure."); // If the sequences don't match, print "Failure."
      indicateFailure();
    }
  }
}
