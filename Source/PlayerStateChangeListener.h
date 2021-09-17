#pragma once

#include <JuceHeader.h>
#include <iostream>

#include "MainComponent.h"

class PlayerStateChangeListener : public juce::ChangeListener
{
public:
  PlayerStateChangeListener(juce::AudioTransportSource *transportSource, MainComponent *parent)
  {
    std::cout << "Change listener created" << std::endl;
    this->transportSource = transportSource;
    this->parent = parent;
  }
  ~PlayerStateChangeListener()
  {
  }

  void changeListenerCallback(juce::ChangeBroadcaster *source)
  {
    if (source == transportSource)
    {
      if (transportSource->isPlaying())
        parent->changeState(Playing);
      else
        parent->changeState(Stopped);
    }
  }

private:
  juce::AudioTransportSource *transportSource;
  MainComponent *parent;
};
