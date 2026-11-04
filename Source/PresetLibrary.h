#pragma once
#include "Processor.h"
namespace noctrl {
struct PresetMetadata {
 juce::String name,author,category,description,notes,tags;int rating=0;
 juce::var toJson()const;static PresetMetadata fromJson(const juce::var&);
};
struct LibraryPack {juce::String id,name,author,description;juce::File folder;bool writable=false,factory=false;int count=0;};
struct LibraryPreset {juce::String id,packId,packName;juce::File file;PresetMetadata metadata;juce::Time modified;int program=-1;bool writable=false;};
class PresetLibrary {
 juce::File importedRoot,userRoot,factoryRoot,metadataRoot;
 juce::File metadataFile(const LibraryPreset&)const;
 static PresetMetadata readMetadata(const juce::File&);
 static juce::Result atomicWrite(const juce::File&,const juce::MemoryBlock&);
public:
 std::vector<LibraryPack> packs;std::vector<LibraryPreset> presets;juce::StringArray categories;
 explicit PresetLibrary(juce::File base={});
 void refresh();
 juce::Result removePack(const juce::String& id);
 bool canManagePreset(const juce::String& id)const;
 bool canMovePreset(const juce::String& id,const juce::String& targetPack)const;
 juce::Result movePreset(const juce::String& id,const juce::String& targetPack,juce::File& moved,juce::String newName={});
 juce::Result renamePack(const juce::String&,juce::String,juce::String&);
 juce::Result editCategory(juce::String,juce::String);
 juce::Result deletePreset(const juce::String& id);
 juce::Result createPack(juce::String name,juce::String author,juce::String description,juce::String& id);
 juce::Result savePreset(const juce::String& packId,const PresetMetadata&,const juce::MemoryBlock& state,juce::File& saved);
 juce::Result overwritePreset(const juce::File&,const PresetMetadata&,const juce::MemoryBlock&);
 static juce::File currentPresetFile(SynthProcessor&);
 static bool canOverwriteCurrent(SynthProcessor&);
 juce::Result updateMetadata(const LibraryPreset&,const PresetMetadata&);
 const LibraryPack* findPack(const juce::String&)const;
 static juce::String inferCategory(juce::String name);
};
}
