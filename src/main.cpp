//------------------------------------------------------------------------------
//Copyright 2003-2012 Robert Pelloni.
//All Rights Reserved.
//------------------------------------------------------------------------------

//-----------------------------
//C HEADERS
//-----------------------------

#include "main.h"


SDL_Window *window;
int WINDOW_DRAWABLE_W = 0;
int WINDOW_DRAWABLE_H = 0;
SDL_Renderer *renderer;



void (*glDrawTexiES)(int,int,int,int,int)=NULL;

void DO_NOTHING_Function();
void DO_NOTHING_Function(BOOL);

bool vbl_done=0;
bool timer_done=0;

Uint32 rmask, gmask, bmask, amask =0;

TTF_Font *font_bobsgame_8px = NULL;
TTF_Font *font_bobsgame_16px = NULL;

bool seeded=0;

bool vbl_init=0;

bool GAME_is_running=0;

bool append_screen=0;

int MAIN_QUIT=0;

bool GLOBAL_hq2x_is_on=0;

int HARDWARE_brightness=0;
int vsync=1;
int fpsmeter=0;//PORT: default off for first-run builds


//-----------------------------
//ini variables
//-----------------------------

int debug=1;

int fullscreen=0;
int skiptext=0;
int easymode=1; // TEMP: Demo 2 easy mode for testing (revert before release)
int cheater=0;

//-----------------------------
//SDL variables
//-----------------------------
bool quit = false;

//-----------------------------
//timer stuff
//-----------------------------
int framesrendered=0;
int secondspassed =0;
int lastsecondspassed=0;
int fps=0;

int vbl_ticks=0;
int render_ticks=0;

Uint64 hires_ticks_per_second;
Uint64 newvbltimer;
Uint64 lastvbltimer;





//==========================================================================================================================
void main_vbl()
{//==========================================================================================================================

	if(TITLESCREEN_running==1)TITLESCREEN_vbl();

	TITLESCREEN_vbl_counter++;
	if(TITLESCREEN_vbl_counter>600)TITLESCREEN_vbl_counter=0;
	intro_vbl_counter2++;
	if(intro_vbl_counter2>600)intro_vbl_counter2=0;

	fade_vbl_counter++;
	if(fade_vbl_counter>600)fade_vbl_counter=0;

	if(GAME_is_running==1)GAME_vbl();




	/*static int last_frame_ticks=0;
	static int frames_skipped=0;
	int ticks=SDL_GetTicks();
	if(ticks>last_frame_ticks)
	{

		int ticks_passed=ticks-last_frame_ticks;
		last_frame_ticks=ticks;

		if(ticks_passed>18)frames_skipped++;

		char ticks_passed_string[128];
		sprintf(ticks_passed_string,"frames skipped:%d",frames_skipped);

		static DEBUG_overlay_STRUCT* ticks_passed_overlay = NULL;

		if(ticks_passed_overlay==NULL)ticks_passed_overlay = DEBUG_make_overlay((char*)ticks_passed_string,(HARDWARE_SCREEN_WIDTH_PIXELS/2)-20,HARDWARE_SCREEN_HEIGHT_PIXELS-30);
		else DEBUG_update_overlay(ticks_passed_overlay,(char*)ticks_passed_string,(HARDWARE_SCREEN_WIDTH_PIXELS/2)-(ticks_passed_overlay->width/2),HARDWARE_SCREEN_HEIGHT_PIXELS-30);
	}*/



	render();

	//SDL_GL_SwapBuffers();
	SDL_GL_SwapWindow(window);

}


//==========================================================================================================================
void main_vbl_timed()
{//==========================================================================================================================

	newvbltimer = SDL_GetPerformanceCounter();
	if(newvbltimer-lastvbltimer>=(16*(hires_ticks_per_second/1000)))
	{
		lastvbltimer=newvbltimer;
		vbl_ticks=lastvbltimer;
		main_vbl();
	}

}



//==========================================================================================================================
void whilefix()
{//==========================================================================================================================
	reset_controls();

	int last_vblticks=vbl_ticks;
	while(last_vblticks==vbl_ticks)
	{
		main_vbl_timed();
	}

	check_controls();
}

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "string"
#include "fstream"

#ifdef __APPLE__
// PORT: native macOS vsync (macos_vsync.mm). Declared here at file scope —
// Clang rejects extern "C" on block-scope declarations.
extern "C" int PORT_macos_set_native_swap_interval(int);
#endif

