# Polycubes 
## by Antonino Perricone
A OpenGL/OpenAL Blockout clone

Old repository with screenshot: https://sourceforge.net/projects/polycubes/

To see correctly this file and other file of code set tab intent to 4.

# Credits
This game is based upon a game by Aleksander Ustaszewski of 1989 and published by California Dreams.
The music in game are made by Kevin MacLeod (site).
This game use OpenGL (www.opengl.org), OpenAL (www.openal.org) and libpng(site).
I search people for conversion for other operation system above Windows and people for translation on other languages above Italian and english. All names will be listed in this section.


# File list source
### Windows
	systemInterface.h				Conjugation file between SO and game
	WinMain.cpp						Principal file of windows: all SO calls must be on one file
	icon1.ico/icon.rc/resource.h	File for windows icon of game
### Utils
	primitives.h					All base class for graphic and physics counts
	dynamicMesh.h/cpp				A class for a symple way to create mesh by code
	font.h/cpp						A class for writing text
	image.h/cpp						A class for intefacing with libpng
	lockit.h/cpp					A class for managing of languages
	Wave.h/cpp						A class for intefacing with OpenAL
### Game
	game.h/cpp						The class witch manage all game
	BOCube.cpp						A class for a single 3D cube, geometry and texturing
	GameSet.h/cpp					3 classes for the game modes, the set of pieces, the sizes and records and lists of these.
	hole.h/cpp						A class for the hole of game, its texturing and save and test cubes in it
	match.h/cpp						A class for a match of game, the level sequence and the score
	Pieces.h/cpp					A class that conserve the 41 possible pieces and give only the useful
	RadiosityMan.h/cpp				The class with the code for matrix radiosity of game and multi-thread
	Collision.cpp					In this file the code for ray collision with the hole.
### Menu Elements
	menuElements.h					The declamations of menu elements in next files
	Arrow.cpp						A class for drawing triangle oriented
	Button.cpp						A class for drawing and manage buttons
	ComboBox.cpp					A class for drawing and manage combo-box
	EditBox.cpp						A class for drawing and manage edit-box
	Rectangle.cpp					A class for drawing reclagle fill, hollow, tube-frame and of glass
	ScrollBar.cpp					A class for drawing and manage scroll-bar (used by combo-box)
	Slider.cpp						A class for drawing and manage Slide (a number with 2 arrow)
### Menu
	menus.h							Definitions of menus
	dialog.cpp						A class for DialogBox
	EditMenu.cpp					A class for the screen where the user set the game
	gallery.cpp						A class for the screen where show the pieces (easter egg)
	help.cpp						A class for the screen where show the help
	Ingame.cpp						A class for the dialog on game pause
	mainmenu.cpp					A class for the main screen
	Options.cpp						A class for the options screen
	Records.cpp						A class for the record screen

# Recommendations
This is code is property if Antonino Perricone, all modification must be approved by him.
I try to made a clean and clear code but it has a few of comments.

# Wish-list
For first I dream a Haiku, a Linux and a Mac version of this game, I think i can doing the Haiku version.
The thread code is rooted on ReadiostyMan.cpp and this file shoud be a platform-independent.
In first version i thought the piece in game will be made with class, distort the hole below it and make a caustic and a shadow in walls and installed pieces.
This code for an empy hole 6x6x12 in quality 0 use plus of 500.000 ray testing, I read the book "realistic Image systhesis using photon mapping" by Henrik Wann Jensen freshly. And i think i can make a better indirect illumination with 100.000 photon tracking, use other 100.000 for make the next piece with caustic make the game more smooth.
