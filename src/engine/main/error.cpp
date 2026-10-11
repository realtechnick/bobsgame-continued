#include <cstring>





#include "../../main.h"
//#include "error.h"


bool error_console_on=1;
char* error_messages[128];
int error_array[128]={0};

DEBUG_overlay_STRUCT* DEBUG_overlays[128];

char error_message[1024];
char sdlerror[1024];
char lastsdlerror[1024];



//===========================================================================================================================
texture_STRUCT* load_texture_from_non_square_surface(SDL_Surface *surface)
{//===========================================================================================================================

	GLuint texture_id;

	int image_w = surface->w;
	int image_h = surface->h;

	int texture_w = power_of_two(image_w);
	int texture_h = power_of_two(image_h);


	//blit from bitmap to a temporary surface with 32 bits per pixel
	SDL_Surface* temp = SDL_CreateSurface(texture_w, texture_h, SDL_PIXELFORMAT_RGBA32);

	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = image_w;
	area.h = image_h;

	//SDL_SetAlpha(surface, 0, 0);

	SDL_BlitSurface(surface, &area, temp, NULL);


	//create texture
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	if (SDL_MUSTLOCK(temp))
		SDL_LockSurface(temp);

	//make texture from temp surface pixels
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp->pixels);



	if (SDL_MUSTLOCK(temp))
		SDL_UnlockSurface(temp);

	//set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//free temporary surface
	SDL_DestroySurface(temp);


	texture_STRUCT* temp_TEXTURE =	(texture_STRUCT*)malloc(sizeof(texture_STRUCT));

	temp_TEXTURE->image_w=image_w;
	temp_TEXTURE->image_h=image_h;
	temp_TEXTURE->texture_w=texture_w;
	temp_TEXTURE->texture_h=texture_h;
	temp_TEXTURE->texture_id=texture_id;




	return temp_TEXTURE;
}

//===========================================================================================================================
void delete_texture(texture_STRUCT *temp_TEXTURE)
{//===========================================================================================================================


	temp_TEXTURE->image_w=0;
	temp_TEXTURE->image_h=0;
	temp_TEXTURE->texture_w=0;
	temp_TEXTURE->texture_h=0;


	if(glIsTexture(temp_TEXTURE->texture_id))
	{
		glDeleteTextures(1, &temp_TEXTURE->texture_id);
		//glFlush();
	}

	free(temp_TEXTURE);

}

//===========================================================================================================================
void draw_texture_struct(texture_STRUCT* PLAYER_TEXTURE, int x, int y)
{//===========================================================================================================================

	GLuint texture_id = PLAYER_TEXTURE->texture_id;
	int w = PLAYER_TEXTURE->texture_w;
	int h = PLAYER_TEXTURE->texture_h;


	glBindTexture(GL_TEXTURE_2D,texture_id);
	glEnable(GL_TEXTURE_2D);


	GLfloat box[] = {x,y + h,0,  x + w,y + h,0,     x + w, y, 0,   x,y,0};
	GLfloat tex[] = {0,1, 1,1, 1,0, 0,0};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0,box);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}





//===========================================================================================================================
void ERROR_init_error_console()
{//===========================================================================================================================

	int x=0;
	for(x=0;x<128;x++)error_messages[x]=NULL;

	for(x=0;x<128;x++)DEBUG_overlays[x]=NULL;

}

//===========================================================================================================================
void ERROR_set_error(const char* error_string)
{//===========================================================================================================================


		//copy the new error string into a stored string
		char* new_error = (char*)malloc( (strlen(error_string)+2) * sizeof(char) );
		sprintf(new_error,"%s\n",error_string);

		//add this string to the error message array
		int slot=-1;
		int x=0;
		for(x=0;x<128;x++)if(error_messages[x]==NULL){slot=x;x=128;break;}

		if(slot==-1)
		{
			//too many errors, could recycle the array, could use linked list, could delete errors as they get bumped off screen.
			return;
		}

		error_messages[slot]=new_error;

		fprintf(stderr,new_error);


}


