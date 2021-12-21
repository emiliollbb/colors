#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include "microtar.h"
#include "jengine.hpp"

using namespace std;

Resource::Resource(string &file_name) {
    this->file_name=file_name;
    this->data=NULL;
    this->size=0;
}

Resource::~Resource() {
    if(this->data!=NULL) {
        delete[] this->data;
        this->data=NULL;
    }
}

string Resource::getFileName(void) {
    return this->file_name;
}

void Resource::load(void) {
    // https://github.com/rxi/microtar
    mtar_t tar;
    mtar_header_t h;
    int status;
    
    /* Open tar archive for reading */
    status = mtar_open(&tar, "game.dat", "r");    
    if(status!=0) {
        string error = "Error opening tar file "+ string(mtar_strerror(status));
        throw JEngineException(error);
    }
    
    /* Find file */
    status = mtar_find(&tar, this->file_name.c_str(), &h);
    if(status!=0) {
        string error = "Error locating resource in tar file "+ string(mtar_strerror(status));
        throw JEngineException(error);
    }
    
    /* Initialize data */
    this->size = h.size;
    this->data = new char[this->size];
    
    /* Read file */
    status = mtar_read_data(&tar, this->data, this->size);
    if(status!=0) {
        string error = "Error reading resource in tar file "+ string(mtar_strerror(status));
        throw JEngineException(error);
    }
    
    /* Close archive */
    status = mtar_close(&tar);
    if(status!=0) {
        string error = "Error closing tar file "+ string(mtar_strerror(status));
        throw JEngineException(error);
    }    
}

char* Resource::getData(void) {
    return this->data;
}

long Resource::getSize(void) {
    return this->size;
}

JEngineException::JEngineException(string& what) {
	this->message = what;
}

string* JEngineException::what(void) {
	return &message;
}

JEngine::JEngine() {
}

JEngine::~JEngine() {  
}

void JEngine::init() {
    quit=false;
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
    {
	  string error = "SDL could not be initialized! SDL Error: " + string(SDL_GetError());
      throw JEngineException(error);
    }
  
    //Set texture filtering to linear
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  
    //Check for joysticks 
    for(int i=0; i<SDL_NumJoysticks(); i++)
    {
      //Load joystick 
      sdl_gamepads[i] = SDL_JoystickOpen(i); 
      if(sdl_gamepads[i] == NULL ) 
      {
		 string error =  "Unable to open game controller #" + to_string(i) + "! SDL Error: " + string(SDL_GetError());
	     throw JEngineException(error);
      }
    }
      
    // Get display mode
    if (SDL_GetDesktopDisplayMode(0, &sdl_display_mode) != 0) {
      string error = "SDL_GetDesktopDisplayMode faile! SDL Error: " + string(SDL_GetError());
      throw JEngineException(error);
    }
    SCREEN_WIDTH=sdl_display_mode.w;
    SCREEN_HEIGHT=sdl_display_mode.h;
    
    //Create window
    sdl_window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
    if( sdl_window == NULL )
    {
	  string error = "Window could not be created! SDL Error: " + string(SDL_GetError());
	  throw JEngineException(error);
    }
  
    //Create renderer for window
    this->sdl_renderer = SDL_CreateRenderer( sdl_window, -1, SDL_RENDERER_ACCELERATED );
    if( this->sdl_renderer == NULL )
    {
	  string error = "Renderer could not be created! SDL Error: " + string(SDL_GetError());
	  throw JEngineException(error);
    }
    
    //Initialize PNG loading
    if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) )
    {
	  string error = "SDL_image could not initialize! SDL Error: " + string(SDL_GetError());
	  throw JEngineException(error);
    }
    
    //Initialize SDL_ttf 
    if(TTF_Init()<0) 
    {
        string error = "SDL_ttf could not initialize! SDL_ttf Error: " + string(TTF_GetError());
        throw JEngineException(error);
    }
  
    //Initialize SDL_mixer 
    if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 512 )<0) 
    { 
        string error = "SDL_mixer could not initialize!";
        throw JEngineException(error);
    }

}

