/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "WavetableOscillator.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void updateAmplitude();
    void updateFrequency();
    void updateGeneratorsType(OscillatorType type);
    void waveMenuChanged();
    void mute();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    // Declaracion de los labels de cpu y demas
    Label cpuUsageLabel, cpuUsageText, frequencyLabel, amplitudeLabel, waveformLabel;
    // Declaracion de sliders
    Slider frequencySlider, amplitudeSlider;
    // Menu
    ComboBox waveMenu;
    // Tipo de generador inicializado en Seno
    OscillatorType currentType = SIN;
    // Inicializacion de valores
    float level = 0.0f, currentSampleRate = 0.0f;
    // Arreglo propio de Juce que contendrá objetos de tipo WavetableOscillator
    OwnedArray<WavetableOscillator> oscillators;
    TextButton muteButton;
    bool isPlaying = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
