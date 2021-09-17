#include "MainComponent.h"
#include "PlayerStateChangeListener.h"

#include "portable-file-dialogs.h"

#if _WIN32
#define DEFAULT_PATH "C:\\"
#else
#define DEFAULT_PATH "/home"
#endif

//==============================================================================
MainComponent::MainComponent()
{

    open_button = new juce::TextButton(std::string("blblbl"));
    play_button = new juce::TextButton();
    stop_button = new juce::TextButton();

    addAndMakeVisible(open_button);
    open_button->setButtonText("Open...");
    open_button->onClick = [this]
    { this->open_button_clicked(); };
    open_button->setBounds(10, 10, 780, 100);

    addAndMakeVisible(play_button);
    play_button->setButtonText("Play");
    play_button->onClick = [this]
    { this->play_button_clicked(); };
    play_button->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    play_button->setEnabled(false);
    play_button->setBounds(10, 120, 780, 100);

    addAndMakeVisible(stop_button);
    stop_button->setButtonText("Stop");
    stop_button->onClick = [this]
    { this->stop_button_clicked(); };
    stop_button->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stop_button->setEnabled(false);
    stop_button->setBounds(10, 230, 780, 100);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize(800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio) && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                                          [&](bool granted)
                                          { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
    }

    formatManager.registerBasicFormats();

    // this->change_listener = &new PlayerStateChangeListener();
    transportSource.addChangeListener(new PlayerStateChangeListener(&this->transportSource, this));
}

MainComponent::~MainComponent()
{
    // delete buttons
    deleteAllChildren();
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

// //callback called to stop playing
// void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source)
// {
//     std::cout << "went in callback" << std::endl;
//     if (source == &transportSource)
//     {
//         if (transportSource.isPlaying())
//             changeState(Playing);
//         else
//             changeState(Stopped);
//     }
// }

void MainComponent::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Stopped:
            stop_button->setEnabled(false);
            play_button->setEnabled(true);
            transportSource.setPosition(0.0);
            break;

        case Starting:
            play_button->setEnabled(false);
            transportSource.start();
            break;

        case Playing:
            stop_button->setEnabled(true);
            break;

        case Stopping:
            transportSource.stop();
            break;
        }
    }
}

void MainComponent::open_button_clicked()
{
    if (!pfd::settings::available())
    {
        std::cout << "Portable File Dialogs are not available on this platform.\n";
        return;
    }

    std::cout << "preparing to launch file chooser" << std::endl;

    auto f = pfd::open_file("Choose file to read", DEFAULT_PATH,
                            {"Audio Files (.wav .mp3)", "*.mp3 *.wav",
                             "All Files", "*"});
    std::cout << "Selected files:";
    std::cout << f.result()[0] << std::endl;

    if (f.result().size() == 0) {
        std::cout << "No file was selected" << std::endl;
        return;
    }

    auto selected_file = f.result()[0];

    auto file = juce::File(selected_file);

    if (file != juce::File{})
    {
        auto *reader = formatManager.createReaderFor(file);

        if (reader != nullptr)
        {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            play_button->setEnabled(true);
            readerSource.reset(newSource.release());
        }
    }
}

void MainComponent::play_button_clicked()
{
    changeState(Starting);
}

void MainComponent::stop_button_clicked()
{
    changeState(Stopping);
}
