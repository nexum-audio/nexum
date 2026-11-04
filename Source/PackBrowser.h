#pragma once
#include <NexumDesign.h>
#include "PresetLibrary.h"
#include "LibraryUi.h"
namespace noctrl {
class PackBrowser:public juce::Component,private juce::TableListBoxModel,public juce::DragAndDropContainer {
 struct PackList:juce::ListBoxModel {PackBrowser& owner;explicit PackList(PackBrowser& o):owner(o){}int getNumRows()override;void paintListBoxItem(int,juce::Graphics&,int,int,bool)override;void selectedRowsChanged(int)override;};
 struct PackDropList:juce::ListBox,juce::DragAndDropTarget {PackBrowser& owner;explicit PackDropList(PackBrowser&,juce::ListBoxModel*);bool isInterestedInDragSource(const SourceDetails&)override;void itemDragEnter(const SourceDetails&)override;void itemDragMove(const SourceDetails&)override;void itemDragExit(const SourceDetails&)override;void itemDropped(const SourceDetails&)override;};
 LibraryTheme theme;juce::Array<juce::Component::SafePointer<juce::AlertWindow>> dialogs;SynthProcessor& processor;PresetLibrary library;PackList packModel;PackDropList packList;juce::TableListBox table;
 juce::TextEditor search,name,author,description,notes,tags;juce::ComboBox categoryFilter,category,destination,sortMode;
 juce::TextButton removePackButton{"REMOVE PACK"},createPackButton{"NEW PACK"},saveCurrentButton{"SAVE CURRENT"},importButton{"IMPORT PACK"},refreshButton{"REFRESH"},commitButton{"SAVE DETAILS"},loadButton{"LOAD PRESET"};juce::TextButton categoryButton{"CATEGORIES"},renamePackButton{"FOLDER ACTIONS"},renameEntryButton{"RENAME"},moveButton{"MOVE"};juce::TextButton deleteButton{"DELETE PRESET"};juce::TextButton previewButton{"PREVIEW"},loadFileButton{"LOAD FILE"},assetsButton{"ASSET FOLDER"},infoButton{"IMPORT INFO"};std::array<juce::TextButton,5> stars;
 std::vector<size_t> visibleRows;juce::String packFilter="all",selectedId,status;int rating=0,sortColumn=1;bool ascending=true,saving=false;int dropRow=-1;
 int getNumRows()override;void paintRowBackground(juce::Graphics&,int,int,int,bool)override;void paintCell(juce::Graphics&,int,int,int,int,bool)override;void selectedRowsChanged(int)override;void cellDoubleClicked(int,int,const juce::MouseEvent&)override;void returnKeyPressed(int)override;void sortOrderChanged(int,bool)override;
 juce::var getDragSourceDescription(const juce::SparseSet<int>&)override;void deleteKeyPressed(int)override;void cellClicked(int,int,const juce::MouseEvent&)override;
 void deleteSelected();void moveSelectedTo(const juce::String&,const juce::String&,juce::String newName={});void presetMenu(int);int dropDestination(const juce::DragAndDropTarget::SourceDetails&)const;
 void promptName(juce::String,juce::String,std::function<void(juce::String)>);void categoryMenu();void packActions();void quickMetadata(juce::String,int);void setDetailsEnabled(bool);
 void filterRows();void selectPack(int);void selectRow(int);void fillDetails(const PresetMetadata&);PresetMetadata details()const;const LibraryPreset* selected()const;void commit();void loadSelected();void newPack();void removePack();void setStatus(juce::String,bool error=false);void updateStars();void rebuildDestinations(juce::String preferred={});
public:
 explicit PackBrowser(SynthProcessor&,juce::File base={});~PackBrowser()override;
 std::function<void()> onImportRequested,onLibraryChanged,onLoadFileRequested,onAssetsRequested,onInfoRequested;
 void refresh();void showSaveCurrent();juce::Result overwriteCurrent();
 void paint(juce::Graphics&)override;void paintOverChildren(juce::Graphics&)override;void resized()override;
};
}