//===========================================================================================================================
void ERROR_draw_error_console()
{//===========================================================================================================================
	//go through error_messages
	//find non-NULL value

	int amount_of_errors=0;

	int x=0;
	for(x=0;x<128;x++)
	if(error_messages[x]!=NULL)amount_of_errors++;


	//start at top of screen
	//fill values

	int line_height=12;

	int max_lines;
	max_lines = HARDWARE_SCREEN_HEIGHT_PIXELS/line_height;

	//if height of screen is greater than lines that can fit, start at the last error - amount

	int start_error=0;
	if(amount_of_errors>max_lines)start_error=amount_of_errors-max_lines;


	SDL_Surface* error_SURFACE=NULL;
	texture_STRUCT* error_TEXTURE = NULL;
	SDL_Color green_COLOR = {0,255,0,255};
	SDL_Color white_COLOR = {255,255,255,255};
	SDL_Color black_COLOR = {0,0,0,255};

	int line=0;
	//draw font to screen
	for(x=amount_of_errors-1;x>=start_error;x--)
	{
		line++;

		if(x==amount_of_errors-1)error_SURFACE=TTF_RenderText_Shaded(font_bobsgame_8px,error_messages[x],strlen(error_messages[x]),green_COLOR, black_COLOR);
		else error_SURFACE=TTF_RenderText_Shaded(font_bobsgame_8px,error_messages[x],strlen(error_messages[x]),white_COLOR, black_COLOR);

		error_TEXTURE=load_texture_from_non_square_surface(error_SURFACE);

		draw_texture_struct(error_TEXTURE,0,(HARDWARE_SCREEN_HEIGHT_PIXELS)-(line_height*line));

		if(error_SURFACE!=NULL){SDL_DestroySurface(error_SURFACE);error_SURFACE=NULL;}
		if(error_TEXTURE!=NULL){delete_texture(error_TEXTURE);error_TEXTURE=NULL;}
	}
}


//===========================================================================================================================
void ERROR_check_SDL_and_GL_errors(const char* error_string)
{//===========================================================================================================================

	if(debug==1)
	{

		strcpy(error_message,error_string);

		//check GL errors
		int errloop=0;

		do
		{
			error_array[errloop]=glGetError();
			errloop++;
		}
		while(error_array[errloop-1]!=0);


		errloop=0;
		while(error_array[errloop]!=0)
		{

			int val = error_array[errloop];

			if(val==GL_INVALID_ENUM)strcat(error_message," GL_INVALID_ENUM \n");
			else if(val==GL_INVALID_VALUE)strcat(error_message," GL_INVALID_VALUE \n");
			else if(val==GL_INVALID_OPERATION)strcat(error_message," GL_INVALID_OPERATION \n");
			else if(val==GL_STACK_OVERFLOW)strcat(error_message," GL_STACK_OVERFLOW \n");
			else if(val==GL_STACK_UNDERFLOW)strcat(error_message," GL_STACK_UNDERFLOW \n");
			else if(val==GL_OUT_OF_MEMORY)strcat(error_message," GL_OUT_OF_MEMORY \n");
			//else if(val==GL_TABLE_TOO_LARGE)strcat(error_message," GL_TABLE_TOO_LARGE \n");
			else strcat(error_message," UNKNOWN GL ERROR \n");

			//fprintf(stderr,error_message);
			ERROR_set_error(error_message);

			errloop++;
		}

		//check SDL errors

		strcpy(sdlerror,SDL_GetError());
		if(strcmp(lastsdlerror,sdlerror)!=0)
		{
			strcpy(lastsdlerror,sdlerror);
			//fprintf(stderr,sdlerror,"\n");
			ERROR_set_error(sdlerror);
		}
	}
}




//===========================================================================================================================
DEBUG_overlay_STRUCT* DEBUG_make_overlay(const char* text, int hardware_screen_x, int hardware_screen_y)
{//===========================================================================================================================


//this uses actual hardware screen x and y because it does not render as a game sprite like most other objects like captions, so it isnt affected by ZOOM


	//copy the new error string into a stored string
	DEBUG_overlay_STRUCT* overlay = (DEBUG_overlay_STRUCT*)malloc(sizeof(DEBUG_overlay_STRUCT));

	//add this string to the error message array
	int slot=-1;
	int x=0;
	for(x=0;x<128;x++)if(DEBUG_overlays[x]==NULL){slot=x;x=128;break;}

	if(slot==-1)
	{
		//too many errors, could recycle the array, could use linked list, could delete errors as they get bumped off screen.
		return NULL;
	}

	DEBUG_overlays[slot]=overlay;

	sprintf(overlay->text,text);
	overlay->x=hardware_screen_x;
	overlay->y=hardware_screen_y;
	overlay->width=0;


	return overlay;

}

//===========================================================================================================================
void DEBUG_update_overlay(DEBUG_overlay_STRUCT* overlay, const char* text, int hardware_screen_x, int hardware_screen_y)
{//===========================================================================================================================

	sprintf(overlay->text,text);
	overlay->x=hardware_screen_x;
	overlay->y=hardware_screen_y;

}

