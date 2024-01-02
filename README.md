# sm_signals
Linux signal callbacks for SourceMod

### Building under VS2022 and CMake

The build folder structure should be

| HL2SDK/  
| ...  
| sourcemod-sdk/  
|- sourcepawn/  
  |- core/  
  |- ...  
  |- public/  
    |- extensions/  
    |- amtl/  
    |- ...  
    |- __signals-extension/__  
      |- our files   
|- __CMakeLists.txt__  
|- __CMakePresets.json__  

