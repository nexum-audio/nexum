#pragma once
#include "ExternalPresetImport.h"
namespace noctrl {
struct WavetableLibrary {
 struct Entry { juce::File file; juce::String pack; int frames=0; };
 struct ImportResult { int added=0,skipped=0; juce::String error; juce::File folder; };
 static juce::File root(){return ExternalPresetImport::defaultLibraryRoot().getParentDirectory().getChildFile("Wavetable Packs");}
 static int frames(const juce::File& file){
  if(!file.hasFileExtension("wav")||file.getSize()>8*1024*1024)return 0;
  juce::AudioFormatManager formats;formats.registerBasicFormats();std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
  return reader&&reader->lengthInSamples>=2048&&reader->lengthInSamples<=256*2048&&reader->lengthInSamples%2048==0?(int)reader->lengthInSamples/2048:0;
 }
 static std::vector<Entry> scan(juce::File base=root()){
  std::vector<Entry> result;auto folders=base.findChildFiles(juce::File::findDirectories,false);folders.sort();
  for(auto& folder:folders){if(folder.isSymbolicLink()||folder.getFileName().startsWithChar('.'))continue;auto files=folder.findChildFiles(juce::File::findFiles,true,"*.wav");files.sort();for(auto& file:files)if(!file.isSymbolicLink())if(int count=frames(file))result.push_back({file,folder.getFileName(),count});}return result;
 }
 static ImportResult importPack(const juce::File& source,juce::File base=root()){
  using namespace juce;ImportResult result;
  auto name=File::createLegalFileName(source.isDirectory()?source.getFileName():source.getFileNameWithoutExtension());if(name.isEmpty())name="Wavetables";
  auto stage=base.getChildFile(".import-"+Uuid().toString());if(stage.createDirectory().failed()){result.error="Could not create wavetable library.";return result;}
  juce::int64 copiedBytes=0;auto copy=[&](const File& file,String relative){int count=frames(file);if(!count){++result.skipped;return;}if(copiedBytes+file.getSize()>256*1024*1024){result.error="Pack exceeds 256 MB.";return;}copiedBytes+=file.getSize();auto destination=stage.getChildFile(relative);if(!destination.isAChildOf(stage)||destination.getParentDirectory().createDirectory().failed()||!file.copyFileTo(destination)){result.error="Could not copy wavetable: "+file.getFileName();return;}++result.added;};
  if(source.isDirectory()){auto files=source.findChildFiles(File::findFiles,true,"*.wav");if(files.size()>4096)result.error="Pack contains more than 4096 WAV files.";else for(auto& file:files){if(file.isSymbolicLink()){++result.skipped;continue;}copy(file,file.getRelativePathFrom(source));if(result.error.isNotEmpty())break;}}
  else if(source.hasFileExtension("zip")){
   ZipFile archive(source);if(archive.getNumEntries()>8192)result.error="Archive has too many entries.";int64 total=0;
   for(int i=0;result.error.isEmpty()&&i<archive.getNumEntries();++i){auto* entry=archive.getEntry(i);auto path=entry->filename.replaceCharacter('\\','/');if(!path.endsWithIgnoreCase(".wav"))continue;
    auto parts=StringArray::fromTokens(path,"/","");if(File::isAbsolutePath(path)||path.containsChar(':')||parts.contains("..")||entry->isSymbolicLink){result.error="Archive contains an invalid file path.";break;}
    if(entry->uncompressedSize<=0||entry->uncompressedSize>8*1024*1024||total+entry->uncompressedSize>256*1024*1024){result.error="Wavetable archive exceeds the import size limit.";break;}total+=entry->uncompressedSize;
    std::unique_ptr<InputStream> input(archive.createStreamForEntry(i));MemoryBlock bytes;if(!input||input->readIntoMemoryBlock(bytes,entry->uncompressedSize)!=entry->uncompressedSize){result.error="Could not read ZIP entry.";break;}
    auto file=stage.getChildFile(path);if(!file.isAChildOf(stage)||file.existsAsFile()||file.getParentDirectory().createDirectory().failed()||!file.replaceWithData(bytes.getData(),bytes.getSize())){result.error="Could not extract WAV file.";break;}
    if(frames(file))++result.added;else{file.deleteFile();++result.skipped;}
   }
  }else if(source.existsAsFile())copy(source,source.getFileName());
  if(result.added==0&&result.error.isEmpty())result.error="No compatible WAV wavetables found (2048 samples per frame, up to 256 frames).";
  if(result.error.isEmpty()){result.folder=base.getChildFile(name).getNonexistentSibling();if(!stage.moveFileTo(result.folder))result.error="Could not save wavetable pack.";}
  if(result.error.isNotEmpty()){stage.deleteRecursively();result.added=0;}return result;
 }
};

}