//===========================================================================================================================
void DEBUG_delete_overlay(DEBUG_overlay_STRUCT* overlay)
{//===========================================================================================================================

	if(overlay==NULL)return;

	int x=0;
	for(x=0;x<128;x++)if(DEBUG_overlays[x]==overlay){DEBUG_overlays[x]=NULL;x=128;break;}

	free(overlay);

}

//===========================================================================================================================
void DEBUG_draw_overlays()
{//===========================================================================================================================


	SDL_Surface* error_SURFACE=NULL;
	texture_STRUCT* error_TEXTURE = NULL;
	SDL_Color white_COLOR = {255,255,255,255};
	SDL_Color black_COLOR = {0,0,0,255};

	int x=0;
	for(x=0;x<128;x++)
	{
		if(DEBUG_overlays[x]!=NULL)
		{

			error_SURFACE=TTF_RenderText_Shaded(font_bobsgame_8px,DEBUG_overlays[x]->text,strlen(DEBUG_overlays[x]->text),white_COLOR, black_COLOR);
			error_TEXTURE=load_texture_from_non_square_surface(error_SURFACE);

			draw_texture_struct(error_TEXTURE,DEBUG_overlays[x]->x,DEBUG_overlays[x]->y);

			DEBUG_overlays[x]->width=error_TEXTURE->image_w;

			if(error_SURFACE!=NULL){SDL_DestroySurface(error_SURFACE);error_SURFACE=NULL;}
			if(error_TEXTURE!=NULL){delete_texture(error_TEXTURE);error_TEXTURE=NULL;}


		}
	}

	MINIMAP_draw();

}

//===========================================================================================================================
// MINIMAP test -- CONTINUED (Nick), 2026-10-11 (v9).
// 512x512 texels, 1 texel = 1 game px (full-res, no undersampling), drawn at
// 512 true physical px on a side: with the Retina backing store
// (SDL_WINDOW_HIGH_PIXEL_DENSITY) every texel is native 1x1 physical. Covers
// 512x512 game px centered on Yuu. All 4 bg layers composited back-to-front
// (3,0,2,1) with HARDWARE_MAP_PALETTE_ENHANCED_RGB (time-of-day correct).
// Sprites (people, cars, signs, Yuu himself) are drawn from each NPC's current
// GPU texture at 1:1 physical px, so walk animation matches the game. Toggle F4.
//===========================================================================================================================
#define MINIMAP_PX 512 //texture texels; 1 texel = 1 game px
#define MINIMAP_X 8
//on-screen box is sized at draw time: 512 true physical px on a side, so texels
//are native 1x1 physical (requires the Retina backing store)

int minimap_on=1;

static GLuint minimap_tex_id=0;
static unsigned char minimap_px[MINIMAP_PX*MINIMAP_PX*4];

//draw one NPC's current sprite frame onto the minimap at 1:1 physical px.
//uses the preloaded frame texture cache directly (same lookup as
//HARDWARE_update_sprite_texture), so it's always the current animation frame
//and immune to the per-frame sprite create/delete cycle.
//falls back to a yellow dot if the frame texture isn't available.
static void MINIMAP_draw_npc(NPC* npc, int ox, int oy, float px2u, float bx0, float by0, float bsz)
{
	if(npc==NULL||!npc->visible)return;
	if(npc->gfx==NULL)return;
	GFX* gfx=npc->gfx;

	int nx=npc->MAP_x-ox;
	int ny=npc->MAP_y-oy;

	GLuint tex=0;
	int sw=0,sh=0,tw=0,th=0;
	int cache=gfx->IndexInCachedTextureIDArray;
	int dsx=gfx->data_size_x, dsy=gfx->data_size_y;
	if(cache>=0&&dsx>0&&dsy>0)
	{
		int frame=npc->gfx_index/(dsx*dsy);
		if(frame>=0&&frame<64)
		{
			GLuint t=GLTex_preloaded_sprite_textures[cache*64+frame];
			if(glIsTexture(t))
			{
				tex=t;
				sw=dsx; sh=dsy; //native frame size
				tw=gfx->texture_size_x; th=gfx->texture_size_y;
			}
		}
	}

	if(tex==0||sw<=0||sh<=0||tw<=0||th<=0)
	{
		//fallback: 3x3 physical px yellow square (plain GL, no texture)
		if(nx<1||ny<1||nx>=MINIMAP_PX-1||ny>=MINIMAP_PX-1)return;
		float dx=nx*px2u, dy=ny*px2u;
		float dw=3.0f*px2u;
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0f,1.0f,0.0f,1.0f);
		GLfloat box[]={bx0+dx,by0+dy,0,  bx0+dx,by0+dy+dw,0,  bx0+dx+dw,by0+dy+dw,0,  bx0+dx+dw,by0+dy,0};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,box);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		return;
	}

	//cull outside the box (with sprite-size margin)
	float mx=bx0+nx*px2u;
	float my=by0+ny*px2u;
	float mw=sw*px2u;
	float mh=sh*px2u;
	if(mx+mw<bx0||my+mh<by0||mx>bx0+bsz||my>by0+bsz)return;

	//crisp 1:1 (draw_sprite resets the filter for the main view every frame,
	//so borrowing NEAREST here is safe)
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glEnable(GL_TEXTURE_2D);

	//same subrect pattern as draw_texture_alpha: frame lives at UV (0,0)-(sw/tw,sh/th)
	float tx=(float)sw/(float)tw;
	float ty=(float)sh/(float)th;

	GLfloat box[]={mx,my,0,  mx,my+mh,0,  mx+mw,my+mh,0,  mx+mw,my,0};
	GLfloat texc[]={0,0,  0,ty,  tx,ty,  tx,0};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,box);
	glTexCoordPointer(2,GL_FLOAT,0,texc);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//===========================================================================================================================
