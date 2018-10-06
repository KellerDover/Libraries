/*
  RazorIMU_9DOF.h - Library to deal with Razor 9 DOF IMU
  Copyright (c) 2018 RoboManipal. All right reserved
  File created by : Avneesh Mishra
  Last commit done by : Avneesh Mishra
*/
// Include library
#include "RazorIMU_9DOF.h"

// ################### Constructors #######################
RazorIMU_9DOF::RazorIMU_9DOF() {
    // Empty constructor
}
RazorIMU_9DOF::RazorIMU_9DOF(Stream *AttachedSerial) {
    // Connect IMU Serial
    this->AttachIMUSerial(AttachedSerial);
}

// ############# Member Functions #########################
// Attach Serial
void RazorIMU_9DOF::AttachIMUSerial(Stream *AttachedSerial) {
    // Connect IMU Serial
    this->IMU_Serial = AttachedSerial;
}

// Get raw data from the serial
void RazorIMU_9DOF::GrabData() {
    // Send out a '#f' on the serial
    this->IMU_Serial->println("#f");
    // Debugger message about the inquiry
    this->debugger.print("Sending a '#f' to IMU serial", DEBUG); 
    // Wait until response
    while(!this->IMU_Serial->available()) {
        // Do nothing
    }
    // Debugger message (Level: DEBUG)
    // Values Y: %YAW% P: %PITCH% R: %ROLL%
    // For example:
    // Values Y: -172 P: 96 R: 100
    String msg = "Values";
    // Output is like : #YPR=<YAW>,<PITCH>,<ROLL>
    // In case the IMU side has been modified to send in another order
    if (this->IMU_Serial->available()) {
        this->IMU_Serial->read(); // Read the '#' in the beginning
        // Get the order of 'Y', 'P' and 'R' as strings
        String s1 = this->IMU_Serial->readStringUntil('=');
        for (int i = 0; i < s1.length(); i++) {
            String s_buffer = this->IMU_Serial->readStringUntil(',');
            int c;   // Command sent
            if (s1[i] == 'Y') {
                msg.concat(" Y: ");
                c = YAW;
            } else if (s1[i] == 'P') {
                msg.concat(" P: ");
                c = PITCH;
            } else if (s1[i] == 'R') {
                msg.concat(" R: ");
                c = ROLL;
            }
            // Assign the PITCH, ROLL and YAW values
            this->PRY_raw_values[c] = s_buffer.toFloat();
            msg.concat(this->PRY_raw_values[c]);
        }
    }
    // Reading done
    this->debugger.print(msg, DEBUG);
}

// Convert raw data to full scale: 0 to 360 degree data
void RazorIMU_9DOF::Calculate_fullScales() {
    // Debugger message (Level: DEBUG)
    // Full scale PRY values: %FS_PITCH% %FS_ROLL% %FS_YAW%
    String msg = "Full scale PRY values: ";
    for (int i = 0; i < 3; i++) {
        /*
        *    If raw > 0 => 0 to 180, then let it be as it is
        *    else if raw < 0 => -180 to -0, then it must be converted to
        *       full scale by adding 360 to the negative value
        *       Basically, it's 360 - |raw|, but since raw is already negative
        *       we can do 360 + raw.
        */
        if (this->PRY_raw_values[i] >= 0) {
            this->PRY_full_scale[i] = this->PRY_raw_values[i];
        } else {
            this->PRY_full_scale[i] = 360 + this->PRY_raw_values[i];
        }
        msg.concat(this->PRY_full_scale[i]);
        msg.concat(" ");
    }
    this->debugger.print(DEBUG, msg);
}

// Update all the PRY parameters
void RazorIMU_9DOF::UpdateData() {
    // Grab the data from GrabData function
    this->GrabData();
    // Update the full scale values
    this->Calculate_fullScales();
}

// Get reference values for full scale values
void RazorIMU_9DOF::GrabReference() {
    // Reset the reference values
    this->UpdateData();
    // Debugger message (Level: INFO)
    // PRY reference set to: %PITCH_REF% %ROLL_REF% %YAW_REF%
    // For example:
    // PRY reference set to: 187 90 86
    String msg = "PRY reference set to: ";
    for (int i = 0; i < 3; i++) {
        this->PRY_full_scale_ref[i] = PRY_full_scale[i];
        msg.concat(this->PRY_full_scale_ref[i]);
        msg.concat(" ");
    }
    this->debugger.print(msg, INFO);
}
// Get new reference
void RazorIMU_9DOF::ResetReference() {
    this->GrabReference();    // Get new reference
}
// -------- Getting raw PRY values from IMU ----------
float RazorIMU_9DOF::GetRaw_PITCH() {
    // Read Pitch
    return this->PRY_raw_values[PITCH];
}
float RazorIMU_9DOF::GetRaw_ROLL() {
    // Read Roll
    return this->PRY_raw_values[ROLL];
}
float RazorIMU_9DOF::GetRaw_YAW() {
    // Read Yaw
    return this->PRY_raw_values[YAW];
}
// Getting the full scale values
float RazorIMU_9DOF::GetPitch() {
    // Pitch
    return this->PRY_full_scale[PITCH];
}
float RazorIMU_9DOF::GetRoll() {
    // Roll
    return this->PRY_full_scale[ROLL];
}
float RazorIMU_9DOF::GetYaw() {
    // Yaw
    return this->PRY_full_scale[YAW];
}
