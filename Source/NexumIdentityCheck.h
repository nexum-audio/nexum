#pragma once
static int checkSynthIdentity(const juce::String&,const juce::String& synthPath,const juce::String& fxPath){
 using namespace juce;VST3PluginFormat format;OwnedArray<PluginDescription> synth,fx;
 format.findAllTypesForFile(synth,synthPath);format.findAllTypesForFile(fx,fxPath);
 if(synth.size()!=1||fx.size()!=1||synth[0]->name!="Nexum"||fx[0]->name!="Nexum FX"||!synth[0]->isInstrument||fx[0]->isInstrument||synth[0]->uniqueId==fx[0]->uniqueId)return 1;
 String error;auto plugin=format.createInstanceFromDescription(*synth[0],48000,512,error);if(!plugin)return 2;
 MemoryBlock state;plugin->getStateInformation(state);plugin->setStateInformation(state.getData(),(int)state.getSize());plugin->prepareToPlay(48000,512);
 AudioBuffer<float> audio(2,512);MidiBuffer midi;midi.addEvent(MidiMessage::noteOn(1,60,(uint8)100),0);float peak=0;
 for(int i=0;i<16;++i){audio.clear();plugin->processBlock(audio,midi);midi.clear();peak=jmax(peak,audio.getMagnitude(0,512));}plugin->releaseResources();
 if(!std::isfinite(peak)||peak<.0001f)return 3;std::unique_ptr<AudioProcessorEditor> editor(plugin->createEditor());if(!editor)return 4;
 std::cout<<"PASS: Nexum and Nexum FX identities, state recall, audible synthesis and editor.\n";return 0;
}
