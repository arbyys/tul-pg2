# PG2 - 3D aplikace

## Popis aplikace
- **vlastnosti**
	- náhodně jezdící kolečková židle
	- střela do židle ji teleportuje na jiné (náhodné) místo
	- skleničky na stole, střela je rozbije
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

## Todo
- J: mrknout na hodnoty uniformů světel v App.cpp (diffuse, specular atd.) - zprovoznit světla
- J: implementovat hitboxy skleniček + rozbíjení (posun skleniček po ose Y) + jejich obnovení tlačítkem R
- A: trochu přepsat objloader
- A: okomentovat opengl metody
- A: formátovat kód, učesat ho, odstranit zbytečnosti
- ?: opravit čáry mezi texturama height mapy (nebo to nechat, to je asi fuk)
- ?: opravit earrape při skoku v chůzi do kopce (nebo to nechat, to je asi fuk)

## Povinné požadavky
- ✔ 3D GL Core shaders at least 4.5, GL debug
- ✔ high perforce => at least 60 FPS
- ✔ allow VSyncntrol, fullscreen vs. windowed switching (restore window position & size)
- ✔ event proceng (camera, object, app behaviour...): mouse (both axes, wheel), keyboard
- ✔ multiple dirent independently moving 3D models, at leats two loaded from file
- ✔ at least th different textures (or subtextures from texture atlas etc.)
- ❌ lighting model, at least 3 lights (ambient+directional+reflector: at least one is moving; + possible other lights)
- ✔ correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )


## Rozšiřující požadavky
- ✔ height map textured by height & proper player height coords
- ✔ working collisions (only projectiles x environment)
- ❌ particles
- ✔ scripting (useful)
- ✔ audio (better than just background)
