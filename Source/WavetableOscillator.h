
#include "../JuceLibraryCode/JuceHeader.h"
#pragma once

// Enumerado para definir opciones de tipo de generador
enum OscillatorType {
    SIN, TRIANGLE, SAWTOOTH, SQUARE
};

// Declaracion de clase
class WavetableOscillator
{
public:
    // Constructor de la clase
    WavetableOscillator (OscillatorType _type);
    // Se actualiza la frecuencia acorde a los paramentros de entrada y las caracteristicas de la tabla
    void setFrequency (float frequency, float sampleRate);
    // Toma el siguiente punto de la tabla
    float getNextSample() noexcept;
    // Al ejecutar este metodo se actualiza el tipo de onda y por ende la wavetable
    void setType(OscillatorType _type);
    // Crea la tabla y la retorna en caso de necesitarla
    AudioSampleBuffer createAndGetWavetable();
    
private:
    // Instancia de AudioSampleBuffer que sera la tabla de onda (cada oscilador tiene su tabla)
    AudioSampleBuffer wavetable;
    int tableSize;
    // Tipo de onda
    OscillatorType type;
    // Inicializacion de valores
    float currentIndex = 0.0f, tableDelta = 0.0f;
};
