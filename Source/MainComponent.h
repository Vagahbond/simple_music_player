#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
{
public:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

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

    void changeState(TransportState newState);

    void changeListenerCallback(juce::ChangeBroadcaster *source);

    // Buttons
    juce::TextButton *play_button;
    juce::TextButton *stop_button;
    juce::TextButton *open_button;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
