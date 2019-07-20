/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Declaración de etiquetas para uso de CPU
    cpuUsageLabel.setText ("CPU Usage", dontSendNotification);
    cpuUsageText.setJustificationType (Justification::right);
    waveformLabel.setText("Tipo de Onda", dontSendNotification);
    amplitudeLabel.setText("Amplitud", dontSendNotification);
    frequencyLabel.setText("Frecuencia", dontSendNotification);
    waveformLabel.setJustificationType(Justification::centred);
    addAndMakeVisible (cpuUsageLabel);
    addAndMakeVisible (cpuUsageText);
    addAndMakeVisible (waveformLabel);
    addAndMakeVisible (frequencyLabel);
    addAndMakeVisible (amplitudeLabel);
    
    // Slider de frecuencia
    addAndMakeVisible (frequencySlider);
    frequencyLabel.attachToComponent (&frequencySlider, true);
    frequencySlider.setRange (50, 5000);
    frequencySlider.setTextValueSuffix (" Hz");
    frequencySlider.setSkewFactorFromMidPoint (500); // [4]
    frequencySlider.onValueChange = [this]
    {
        // Llamada al metodo de actualizar frecuencia
        updateFrequency();
    };
    
    // Slider para nivel
    addAndMakeVisible (amplitudeSlider);
    amplitudeLabel.attachToComponent(&amplitudeSlider, true);
    amplitudeSlider.setRange (0.0, 0.25);
    amplitudeSlider.setSkewFactorFromMidPoint (0.12); // [4]
    amplitudeSlider.onValueChange = [this]
    {
        // Llamada al metodo de actualizar nivel
        updateAmplitude();
    };
    
    // Creacion del menu
    addAndMakeVisible (waveMenu);
    waveMenu.addItem ("Seno", 1);
    waveMenu.addItem ("Cuadrada", 2);
    waveMenu.addItem ("Triangular", 3);
    waveMenu.addItem ("Diente de Sierra", 4);
    waveMenu.onChange = [this] { waveMenuChanged(); };
    waveMenu.setSelectedId (1);
    
    // Button
    addAndMakeVisible (muteButton);
    muteButton.setButtonText ("Pause");
    muteButton.onClick = [this] { mute(); };
    
    // Tamaño de la ventana inicial
    setSize (1000, 250);
    setAudioChannels (0, 2); // no inputs, two outputs
    startTimer (50);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}


//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    auto numberOfOscillators = 2;
    auto frequency = 1000;
    for (auto i = 0; i < numberOfOscillators; ++i)
    {
        // Creacion de una instancia de un WavetableOscillator con tipo de onda Seno
        auto* oscillator = new WavetableOscillator (SIN);
        // Frecuencia inicial
        // Genearacion de la tabla para ese objeto
        oscillator->createAndGetWavetable();
        // Frecuencia de reproduccion para el objeto
        oscillator->setFrequency ((float) frequency, sampleRate);
        // Agregar a la lista de osciladores
        oscillators.add (oscillator);
    }
    frequencySlider.setValue(frequency);
    // Distribucion del nivel maximo (0.25f) entre los osciladores y seteo inicial a la mitad del valor maximo
    level = (0.25f / numberOfOscillators) / 2;
    amplitudeSlider.setValue(level);
    // Actualizacion de la variable para frecuencia de muestreo actual
    currentSampleRate = sampleRate;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Canal Izquierdo
    auto* leftBuffer  = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    // Canal Derecho
    auto* rightBuffer = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
    // Limpiar info presente en el buffer
    bufferToFill.clearActiveBufferRegion();
    if (isPlaying) {
        // Iteracion sobre la lista oscillators
        for (auto oscillatorIndex = 0; oscillatorIndex < oscillators.size(); ++oscillatorIndex)
        {
            // Tomar un oscilador
            auto* oscillator = oscillators.getUnchecked (oscillatorIndex);
            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                // Actualizacion del valor de la muestra segun el nivel al que se encuentre (elegido por el usuario
                auto levelSample = oscillator->getNextSample() * level;
                // Se guarda la misma informacion en cada canal
                leftBuffer[sample]  += levelSample;
                rightBuffer[sample] += levelSample;
            }
        }
    }
}

void MainComponent::releaseResources() {}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    auto sliderLeft = 80;
    double height = 130, menuH = 40, heightAdjust = 60;
    // Ajustar para que el menu se centre en ventanas mayores a 400px
    if (getHeight() >= 400)
    {
        height = getHeight() / 2;
        menuH = 60;
        heightAdjust = 80;
    }
    frequencySlider.setBounds (sliderLeft + 10, 50, getWidth() - sliderLeft - 20, 20);
    amplitudeSlider.setBounds (sliderLeft + 10, 90, getWidth() -  sliderLeft - 20 ,20);
    cpuUsageLabel.setBounds(10, 10, getWidth() - 20, 20);
    cpuUsageText.setBounds (10, 10, getWidth() - 20, 20);
    waveformLabel.setBounds(10, height - 20, getWidth() - 20, 20);
    waveMenu.setBounds (10, height, getWidth() - 20, menuH);
    muteButton.setBounds(10, height + heightAdjust, getWidth() - 20, 34);
}

//==============================================================================
void MainComponent::timerCallback()
{
    auto cpu = deviceManager.getCpuUsage() * 100;
    cpuUsageText.setText (String (cpu, 6) + " %", dontSendNotification);
}


//==============================================================================

// Actualizar frecuencia
void MainComponent::updateFrequency()
{
    // Actualiza para todos los osciladores
    for (auto* oscillator : oscillators) {
        oscillator->setFrequency(frequencySlider.getValue(), currentSampleRate);
    }
}

// Actualiza nivel del generador
void MainComponent::updateAmplitude() {
    level = amplitudeSlider.getValue();
}

// Actualiza el tipo de onda que se genera
void MainComponent::updateGeneratorsType(OscillatorType type) {
    // Itera sobre la lista de osciladores
    for (auto* generator : oscillators) {
        // Se hace la comparacion para evitar calculos innecesarios
        if (currentType != type) {
            generator->setType(type);
            // Se actualiza el tipo global
            currentType = type;
        }
    }
}

// Actualizacion del menu
void MainComponent::waveMenuChanged()
{
    OscillatorType type;
    switch (waveMenu.getSelectedId())
    {
        case 1: type = SIN; break;
        case 2: type = SQUARE; break;
        case 3: type = TRIANGLE; break;
        case 4: type = SAWTOOTH; break;
            // En caso de un fallo
        default: type= SIN; break;
    }
    // Llamada al metodo
    updateGeneratorsType(type);
}

// Mute button
void MainComponent::mute()
{
    if (isPlaying)
    {
        isPlaying = false;
        muteButton.setButtonText("Play");
    } else {
        isPlaying = true;
        muteButton.setButtonText("Pause");
    }
}

