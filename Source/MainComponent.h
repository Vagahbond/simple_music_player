#pragma once

#include <JuceHeader.h>
// #include "PlayerStateChangeListener.h"

enum TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    void changeState(TransportState newState);

    //==============================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================

private:
    //==============================================================================
    // Your private member variables go here...

    void open_button_clicked();

    void play_button_clicked();

    void stop_button_clicked();

    // Buttons
    juce::TextButton *play_button;
    juce::TextButton *stop_button;
    juce::TextButton *open_button;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;
    // std::unique_ptr<PlayerStateChangeListener> change_listener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
