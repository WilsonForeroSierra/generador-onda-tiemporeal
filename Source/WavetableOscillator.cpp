/*
 ==============================================================================
 
 WavetableOscillator.cpp
 Created: 19 Feb 2019 11:37:53pm
 Author:  Wilson Forero
 
 ==============================================================================
 */

#include "WavetableOscillator.h"
WavetableOscillator::WavetableOscillator (OscillatorType _type)
:type(_type)
{}

void WavetableOscillator::setFrequency (float frequency, float sampleRate)
{
    auto tableSizeOverSampleRate = tableSize / sampleRate;
    tableDelta = frequency * tableSizeOverSampleRate;
}

void WavetableOscillator::setType(OscillatorType _type){
    type = _type;
    createAndGetWavetable();
}

float WavetableOscillator::getNextSample() noexcept
{
    
    auto tableSize = wavetable.getNumSamples();
    auto index0 = (unsigned int) currentIndex;
    auto index1 = index0 + 1;
    auto frac = currentIndex - (float) index0;
    auto* table = wavetable.getReadPointer (0);
    // Tomar dos valores (actual y siguiente)
    auto value0 = table[index0];
    auto value1 = table[index1];
    // Interpolacion
    auto currentSample = value0 + frac * (value1 - value0);
    // Aumentar el indice segun el delta y luego si ya fueron tomados todos los datos es necesario volver a iniciar
    if ((currentIndex += tableDelta) > tableSize)
        currentIndex -= tableSize;
    // Retorna una muestra
    return currentSample;
}

// Para crear la tabla dependiendo del tipo de generador almecenado en type
AudioSampleBuffer WavetableOscillator::createAndGetWavetable()
{
    // Tamaño tomado del ejemplo de la pagina oficial
    int tableSize1 = 128;
    
    // Tabla de un canal y el tamaño anterior + 1 de valores
    wavetable.setSize (1, tableSize1 + 1);
    wavetable.clear();
    auto* samples = wavetable.getWritePointer (0);
    
    // Retorna el mismo numero de tableSize1 + 1 pero es solo para comprobar
    tableSize = wavetable.getNumSamples();
    jassert(tableSize1 + 1 == tableSize);
    
    // Sin armonicos
    
    //Se toma el paso entre angulos
    auto angleDelta = MathConstants<double>::twoPi / (double) (tableSize1 - 1);
    auto currentAngle = 0.0;
    
    /*
     Operaciones necesarias para hacer la onda triangular
     puntualmente para dividirla en partes
    */
    int over4 = tableSize / 4, div = tableSize * 3 / 4;
    
    // For para generar la tabla
    for (auto i = 0; i < tableSize1; ++i)
    {
        float sample = 0.0f;
        if (type == TRIANGLE) {
            if (i < over4)
                sample = i / tableSize;
            else if (i < div)
                sample = 2 + (-4*i/tableSize);
            else
                sample = -4 + (4 * i / tableSize);
            sample *= 0.5f;
        } else if (type == SAWTOOTH)
            sample = -1.0 + (2 * i / tableSize);
        else if (type == SQUARE)
            sample = std::sin (currentAngle) > 0 ? 1 : -1;
        else if (type == SIN)
            sample = std::sin (currentAngle);
        
        // Almacenar el sample en la tabla
        samples[i] = (float) sample;
        // Avanzar al siguiente angulo
        currentAngle += angleDelta;
    }
    // El ultimo valor es el mismo del inico (por esto el size + 1)
    samples[tableSize1] = samples[0];
    // Retorna la tabla
    return wavetable;
}
