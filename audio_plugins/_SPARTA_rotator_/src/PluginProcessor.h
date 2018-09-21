/*
 ==============================================================================
 
 This file is part of SPARTA; a suite of spatial audio plug-ins.
 Copyright (c) 2018 - Leo McCormack.
 
 SPARTA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 SPARTA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with SPARTA.  If not, see <http://www.gnu.org/licenses/>.
 
 ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "rotator.h"
#include <string.h>

#define MAX_NUM_CHANNELS 64
#define BUILD_VER_SUFFIX "alpha"            /* String to be added before the version name on the GUI (beta, alpha etc..) */
#define DEFAULT_OSC_PORT 9000

enum {	
    /* For the default VST GUI */
    k_yaw,
    k_pitch,
    k_roll,
    k_flipYaw,
    k_flipPitch,
    k_flipRoll,
    
	k_NumOfParameters
};

class PluginProcessor  : public AudioProcessor,
                         private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    int nNumInputs;                         /* current number of input channels */
	int nNumOutputs;                        /* current number of output channels */
	int nSampleRate;                        /* current host sample rate */
    int nHostBlockSize;                     /* typical host block size to expect, in samples */ 
    void* hRot;                             /* handle */

	bool isPlaying;
	AudioPlayHead* playHead;                /* Used to determine whether playback is currently occuring */
	AudioPlayHead::CurrentPositionInfo currentPosition;

	float** ringBufferInputs;
	float** ringBufferOutputs;
 
    int getCurrentBlockSize(){
        return nHostBlockSize;
    }
	
    OSCReceiver osc;
    void oscMessageReceived(const OSCMessage& message) override;
    int osc_port_ID;
    void setOscPortID(int newID){
        osc.disconnect();
        osc_port_ID = newID;
        osc.connect(osc_port_ID);
    }
    int getOscPortID(){
        return osc_port_ID;
    }

    /***************************************************************************\
                                    JUCE Functions
    \***************************************************************************/
public:
    PluginProcessor();
    ~PluginProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const String getName() const override;
    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;
    const String getParameterName (int index) override;
    const String getParameterText (int index) override;
    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair(int index) const override;
    void changeProgramName(int index, const String& newName) override;
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
