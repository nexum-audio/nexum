#pragma once
#include "Processor.h"
#include <map>
namespace noctrl {
struct ImportedTable {
 int frameCount=0;std::vector<float> data;juce::MemoryBlock originalWav;juce::String name;
 float sample(float position,double phase,double frequency,double sr)const noexcept;
 static std::shared_ptr<ImportedTable> fromWav(const juce::MemoryBlock&,juce::String name,juce::String& error);
};
struct Expansion;
struct ImportedAssets {
 std::array<std::shared_ptr<const ImportedTable>,3> tables;
 std::array<std::array<float,256>,2> lfo{};std::array<bool,2> customLfo{};
 std::array<float,3> tune{},pan{},initialPhase{},randomPhase{1,1,1};std::array<bool,3> enabled{true,true,true};float wheel=0;
 std::shared_ptr<const Expansion> expansion;
 bool userSource=false;float subOctave=-1;juce::String name,author,description,sourcePath;juce::StringArray warnings;
};
struct ImportResult {
 bool ok=false;juce::String error,name;juce::StringArray warnings;
 std::map<juce::String,float> parameters;std::shared_ptr<ImportedAssets> assets;
};
struct PackResult {bool ok=false;int presets=0,assets=0;juce::File root,report;juce::String error;juce::StringArray names;};
class ExternalPresetImport {
public:
 static ImportResult readPreset(const juce::File&,const juce::File& assetRoot);
 static PackResult importPack(const juce::File&,const juce::File& libraryRoot);
 static juce::File defaultLibraryRoot();
 static juce::var decodePreset(const juce::MemoryBlock&,juce::String& error);
};
}



