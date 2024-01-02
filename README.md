# sm_signals
Linux signal callbacks for SourceMod

### Building under VS2022 and CMake

The build folder structure should be

| HL2SDK/  
| ...  
| sourcemod-sdk/  
&nbsp;&nbsp;|- sourcepawn/  
&nbsp;&nbsp;|- core/  
  &nbsp;&nbsp;&nbsp;&nbsp;|- ...  
  &nbsp;&nbsp;&nbsp;&nbsp;|- public/  
    &nbsp;&nbsp;&nbsp;&nbsp;|- extensions/  
    &nbsp;&nbsp;&nbsp;&nbsp;|- amtl/  
    &nbsp;&nbsp;&nbsp;&nbsp;|- ...  
    &nbsp;&nbsp;&nbsp;&nbsp;|- __signals-extension/__  
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|- our files   
|- __CMakeLists.txt__  
|- __CMakePresets.json__  