char *textFileRead(const char *fn) {

FILE *fp;
char *content = NULL;

int count=0;

if (fn != NULL) {
	fp = fopen(fn,"rt");

	if (fp != NULL) {

fseek(fp, 0, SEEK_END);
count = ftell(fp);
rewind(fp);

		if (count > 0) {
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
}
return content;
}


/*
GLuint loadShaderFromFile( std::string path, GLenum shaderType )
{
	//Open file
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile( path.c_str() );
	//Source file loaded
	if( sourceFile )
	{
		//Get shader source
		shaderString.assign( ( std::istreambuf_iterator< char >( sourceFile ) ), std::istreambuf_iterator< char >() );

		//Create shader ID
		shaderID = glCreateShader( shaderType );

		//Set shader source
		const GLchar* shaderSource = shaderString.c_str();
		glShaderSource( shaderID, 1, (const GLchar**)&amp;shaderSource, NULL );

		//Compile shader source
		glCompileShader( shaderID );

		//Check shader for errors
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv( shaderID, GL_COMPILE_STATUS, &amp; shaderCompiled );
		if( shaderCompiled != GL_TRUE )
		{
			printf( "Unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource );

			glDeleteShader( shaderID );
			shaderID = 0;
		}
	}
	else
	{
		printf( "Unable to open file %s\n", path.c_str() );
	}

	return shaderID;

}
*/



//==========================================================================================================================
int main(int argc, char *argv[])//int argc, char **argv)
{//==========================================================================================================================

	//-----------------------------
	//load ini file
	//-----------------------------
		//easy mode
		//debug/skiptext
		//bpp
		//fullscreen
/*
		dictionary*	ini ;
		ini = iniparser_load("config.ini");
		if (ini==NULL){fprintf(stderr,"config.ini not found\n");}
		else
		{
			fullscreen = iniparser_getboolean(ini, "bobsgame:fullscreen", 0);
			easymode = iniparser_getboolean(ini, "bobsgame:easymode", 0);
			debug = iniparser_getboolean(ini, "bobsgame:debug", 0);
			///if(debug)skiptext=1;
			//s = iniparser_getstring(ini, "bobsgame:easymode", NULL);
			//i = iniparser_getint(ini, "skiptext:year", -1);
			//s = iniparser_getstring(ini, "wine:country", NULL);
			//d = iniparser_getdouble(ini, "wine:alcohol", -1.0);
			iniparser_freedict(ini);
		}
*/

	//-----------------------------
	//init error console
	//-----------------------------
		ERROR_init_error_console();

	//-----------------------------
	//init sdl
	//-----------------------------

		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS)){fprintf(stderr,"couldn't init SDL: %s\n",SDL_GetError());exit(2);}
		atexit(SDL_Quit);

	//-----------------------------
	//setup window
	//-----------------------------
		//SDL_WM_SetCaption( "\"bob's game\" alpha 1", "\"bob's game\" alpha 1" );
		//SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);








	//Use compatibility profile for legacy fixed-function OpenGL (the game uses glVertexPointer etc.)
	//macOS supports up to OpenGL 2.1 in compatibility mode
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		window = SDL_CreateWindow("\"bob's game\" alpha 1",
								640*WINDOW_SCALE, 480*WINDOW_SCALE,
								//SDL_WINDOW_FULLSCREEN |
								SDL_WINDOW_OPENGL);

		//renderer = SDL_CreateRenderer(window, -1, 0);
		//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		//SDL_RenderClear(renderer);
		//SDL_RenderPresent(renderer);




	if (window == NULL) {
		std::cerr << "There was an error creating the window: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (context == NULL) {
		std::cerr << "There was an error creating OpenGL context: " << SDL_GetError() << std::endl;
		return 1;
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL) {
		std::cerr << "There was an error with OpenGL configuration:" << std::endl;
		return 1;
	}

	SDL_GL_MakeCurrent(window, context);

	// PORT: cache the true drawable size (differs from window size on HiDPI/Retina).
	SDL_GetWindowSizeInPixels(window, &WINDOW_DRAWABLE_W, &WINDOW_DRAWABLE_H);
	fprintf(stderr, "Window drawable size: %dx%d\n", WINDOW_DRAWABLE_W, WINDOW_DRAWABLE_H);
































		//SDL_EnableUNICODE(1);
		//SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		SDL_ShowCursor();

	//-----------------------------
	//set video mode
	//-----------------------------
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

//		if(fullscreen==1)
//		SDLSurface_screen = SDL_SetVideoMode(HARDWARE_SCREEN_WIDTH_PIXELS, HARDWARE_SCREEN_HEIGHT_PIXELS, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL | SDL_FULLSCREEN);
//		else
//		SDLSurface_screen = SDL_SetVideoMode(HARDWARE_SCREEN_WIDTH_PIXELS, HARDWARE_SCREEN_HEIGHT_PIXELS, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_HWACCEL);
//
//		if(SDLSurface_screen==NULL){fprintf(stderr,"Couldn't set video mode: %s\n",SDL_GetError());exit(2);}
//
//		fprintf(stderr,"GL Version: %s\n",(char*)glGetString(GL_VERSION));


		//set screen black here and swap buffer before loading
		//SDL_GL_SwapBuffers();
	SDL_GL_SwapWindow(window);

		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x000000ff;
		#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0xff000000;
		#endif



	//-----------------------------
	//set up GL modes
	//-----------------------------

		//set up swap and framebuffer!

		glewInit();


		///todo: need to figure out how to force swap
		///todo: figure out why uses 100% gpu wtf
//PORT: pace presents at 60Hz via the vsync block itself. Game logic is
//software-gated at 16.6ms but a 120Hz display vsyncs at 8.33ms; the 0.066ms
//per-frame phase drift accumulated into a missed vsync (micro-stutter) every
//couple seconds. Swap interval refresh/60 makes the swap the 60Hz timer, so
//no drift is possible. The software 60Hz gate below then just stays satisfied.
int display_refresh=60;
{
	//SDL3: SDL_GetDesktopDisplayMode returns const SDL_DisplayMode* (refresh_rate is float)
	const SDL_DisplayMode* dm=SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(window));
	if(dm!=NULL&&dm->refresh_rate>0)display_refresh=(int)(dm->refresh_rate+0.5f);
}
int swap_interval=display_refresh/60;
if(swap_interval<1)swap_interval=1;
fprintf(stderr,"Display refresh %dHz, swap interval %d\n",display_refresh,swap_interval);
#ifdef _WIN32
		if(WGL_EXT_swap_control)
		{
			wglSwapIntervalEXT(swap_interval);
		}
		else
		{vsync=0;fprintf(stderr,"Vsync Failed.\n");}
