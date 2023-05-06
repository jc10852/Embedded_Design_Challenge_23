// Include the Adafruit Circuit Playground library
#include <Adafruit_CircuitPlayground.h>

// Constants and global variables
const int MAX_SEQUENCE_LENGTH = 20;
float recordedSequence[3][MAX_SEQUENCE_LENGTH]; // Array to store the motion sequence
int sequenceLength = 0; // Keeps track of the sequence length
const float alpha = 0.5; //alpha vlaue for low pass filter

// Setup function runs once at the beginning
void setup() {
  Serial.begin(9600); // Initialize serial communication
  CircuitPlayground.begin(); // Initialize Circuit Playground
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

// Function to record the motion sequence
void recordSequence() {
  Serial.println("Recording..."); // Inform the user that recording has started
  delay(1000); // Wait for 1 second

  // Record motion data until the maximum sequence length is reached
  while(sequenceLength < MAX_SEQUENCE_LENGTH) {
    // Record X, Y, and Z values of motion and process via low pass filter
    lowPass(CircuitPlayground.motionX(), sequenceLength, 0);
    lowPass(CircuitPlayground.motionX(), sequenceLength, 1);
    lowPass(CircuitPlayground.motionX(), sequenceLength, 2);
    sequenceLength++; // Increment the sequence length
    delay(50); // Wait for 100ms between readings
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
    float deltaX = abs(recordedSequence[0][i] - CircuitPlayground.motionX());
    float deltaY = abs(recordedSequence[1][i] - CircuitPlayground.motionY());
    float deltaZ = abs(recordedSequence[2][i] - CircuitPlayground.motionZ());
    // Calculate the distance between the two sets of values
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
    // Check if the distance is within the tolerance threshold
    if(distance > 10) {
      return false;
    }
    delay(50); // Wait for 100ms between readings
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

// Main loop function
void loop() {
  // If the left button is pressed, record a new sequence
  if(CircuitPlayground.leftButton()) {
    sequenceLength = 0;
    recordSequence();
    Serial.print("[");
    for(auto element: recordedSequence[0]){
      Serial.print(element);
      Serial.print(", ");
    }
    Serial.print("]");
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




