# PG2 - 3D aplikace

## Srcs
- https://github.com/RadekMocek/PG2
- https://github.com/Bahamut731lp/PG2
- https://github.com/dominikjires/PG2
- https://github.com/DavidKarnik/pg2
- https://github.com/jirihalbych/PG2

## Popis aplikace
- **vlastnosti**
	- náhodně jezdící kolečková židle
	- střela do židle ji teleportuje na jiné (náhodné) místo
	- skleničky s vínem na stole, rozbijou se při střele
- **ovládání**
	- `WSAD`: pohyb
	- `LMB`: výstřel
	- `Space`: skok
	- `LCtrl`: sprint (držení)
	- `ESC`: ovládání kamery myší (toggle)
	- `F`: fullscreen
	- `V`: ovládání vsync
	- `R`: obnova rozbitých skleniček
	- `+` / `-`: změna FOV

## Temp poznámky
- domyslet světla (skleničky budou svítit?, vystřelený projektil bude svítit?)
- implementovat skok
- přepsat objloader

## Povinné požadavky [✔ ❌]
- ✔ 3D GL Core shaders at least 4.5, GL debug
- ✔ high perforce => at least 60 FPS
- ✔ allow VSyncntrol, fullscreen vs. windowed switching (restore window position & size)
- ✔ event proceng (camera, object, app behaviour...): mouse (both axes, wheel), keyboard
- ✔ multiple dirent independently moving 3D models, at leats two loaded from file
- ✔ at least th different textures (or subtextures from texture atlas etc.)
- ✔ lighting model, at least 3 lights (ambient+directional+reflector: at least one is moving; + possible other lights)
- ✔ correct full scale transparency (at least one transparent object; NOT if(alpha<0.1) {discard;} )


## Rozšiřující požadavky
- ❌ height map textured by height & proper player height coords
- ❌ working collisions (only projectiles x environment)
- ❌ particles
- ❌ scripting (useful)
- ❌ audio (better than just background)