#elif defined(__APPLE__)
		// PORT: native macOS vsync (macos_vsync.mm). SDL3 emulates the GL
		// swap interval in software on macOS and interval 2 does not yield
		// reliable 16.67ms presents on 120Hz displays (frametimes showed
		// interval-1 behavior). Drive the GPU directly; set SDL's interval
		// to 0 so its software wait doesn't stack on top of the driver's.
		// 120Hz -> native interval 2, 60Hz -> native interval 1: both pace
		// presents at exactly 16.67ms, so the 60Hz software gate below just
		// stays satisfied with no phase drift possible.
		if(PORT_macos_set_native_swap_interval(swap_interval))
		{
			SDL_GL_SetSwapInterval(0);
			fprintf(stderr,"Native macOS swap interval %d\n",swap_interval);
		}
		else if(!SDL_GL_SetSwapInterval(swap_interval))
		{vsync=0;fprintf(stderr,"Vsync Failed.\n");}
#else
		// Linux/other: use SDL for vsync control
		if(!SDL_GL_SetSwapInterval(swap_interval))
		{vsync=0;fprintf(stderr,"Vsync Failed.\n");}
#endif
		ERROR_check_SDL_and_GL_errors("framebuffer");

		if(GLEW_EXT_framebuffer_object){framebuffer=1;}
		else {framebuffer=0;fprintf(stderr,"No Framebuffer.\n");}
		ERROR_check_SDL_and_GL_errors("framebuffer");


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, HARDWARE_SCREEN_WIDTH_PIXELS, HARDWARE_SCREEN_HEIGHT_PIXELS, 0, -1, 1 );

		//glViewport(0,0,HARDWARE_SCREEN_WIDTH_PIXELS,HARDWARE_SCREEN_HEIGHT_PIXELS);
		//why not set the viewport? figure this out.

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0, 0, 0, 0);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glGenTextures(1,&screen); // make a new texture

		ERROR_check_SDL_and_GL_errors("set GL mode");


	//-----------------------------
	//initialize framebuffer
	//-----------------------------
		if(framebuffer)
		{
			glBindTexture(GL_TEXTURE_2D,screen);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8, HARDWARE_SCREEN_WIDTH_PIXELS,HARDWARE_SCREEN_HEIGHT_PIXELS,0, GL_BGRA,GL_UNSIGNED_BYTE,0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glGenerateMipmapEXT(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,0);

			glGenFramebuffersEXT(1, &fb);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, screen, 0);
		}

		ERROR_check_SDL_and_GL_errors("framebuffer");


	//-----------------------------
	//set up shaders
	//-----------------------------

		char *fs,*vs;

		vert[0] = glCreateShader(GL_VERTEX_SHADER);
		vs = textFileRead("shaders/coord.vert");
		glShaderSource(vert[0], 1, (const char **)&vs, NULL);
		free(vs);
		glCompileShader(vert[0]);

		frag[0] = glCreateShader(GL_FRAGMENT_SHADER);
		fs = textFileRead("shaders/nothing.frag");
		glShaderSource(frag[0], 1, (const char **)&fs, NULL);
		free(fs);
		glCompileShader(frag[0]);


		p[0] = glCreateProgram();
		glAttachShader(p[0], vert[0]);
		glAttachShader(p[0], frag[0]);
		glLinkProgram(p[0]);


		vert[1] = glCreateShader(GL_VERTEX_SHADER);
		vs = textFileRead("shaders/coord.vert");
		glShaderSource(vert[1], 1, (const char **)&vs, NULL);
		free(vs);
		glCompileShader(vert[1]);

		frag[1] = glCreateShader(GL_FRAGMENT_SHADER);
		fs = textFileRead("shaders/gamma.frag");
		glShaderSource(frag[1], 1, (const char **)&fs, NULL);
		free(fs);
		glCompileShader(frag[1]);

		p[1] = glCreateProgram();
		glAttachShader(p[1], vert[1]);
		glAttachShader(p[1], frag[1]);
		glLinkProgram(p[1]);

		ERROR_check_SDL_and_GL_errors("shaders");


	//-----------------------------
	//set up sound
	//-----------------------------

		SDL_AudioSpec audiospec;
		SDL_zero(audiospec);
		audiospec.freq = 44100;
		audiospec.format = SDL_AUDIO_S16;
		audiospec.channels = 2;
		if(Mix_OpenAudio(0, &audiospec)!=0){fprintf(stderr,"couldn't set up audio\n%s\n",SDL_GetError());}
		Mix_AllocateChannels(32);


		HARDWARE_sound_init(); //load wavs into buffers
		ERROR_check_SDL_and_GL_errors("sound");

	//-----------------------------
	//set up controllers
	//-----------------------------
		int joystick_count = 0;
		SDL_JoystickID *joystick_ids = SDL_GetJoysticks(&joystick_count);
		if(joystick_count>=1)
		{
			stick=SDL_OpenJoystick(joystick_ids[0]);
		}
		SDL_free(joystick_ids);

	//-----------------------------
	//set up timers
	//-----------------------------
		srand(time(NULL));
		Uint64 timer_ticks=0;

		Uint64 newtimer;//=0;
		Uint64 lasttimer;//=0;
		//Uint64 hires_ticks_per_second;//=0;
		lasttimer=0;
		lastvbltimer=0;
		hires_ticks_per_second = SDL_GetPerformanceFrequency();


	//-----------------------------
	//init TTF
	//-----------------------------
		TTF_Init();

		font_bobsgame_8px=TTF_OpenFont("data/bobsgame.ttf",8);
		font_bobsgame_16px=TTF_OpenFont("data/bobsgame.ttf",16);

	//-----------------------------
	//init game
	//-----------------------------



		GAME_init();
		GAME_is_running=1;

		//if(framebuffer)ERROR_set_error("using framebuffer");
		//if(vsync)ERROR_set_error("using vsync");



	///=======================================================================================
	while(MAIN_QUIT==false)///MAIN GAME LOOP
	{///=======================================================================================

		if(vsync==1)
		{
			// Game logic is tuned for 60 ticks/sec (20 substeps x 60fps = 1200/sec).
			// Vsync alone follows the display refresh, so on a 120Hz+ screen the
			// whole simulation runs fast. Gate logic to 60Hz; vsync still gives
			// tear-free presentation.
			// PORT: use a fixed timestep (lasttimer+=interval, not =newtimer) so the
			// gate can't drift from the vsync grid, and sleep precisely with
			// SDL_DelayNS. SDL_Delay(1) overshoots and misses the vsync deadline,
			// dropping frames (58-59fps stutter).
			// PORT: stutter-debug timing. st_t0 marks the loop-iteration start; the
			// logic/vbl/wait split lets a hitch's [STUTTER] log line show where time went.
			Uint64 st_t0 = SDL_GetPerformanceCounter();
			float st_logic_ms=0.0f, st_vbl_ms=0.0f, st_wait_ms=0.0f;
			newtimer = SDL_GetPerformanceCounter();
			Uint64 tick_interval = hires_ticks_per_second/60;
			if(newtimer-lasttimer >= tick_interval)
			{
				lasttimer+=tick_interval;
				// if badly behind (e.g. breakpoint), resync instead of spiral of death
				if(newtimer-lasttimer >= tick_interval)lasttimer=newtimer;

				Uint64 st_t1 = SDL_GetPerformanceCounter();
				// PORT: wobble test — GAME_main() picks substeps/frame by movement:
				// run 20 (5.0px cardinal / 4.0px diagonal, Bob's original run speeds),
				// walk-diagonal 20 (2.0px), walk-cardinal 21 (3.0px); see the PORT
				// block in GAME_main(). Revert to GAME_main(20) and delete that
				// block to restore Bob's original rhythm.
				GAME_main(21); //kodenermaschiniene
				Uint64 st_t2 = SDL_GetPerformanceCounter();

				//ERROR_check_SDL_and_GL_errors("GAME_main");

				main_vbl();
				Uint64 st_t3 = SDL_GetPerformanceCounter();

				st_logic_ms=(float)((st_t2-st_t1)*1000.0/(double)hires_ticks_per_second);
				st_vbl_ms=(float)((st_t3-st_t2)*1000.0/(double)hires_ticks_per_second);

				//ERROR_check_SDL_and_GL_errors("vbl");

				framesrendered++;
			}
			else
			{
				Uint64 st_w0 = SDL_GetPerformanceCounter();
				Uint64 target = lasttimer+tick_interval;
				Uint64 now = SDL_GetPerformanceCounter();
				if(target>now)
				{
					Uint64 ns_wait = (target-now)*1000000000ULL/hires_ticks_per_second;
					// PORT: SDL_DelayPrecise busy-waits for rock-solid frametime.
					// Burns a CPU core vs SDL_DelayNS, but precision wins.
					if(ns_wait>0)SDL_DelayPrecise(ns_wait);
				}
				Uint64 st_w1 = SDL_GetPerformanceCounter();
				st_wait_ms=(float)((st_w1-st_w0)*1000.0/(double)hires_ticks_per_second);
			}
			// PORT: hand the frame's timing to the stutter debugger (debug.cpp).
			{
				static Uint64 st_prev_t0=0;
				float st_total_ms = st_prev_t0==0 ? 16.666f : (float)((st_t0-st_prev_t0)*1000.0/(double)hires_ticks_per_second);
				st_prev_t0=st_t0;
				DEBUG_stutter_frame(st_total_ms,st_logic_ms,st_vbl_ms,st_wait_ms);
			}
		}
		else
		if(vsync==0)
		{
			newtimer = SDL_GetPerformanceCounter();

			if(newtimer-lasttimer>=(16*(hires_ticks_per_second/1000)))
			{
				lasttimer=newtimer;
				// PORT: wobble test — GAME_main() picks substeps/frame by movement:
				// run 20 (5.0px cardinal / 4.0px diagonal, Bob's original run speeds),
				// walk-diagonal 20 (2.0px), walk-cardinal 21 (3.0px); see the PORT
				// block in GAME_main(). Revert to GAME_main(20) and delete that
				// block to restore Bob's original rhythm.
				GAME_main(21); //kodenermaschiniene

				ERROR_check_SDL_and_GL_errors("GAME_main");

				main_vbl();

				ERROR_check_SDL_and_GL_errors("vbl");

				framesrendered++;
			}
			else
			{
				render();

				//SDL_GL_SwapBuffers();
				SDL_GL_SwapWindow(window);

				framesrendered++;

				ERROR_check_SDL_and_GL_errors("render");
			}
		}

		calculate_fps();


		Uint64 ticks = SDL_GetTicks();
		//int tickspassed = ticks-timer_ticks;
		timer_ticks=ticks;

		if(vsync==1)
		{
			//sleep for 10 ms. prevent CPU from 100%. went down to like 4% every once in awhile, nice.
			//int time=10-(tickspassed%10);//was 16/16
			//if(time>0)Sleep(time);
		}
		else
		if(vsync==0)
		{
			SDL_Delay(2);
		}

		ERROR_check_SDL_and_GL_errors("end mainloop");

	}



	TTF_CloseFont(font_bobsgame_8px);
	TTF_CloseFont(font_bobsgame_16px);


	//if(Mix_PlayingMusic())
	//{
		//Mix_FadeOutMusic(1000);
		//Mix_FreeMusic(background_MUS);
	//}

	Mix_CloseAudio();


	SDL_CloseJoystick(stick);
	SDL_Quit();

	return 0;
}



