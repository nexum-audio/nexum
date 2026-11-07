#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>
#include "NexumStandaloneStartup.h"

// JUCE's plugin wrapper supplies the entry point. This family launcher controls
// only automatic startup; the processors retain their normal host-state loading.
class NexumStandaloneApp final : public juce::JUCEApplication {
    juce::ApplicationProperties properties;
    std::unique_ptr<juce::StandaloneFilterWindow> window;
public:
    NexumStandaloneApp() {
        juce::PropertiesFile::Options options;
        options.applicationName=juce::CharPointer_UTF8(JucePlugin_Name);
        options.filenameSuffix=".settings";
        options.osxLibrarySubFolder="Application Support";
       #if JUCE_LINUX || JUCE_BSD
        options.folderName="~/.config";
       #endif
        properties.setStorageParameters(options);
    }
    const juce::String getApplicationName()override{return juce::CharPointer_UTF8(JucePlugin_Name);}
    const juce::String getApplicationVersion()override{return JucePlugin_VersionString;}
    bool moreThanOneInstanceAllowed()override{return true;}
    void anotherInstanceStarted(const juce::String&)override{}
    void initialise(const juce::String&)override {
        auto* settings=properties.getUserSettings();
        nexum::family::prepareInitStartup(*settings);
        settings->saveIfNeeded();
        auto holder=std::make_unique<juce::StandalonePluginHolder>(settings,false);
        window=std::make_unique<juce::StandaloneFilterWindow>(getApplicationName(),
            juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
            std::move(holder));
        window->setVisible(true);
    }
    void systemRequestedQuit()override {
        if(window)window->pluginHolder->savePluginState();
        if(juce::ModalComponentManager::getInstance()->cancelAllModalComponents()) {
            juce::Timer::callAfterDelay(100,[]{if(auto* app=juce::JUCEApplicationBase::getInstance())app->systemRequestedQuit();});
        } else quit();
    }
    void shutdown()override {
        window=nullptr;
        properties.saveIfNeeded();
    }
};
JUCE_CREATE_APPLICATION_DEFINE(NexumStandaloneApp)
