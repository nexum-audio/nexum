#pragma once
static juce::String checkTrancegate(){
 using namespace juce;using namespace noctrl;
 struct Head:AudioPlayHead{double beat=0,bpm=120;Optional<PositionInfo> getPosition()const override{PositionInfo info;info.setBpm(bpm);info.setPpqPosition(beat);info.setIsPlaying(true);return info;}} head;
 auto owner=std::make_unique<NexumFxProcessor>();auto& p=*owner;p.prepareToPlay(48000,512);int slot=p.addFx(0,16);if(slot<0)return "Trancegate registry";
 auto control=[&](int i,float value){p.set(fxId(slot,"p"+String(i)),fxDefinitions()[16].controls[i].to(value));};
 control(2,100);control(3,.1f);control(4,.1f);p.setPlayHead(&head);
 auto render=[&](int chunk,double bpm=120.){head.bpm=bpm;std::atomic_load(&p.fxRuntime)->reset();AudioBuffer<float> result(2,24000);MidiBuffer midi;
  for(int start=0;start<24000;start+=chunk){int n=jmin(chunk,24000-start);AudioBuffer<float> block(2,n);for(int c=0;c<2;++c)FloatVectorOperations::fill(block.getWritePointer(c),1.f,n);head.beat=start/48000.*bpm/60.;p.processBlock(block,midi);for(int c=0;c<2;++c)result.copyFrom(c,start,block,c,0,n);}return result;};
 auto a=render(512),b=render(101);for(int c=0;c<2;++c)for(int i=0;i<24000;++i)if(std::abs(a.getSample(c,i)-b.getSample(c,i))>1e-5f)return "Trancegate block continuity / PPQ alignment";
 if(a.getSample(0,3000)<.999f||a.getSample(0,9000)>.001f||a.getSample(0,15000)<.999f)return "Trancegate rhythmic gate timing";
 auto slower=render(512,60);if(slower.getSample(0,9000)<.999f||slower.getSample(0,15000)>.001f)return "Trancegate host tempo";
 control(5,1);auto stereo=render(512);if(stereo.getSample(0,3000)<.999f||stereo.getSample(1,3000)>.001f||stereo.getSample(1,9000)<.999f)return "Trancegate stereo step offset";control(5,0);
 control(1,0);auto neutral=render(512);for(int i=0;i<24000;++i)if(neutral.getSample(0,i)!=1.f)return "Trancegate depth zero";control(1,100);
 p.set(fxId(slot,"wet"),.25f);auto mixed=render(512);if(std::abs(mixed.getSample(0,9000)-.75f)>1e-5f)return "Trancegate dry wet";p.set(fxId(slot,"wet"),1);
 control(2,50);auto shortGate=render(512);if(shortGate.getSample(0,4500)>.001f)return "Trancegate gate length";control(2,100);
 p.set(fxId(slot,"enabled"),0);auto bypass=render(512);if(bypass.getSample(0,9000)!=1.f)return "Trancegate bypass";p.set(fxId(slot,"enabled"),1);
 control(3,20);auto smooth=render(512);if(smooth.getSample(0,0)>.01f||smooth.getSample(0,500)<.2f||smooth.getSample(0,500)>.7f)return "Trancegate click smoothing";control(3,.1f);
 // Seek directly into a closed step; it must follow the host rather than continue locally.
 head.beat=.25;head.bpm=120;AudioBuffer<float> seek(2,512);for(int c=0;c<2;++c)FloatVectorOperations::fill(seek.getWritePointer(c),1.f,512);MidiBuffer midi;p.processBlock(seek,midi);if(seek.getSample(0,511)>.001f)return "Trancegate host seek";
 p.set(fxId(slot,"gate3"),.37f);MemoryBlock state;p.getStateInformation(state);auto recall=std::make_unique<NexumFxProcessor>();recall->setStateInformation(state.getData(),(int)state.getSize());if(std::abs(recall->value(fxId(slot,"gate3"))-.37f)>1e-6f)return "Trancegate project recall";
 auto synth=std::make_unique<SynthProcessor>();if(FxLibrary::apply(*synth,FxLibrary::capture(p,0),2).failed())return "Trancegate shared library apply";auto loaded=synth->rack()->entries.back();if(loaded.type!=16||std::abs(synth->value(fxId(loaded.slot,"gate3"))-.37f)>1e-6f)return "Trancegate shared pattern recall";
 p.setPlayHead(nullptr);FxRow row(p,p.rack()->entries[0]);row.setSize(934,FxRow::preferredHeight(16));FxGraph* graph=nullptr;for(auto* child:row.getChildren())if(auto* candidate=dynamic_cast<FxGraph*>(child))graph=candidate;if(!graph)return "Trancegate step editor missing";
 auto source=Desktop::getInstance().getMainMouseSource();ModifierKeys left(ModifierKeys::leftButtonModifier);MouseEvent event(source,{20,45},left,0,0,0,0,0,graph,graph,Time::getCurrentTime(),{20,45},Time::getCurrentTime(),1,false);graph->mouseDown(event);graph->mouseUp(event);if(p.value(fxId(slot,"gate0"))<=.5f||p.value(fxId(slot,"gate0"))>=.99f)return "Trancegate editable steps";
 std::unique_ptr<AudioProcessorEditor> editor(p.createEditor());FileOutputStream out(File::getCurrentWorkingDirectory().getChildFile("Reports/Nexum-Trancegate.png"));PNGImageFormat png;out.setPosition(0);out.truncate();png.writeImageToStream(editor->createComponentSnapshot(editor->getLocalBounds()),out);
 std::cout<<"PASS: Trancegate timing, tempo, seek, block continuity, stereo, depth, gate length, smoothing, mix/bypass, project/shared-library recall and step editor.\n";return {};
}
