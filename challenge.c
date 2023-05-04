// Include the Adafruit Circuit Playground library
#include <Adafruit_CircuitPlayground.h>

// Constants and global variables
const int MAX_SEQUENCE_LENGTH = 20;
float recordedSequence[MAX_SEQUENCE_LENGTH][3]; // Array to store the motion sequence
int sequenceLength = 0; // Keeps track of the sequence length
const float alpha = 0.5; //alpha vlaue for low pass filter

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

  Serial.println("Sequence recorded."); // Inform the user that the recording is done
}

// Function to compare the recorded sequence with a new one
bool compareSequence() {
  Serial.println("Enter sequence..."); // Ask the user to enter a new sequence
  delay(1000); // Wait for 1 second

  // Compare the new sequence with the recorded one
  for(int i = 0; i < sequenceLength; i++) {
    // Calculate the differences in X, Y, and Z values
    float deltaX = recordedSequence[i][0] - CircuitPlayground.motionX();
    float deltaY = recordedSequence[i][1] - CircuitPlayground.motionY();
    float deltaZ = recordedSequence[i][2] - CircuitPlayground.motionZ();
    // Calculate the distance between the two sets of values
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
    // Check if the distance is within the tolerance threshold
    if(distance > 3.5) {
      return false;
    }
    delay(100); // Wait for 100ms between readings
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
void indicateRecording()
{
  CircuitPlayground.clearPixels(); // Clear the LEDs
  for(int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0); //Set Red LED to indicate the error
  }
  CircuitPlayground.playTone(1000,1000); // Alarm for error
  CircuitPlayground.clearPixels(); // Clear the LEDs again
}
/*
  @param data Input signal to be processed
  @param index Index of input signal
  @param data Dimention of input signal

  A software implementation of a simple low pass filter
*/
void lowPass(float data, int index, int dim)
{
  recordedSequence[dim][index] = alpha * data + (1-alpha) * recordedSequence[dim][index-1];
  recordedSequence[dim][index-1] = recordedSequence[dim][index];
}

// Main loop function
void loop() {
  // If the left button is pressed, record a new sequence
  if(CircuitPlayground.leftButton()) {
    sequenceLength = 0;
    recordSequence();
  }
  // If the right button is pressed, compare the new sequence with the recorded one
  if(CircuitPlayground.rightButton()) {
    if(compareSequence()) {
      Serial.println("Success!"); // If the sequences match, print "Success!" and indicate it with LEDs
      indicateSuccess();
    } else {
      Serial.println("Failure."); // If the sequences don't match, print "Failure."
      indicateFailure();
    }
  }
}
