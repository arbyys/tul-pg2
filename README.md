# PG2 - 3D aplikace

## Popis aplikace
- **závislosti**
	- OpenGL 4.9+ (cestu k instalaci umístit do PATH, např. `opencv\build\x64\vc16\bin`)
- **vlastnosti**
	- náhodně jezdící a točící se kolečková židle
	- střela do židle ji teleportuje na jiné (náhodné) místo
	- průhledné skleničky na stole, střela je rozbije
- **ovládání**
	- `WSAD`: pohyb
	- `LMB`: výstřel
	- `Space`: skok
	- `LCtrl`: sprint
	- `ESC`: vypnutí ovládání kamery pomocí myši
	- `F`: fullscreen
	- `V`: ovládání vsync
	- `R`: obnova rozbitých skleniček
	- `C`: ovládání zobrazení crosshairu
	- `SCROLL`: změna FOV
	- `MMB`: reset FOV na základní hodnotu
	- `Q`: vypnutí aplikace

## Povinné požadavky
- ✔ 3D GL Core shaders at least 4.5, GL debug
- ✔ high perforce => at least 60 FPS
- ✔ allow VSyncntrol, fullscreen vs. windowed switching (restore window position & size)
- ✔ event proceng (camera, object, app behaviour...): mouse (both axes, wheel), keyboard
- ✔ multiple dirent independently moving 3D models, at leats two loaded from file
- ✔ at least th different textures (or subtextures from texture atlas etc.)
- ✔ lighting model, at least 3 lights (ambient+directional+reflector: at least one is moving; + possible other lights)
- ✔ correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )


## Rozšiřující požadavky
- ✔ height map textured by height & proper player height coords
- ✔ working collisions (only projectiles x environment)
- ❌ particles
- ✔ scripting (useful)
- ✔ audio (better than just background)
