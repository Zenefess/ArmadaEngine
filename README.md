2024/06/06 update:

   Input processing is complete. Input mappings for triggering functions can be any combination of all 512 binary inputs (keys, mouse buttons, and controller buttons & POV directions) and, optionally, any combination of scalar inputs (mouse and game controller axes). Work on custom function handling is still underway, being worked on in parallel with the rest of the GUI system. Game controller axes support deadzone, sensitivity, and center offset adjmustments. Only 1 mouse and 1 keyboard are supported, currently; this may change in the future.
   
   The following is supported:
   
* Mouse: Wheel tilt, and up to 8 buttons
* Keyboard: Up to 192 unique keys
* Game controllers: Up to 8 devices, each of which allows for up to 8 axes, 4 POVs, and 32 buttons




2024/06/01 update:

   I have spendt most of the time since the last update rewriting fixed-point data type handling, which has included revising a lot of code that utilises those data types. Much easier to use them, now, as their conversions are handled by class functions. Work on this project is still well in progress.

   

2024/05/06 update:

   Currently preparing GUI backend to make creating/editing GUI profiles fully customisable. Support for custom input processing & triggered functions is almost complete.



2024/04/07 update:

   Most of the GUI functionality is done, now. Soon enough I'll start creating a GUI editor.   ...Creating a GUI system from scratch is my biggest irritation when it comes to software development, I have discovered. I will be relieved when it's completed; then I can go back to working on terrain generation, as well as begin work on the AI system.
   At some point I'll decide on a license agreement that allows for attributed use of my code. Right now I'm not sure which elements will be core to the ArmadaEngine and which will be specific to future commerical products. Most will become core elements.
   On a related subject: My brain's progress is holding. Even with a massive 8+ months break due to my dominant arm needing more physiotherapy to stop my hand going numb.




Original text:

   This is a personal project I've been working on in isolation, primarily to rescue myself from chronic symptoms of stage 4 dementia. Back in early 2022 I had gone way beyond failing to remember names on a regular basis; I had reached a point where I was forgetting how to begin complex tasks I'd been doing most of my life. Then I found an old project I had put on freeze indefinitely for other priorities (and I understood most of what I was looking at), so I became determined to use it as a way to bring my brain back.
   And it's working; 4 months of learning & coding with DirectX 11 was enough to restore my memory recall to even better than it was 10 years ago. But events since beginning ArmadaEngine have slowed me down -- including permanent arm & leg injuries -- and I need to keep going.


   Originally this project started way back in the late '90s. At the time I wanted to create a tile-based game engine for making Dungeons & Dragons adventures. By 2001 the idea had evolved into tile-based terrain with arbitrarily positioned entities. Eventually -- when I was thinking about a new career in the mid-to-late '00s -- I started developing "TankEngine"; the precursor to ArmadaEngine. TankEngine's purpose was to allow easy construction of, both, tile-based and freeform play, ranging from turn-based RPGs to real-time shoot-'em-ups.
   But things happened and I put the project away, viewing it as a pleasant little pipe dream. Now, though, I have good reason to resurrect it.


As for the current incarnation of all this, ArmadaEngine expands upon TankEngine's design in the following ways:
* Hybrid voxel/heightfield 3D terrain with real-time deformation, replacing flat 2D tiles
* Multi-threaded & cache-compact modular A.I., replacing library-based fixed functions
* Modular terrain map with parallel worlds/planets/dimensions/universes/etc. access, replacing isolated closed terrain maps
* Definable chemical elements with environment-based transitions & interactions, replacing hard-coded assignable tile properties


   I'm still using DirectInput 8 and OpenAL 1.1; obvious signs of me being behind the times. And we all know Direct3D 11 isn't young, either. I intend to (eventually) create a Vulkan pathway in ArmadaEngine, as well as move on to OpenAL Soft. As for DirectInput 8, I will have to create a replacement one day, I suppose. One thing a time...


Testing on-the-fly geometry generation, entity culling, and GUI rendering & function:
![screenshot0](https://github.com/Zenefess/ArmadaEngine/assets/116688139/72a9ef8c-765e-40f9-8bc9-068725c3d8c2)
![screenshot1](https://github.com/Zenefess/ArmadaEngine/assets/116688139/2cbfe5d4-4431-4f79-aea7-2a3b65a29807)