static void MINIMAP_draw_npc_layer(int sprite_layer,int ox,int oy,float px2u,float bx0,float by0,float bsz)
{//===========================================================================================================================
	//Draws all NPC sprites (plus Yuu) whose layer matches, in game order.
	//sprite_layer 0..3, or 99 for layer>=4 (drawn last, like render.cpp).
	NPC* npc;
	for(npc=first_NPC;npc!=NULL;npc=npc->next)
	{
		if(npc==PLAYER_npc)continue;
		int nl=npc->layer;
		if(sprite_layer==99){if(nl<4)continue;}
		else if(nl!=sprite_layer)continue;
		MINIMAP_draw_npc(npc,ox,oy,px2u,bx0,by0,bsz);
	}
	if(PLAYER_npc!=NULL)
	{
		int nl=PLAYER_npc->layer;
		int match=(sprite_layer==99)?(nl>=4):(nl==sprite_layer);
		if(match)MINIMAP_draw_npc(PLAYER_npc,ox,oy,px2u,bx0,by0,bsz);
	}
}

void MINIMAP_draw()
{//===========================================================================================================================
	if(minimap_on==0)return;
	if(PLAYER_npc==NULL)return;
	if(HARDWARE_map_tileset==NULL)return;
	int mw=HARDWARE_map_width_tiles;
	int mh=HARDWARE_map_height_tiles;
	if(mw<=0||mh<=0)return;

	//bg layers by number
	unsigned short* bgl[4];
	bgl[3]=HARDWARE_map_3;
	bgl[0]=HARDWARE_map_0;
	bgl[2]=HARDWARE_map_2;
	bgl[1]=HARDWARE_map_1;
	int li;
	for(li=0;li<4;li++)if(bgl[li]==NULL)return;

	//exact game render order (render.cpp): bg3,spr0, bg0,spr3, bg2,spr2, bg1,spr1, spr4+
	int bg_order[4]={3,0,2,1};
	int spr_after[4]={0,3,2,1};

	//active raster resolution = physical px in the box (512 Retina, 256 non-Retina).
	//1 texel = 1 world px = 1 physical px, always crisp 1:1.
	int mm_res=512;
	if(WINDOW_DRAWABLE_W>0)mm_res=(int)(128.0f*(float)WINDOW_DRAWABLE_W/640.0f+0.5f);
	if(mm_res>MINIMAP_PX)mm_res=MINIMAP_PX;
	if(mm_res<64)mm_res=64;
	//1:1 -- origin is Yuu's position minus half the box, pixel-exact
	int ox=PLAYER_npc->MAP_x-(mm_res/2);
	int oy=PLAYER_npc->MAP_y-(mm_res/2);

	float px2u=0.25f; //units per physical px (fallback)
	if(WINDOW_DRAWABLE_W>0)px2u=640.0f/(float)WINDOW_DRAWABLE_W;
	float mm_box=128.0f; //constant 20% of window width on any display
	float mm_x=(float)MINIMAP_X;
	float mm_y=480.0f-8.0f-mm_box;

	//clip everything to the box (GL coords are y-up)
	if(WINDOW_DRAWABLE_W>0&&WINDOW_DRAWABLE_H>0)
	{
		GLint sx=(GLint)(mm_x*WINDOW_DRAWABLE_W/640.0f);
		GLint sy=(GLint)((480.0f-mm_y-mm_box)*WINDOW_DRAWABLE_H/480.0f);
		GLsizei swd=(GLsizei)(mm_box*WINDOW_DRAWABLE_W/640.0f);
		GLsizei sht=(GLsizei)(mm_box*WINDOW_DRAWABLE_H/480.0f);
		glEnable(GL_SCISSOR_TEST);
		glScissor(sx,sy,swd,sht);
	}

	//persistent texture, updated per pass
	if(minimap_tex_id==0)
	{
		glGenTextures(1,&minimap_tex_id);
		glBindTexture(GL_TEXTURE_2D,minimap_tex_id);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,MINIMAP_PX,MINIMAP_PX,0,GL_RGBA,GL_UNSIGNED_BYTE,minimap_px);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}

	int step;
	for(step=0;step<4;step++)
	{
		int bg=bg_order[step];
		unsigned short* ml=bgl[bg];
		int is_first=(step==0);

		//rasterize this bg layer; transparent -> alpha 0 (blending shows beneath).
		//first pass is opaque dark slate so the box has a base.
		int bx,by;
		for(by=0;by<mm_res;by++)
		{
			int wy=oy+by;
			int ty=wy>=0?wy/8:-1; //floor-divide: C truncates toward zero
			int pyi=wy>=0?wy%8:-1;
			for(bx=0;bx<mm_res;bx++)
			{
				int wx=ox+bx;
				int tx=wx>=0?wx/8:-1;
				int pxi=wx>=0?wx%8:-1;
				int o=(by*MINIMAP_PX+bx)*4;
				unsigned char r=14,g=18,b=28,a=255;
				if(tx>=0&&ty>=0&&tx<mw&&ty<mh)
				{
					int tile=ml[tx+ty*mw];
					int pi=HARDWARE_map_tileset[64*tile+pxi+pyi*8];
					if(pi==0)
					{
						if(!is_first)a=0;
					}
					else
					{
						r=HARDWARE_MAP_PALETTE_ENHANCED_RGB[3*pi+0];
						g=HARDWARE_MAP_PALETTE_ENHANCED_RGB[3*pi+1];
						b=HARDWARE_MAP_PALETTE_ENHANCED_RGB[3*pi+2];
					}
				}
				else if(!is_first)a=0;
				minimap_px[o+0]=r;
				minimap_px[o+1]=g;
				minimap_px[o+2]=b;
				minimap_px[o+3]=a;
			}
		}

		//1px light-grey border
		int i;
		for(i=0;i<mm_res;i++)
		{
			int t=i*4;
			int b=((mm_res-1)*MINIMAP_PX+i)*4;
			int l=(i*MINIMAP_PX)*4;
			int rr=(i*MINIMAP_PX+mm_res-1)*4;
			minimap_px[t+0]=160;minimap_px[t+1]=160;minimap_px[t+2]=160;minimap_px[t+3]=255;
			minimap_px[b+0]=160;minimap_px[b+1]=160;minimap_px[b+2]=160;minimap_px[b+3]=255;
			minimap_px[l+0]=160;minimap_px[l+1]=160;minimap_px[l+2]=160;minimap_px[l+3]=255;
			minimap_px[rr+0]=160;minimap_px[rr+1]=160;minimap_px[rr+2]=160;minimap_px[rr+3]=255;
		}

		glBindTexture(GL_TEXTURE_2D,minimap_tex_id);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,mm_res,mm_res,GL_RGBA,GL_UNSIGNED_BYTE,minimap_px);
		glEnable(GL_TEXTURE_2D);

		float mm_u=(float)mm_res/(float)MINIMAP_PX;
		GLfloat box[]={mm_x,mm_y+mm_box,0,  mm_x+mm_box,mm_y+mm_box,0,     mm_x+mm_box,mm_y,0,   mm_x,mm_y,0};
		GLfloat tex[]={0,1, mm_u,1, mm_u,1-mm_u, 0,1-mm_u};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,box);
		glTexCoordPointer(2,GL_FLOAT,0,tex);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		//sprites for this layer (game order)
		MINIMAP_draw_npc_layer(spr_after[step],ox,oy,px2u,mm_x,mm_y,mm_box);
	}

	//sprites on layer >=4 draw last, like the game
	MINIMAP_draw_npc_layer(99,ox,oy,px2u,mm_x,mm_y,mm_box);

	glDisable(GL_SCISSOR_TEST);

}
