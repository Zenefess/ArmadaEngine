This is a personal project I've been working on in isolation, primarily to rescue myself from chronic symptoms of stage 4 dementia. Back in early 2022 I had gone way beyond failing to remember names on a regular basis; I had reached a point where I was forgetting how to begin complex tasks I'd been doing most of my life. Then I found an old project I had put on freeze indefinitely for other priorities (and I understood most of what I was looking at), so I became determined to use it as a way to bring my brain back.
And it's working; 4 months of learning & coding with DirectX 11 was enough to restore my memory recall to even better than it was 10 years ago. But events since beginning ArmadaEngine have slowed me down -- including permanent arm & leg injuries -- and I need to keep going.

Originally this project started way back in the late '90s. At the time I wanted to create a tile-based game engine for making Dungeons & Dragons adventures. By 2001 the idea had evolved into tile-based terrain with arbitrarily positioned entities. Eventually -- when I was thinking about a new career in the mid-to-late '00s -- I started developing "TankEngine"; the precursor to ArmadaEngine. TankEngine's purpose was to allow easy construction of, both, tile-based and freeform play, ranging from turn-based RPGs to real-time shoot-'em-ups.
But things happened and I put the project away, viewing it as a pleasant little pipe dream. Now, though, I have good reason to resurrect it.

As for the current incarnation of all this, ArmadaEngine expands upon TankEngine's design in the following ways:
* Hybrid voxel/heightfield 3D terrain, replacing flat 2D tiles
* Multi-threaded & cache-compact modular A.I., replacing library-based fixed functions
* Modular terrain map with parallel worlds/planets/dimensions/universes/etc. access, replacing isolated closed terrain maps
* Definable chemical elements with environment-based transitions & interactions, replacing hard-coded assignable tile properties

I'm still using DirectInput 8 and OpenAL 1.1; obvious signs of me being behind the times. And we all know Direct3D 11 isn't young, either. I intend to (eventually) create a Vulkan pathway in ArmadaEngine, as well as move on to OpenAL Soft. As for DirectInput 8, I will have to create a replacement one day, I suppose. One thing a time...