void JEngine::close() {
    
    //Destroy renderer  
    if(this->sdl_renderer!=NULL)
    {
      SDL_DestroyRenderer( this->sdl_renderer );
      this->sdl_renderer=NULL;
    }
  
    if(sdl_window != NULL)
    {
      // Destroy window
      SDL_DestroyWindow( this->sdl_window );
      this->sdl_window=NULL;
    }
  
    // Close gamepads
    for(int i=0; i<SDL_NumJoysticks(); i++)
    {
      SDL_JoystickClose(this->sdl_gamepads[i]);
      this->sdl_gamepads[i]=NULL;
    }
  
    // Close SDL_mixer
    Mix_CloseAudio();
    // Close SDL TTF
    TTF_Quit();
    // Close SDL IMG
    IMG_Quit();
    // Close SDL
    SDL_Quit();
}

void JEngine::process_input_internal(SDL_Event *e)
{
    //User requests quit
    if(e->type == SDL_QUIT 
      // User press ESC or q
      || (e->type == SDL_KEYDOWN && (e->key.keysym.sym=='q' || e->key.keysym.sym == 27))
    )
    {
      quit = 1;
    }
    else {
        process_input(e);
    }
}

void JEngine::sync_render()
{
    unsigned int start, end, render_time; 
  
    start = SDL_GetTicks();
  
    // Count frames
    frames++;
    // Update game data
    update_game();
  
    // Render screen
    render();  
  
  
    end = SDL_GetTicks();
    render_time = end - start;
  
    // 60 fps -> 16ms
    // 30 fps -> 32ms
    // 50 fps -> 20ms
    // 100 fps -> 10ms
    if(render_time < 20)
    {
      //remaining = 1;
      SDL_Delay(20 - render_time);
    }
    else
    {
      printf("Render time: %ud !!!!\n", render_time);
    }  
}

void JEngine::run()
{
    //Event handler
    SDL_Event e;
  
    // Init quit flag
    quit=0;
  
    // Init SDL
    init();
  
    // Load Media
    load_media();
      
    // Main game loop
    while(!quit)
    {
      //Handle events on queue
      while( SDL_PollEvent( &e ) != 0 )
      {
        process_input_internal(&e);
      }
    
      // Update & Render
      sync_render();
    }
  
    // Close media
    close_media();
    // Close SDL
    close();
}

void JEngine::render() {
    //Clear screen
    SDL_SetRenderDrawColor( sdl_renderer, 0x00, 0x00, 0x00, 0xFF );
    SDL_RenderClear( sdl_renderer );
  
    // Actual rendering
    render_game();
  
    //Update screen
    SDL_RenderPresent(sdl_renderer);
}

void JEngine::load_texture(struct sized_texture *texture, Resource *res) {
    // Aux surface
    SDL_Surface* loadedSurface;
    SDL_RWops *rw;
  
    //Load image at specified path
    rw = SDL_RWFromMem(res->getData(), res->getSize());
    loadedSurface = IMG_Load_RW(rw, 1);
    if(loadedSurface == NULL )
    {
      string error = "Unable to load image %s! SDL_image Error: " + res->getFileName() + string(IMG_GetError());
	  throw JEngineException(error);
    }
    //Get image dimensions 
    texture->width = loadedSurface->w; 
    texture->height = loadedSurface->h;
    //Create texture from surface pixels
    texture->texture = SDL_CreateTextureFromSurface(sdl_renderer, loadedSurface);
    if( texture->texture == NULL )
    {
      string error = "Unable to create texture from %s! SDL Error: " + res->getFileName() + string(SDL_GetError());
	  throw JEngineException(error);
    }
  
    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
}

void JEngine::load_texture(struct sized_texture *texture, string filename) {
    Resource *resource = new Resource(filename);
    resource->load();
    load_texture(texture, resource);
    delete resource;
    
}

void JEngine::update_game(){}
void JEngine::render_game(){}
void JEngine::load_media(){}
void JEngine::close_media(){}
void JEngine::process_input(SDL_Event *e){}

