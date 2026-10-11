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
// MINIMAP test -- CONTINUED (Nick), 2026-10-11 (v2).
// 128x128 box in the bottom-left corner at 0.25x (each minimap px = 4x4 game px,
// so the box covers 512x512 game px). All 4 bg layers composited back-to-front
// exactly like the renderer (3,0,2,1), using HARDWARE_MAP_PALETTE_ENHANCED_RGB
// so colors match the game's time-of-day. The origin is 4px-quantized so the map
// scrolls in whole minimap px and the white player dot stays centered.
// Toggle with F4. Single-commit feature.
//===========================================================================================================================
#define MINIMAP_PX 128
#define MINIMAP_X 8
#define MINIMAP_Y (480-8-128)
#define MINIMAP_GPX 4 //game px per minimap px (0.25x)

int minimap_on=1;

static GLuint minimap_tex_id=0;
static unsigned char minimap_px[MINIMAP_PX*MINIMAP_PX*4];

void MINIMAP_draw()
{//===========================================================================================================================
	if(minimap_on==0)return;
	if(PLAYER_npc==NULL)return;
	if(HARDWARE_map_tileset==NULL)return;
	int mw=HARDWARE_map_width_tiles;
	int mh=HARDWARE_map_height_tiles;
	if(mw<=0||mh<=0)return;

	//bg layers back-to-front, same order the renderer draws them
	unsigned short* mlayers[4];
	mlayers[0]=HARDWARE_map_3;
	mlayers[1]=HARDWARE_map_0;
	mlayers[2]=HARDWARE_map_2;
	mlayers[3]=HARDWARE_map_1;
	int li;
	for(li=0;li<4;li++)if(mlayers[li]==NULL)return;

	//origin in game px, floored to a MINIMAP_GPX multiple (>>2<<2 floors negatives too).
	//The map then steps in whole minimap px and the dot below lands dead-center.
	int qx=PLAYER_npc->MAP_x-(MINIMAP_PX/2)*MINIMAP_GPX;
	int qy=PLAYER_npc->MAP_y-(MINIMAP_PX/2)*MINIMAP_GPX;
	int ox=(qx>>2)<<2;
	int oy=(qy>>2)<<2;

	int bx,by;
	for(by=0;by<MINIMAP_PX;by++)
	{
		int wy=oy+by*MINIMAP_GPX;
		int ty=wy>=0?wy/8:-1; //floor-divide: C truncates toward zero
		int pyi=wy>=0?wy%8:-1;
		for(bx=0;bx<MINIMAP_PX;bx++)
		{
			int wx=ox+bx*MINIMAP_GPX;
			int tx=wx>=0?wx/8:-1;
			int pxi=wx>=0?wx%8:-1;
			int o=(by*MINIMAP_PX+bx)*4;
			unsigned char r=14,g=18,b=28; //out-of-map: dark slate
			if(tx>=0&&ty>=0&&tx<mw&&ty<mh)
			{
				int tii=pxi+pyi*8; //pixel index inside the 8x8 tile
				for(li=0;li<4;li++)
				{
					int tile=mlayers[li][tx+ty*mw];
					int pi=HARDWARE_map_tileset[64*tile+tii];
					if(pi!=0) //transparent pixels show the layer beneath
					{
						r=HARDWARE_MAP_PALETTE_ENHANCED_RGB[3*pi+0];
						g=HARDWARE_MAP_PALETTE_ENHANCED_RGB[3*pi+1];
						b=HARDWARE_MAP_PALETTE_ENHANCED_RGB[3*pi+2];
					}
				}
			}
			minimap_px[o+0]=r;
			minimap_px[o+1]=g;
			minimap_px[o+2]=b;
			minimap_px[o+3]=255;
		}
	}

	//player dot: 3x3 white. (MAP_x-ox) is always in [256,259], so this is (64,64).
	int pdx=(PLAYER_npc->MAP_x-ox)/MINIMAP_GPX;
	int pdy=(PLAYER_npc->MAP_y-oy)/MINIMAP_GPX;
	int dx,dy;
	for(dy=-1;dy<=1;dy++)for(dx=-1;dx<=1;dx++)
	{
		int x=pdx+dx,y=pdy+dy;
		if(x>=1&&y>=1&&x<MINIMAP_PX-1&&y<MINIMAP_PX-1)
		{
			int o=(y*MINIMAP_PX+x)*4;
			minimap_px[o+0]=255;minimap_px[o+1]=255;minimap_px[o+2]=255;
		}
	}

	//1px light-grey border
	int i;
	for(i=0;i<MINIMAP_PX;i++)
	{
		int t=i*4;
		int b=((MINIMAP_PX-1)*MINIMAP_PX+i)*4;
		int l=(i*MINIMAP_PX)*4;
		int rr=(i*MINIMAP_PX+MINIMAP_PX-1)*4;
		minimap_px[t+0]=160;minimap_px[t+1]=160;minimap_px[t+2]=160;
		minimap_px[b+0]=160;minimap_px[b+1]=160;minimap_px[b+2]=160;
		minimap_px[l+0]=160;minimap_px[l+1]=160;minimap_px[l+2]=160;
		minimap_px[rr+0]=160;minimap_px[rr+1]=160;minimap_px[rr+2]=160;
	}

	if(minimap_tex_id==0)
	{
		glGenTextures(1,&minimap_tex_id);
		glBindTexture(GL_TEXTURE_2D,minimap_tex_id);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,MINIMAP_PX,MINIMAP_PX,0,GL_RGBA,GL_UNSIGNED_BYTE,minimap_px);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,minimap_tex_id);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,MINIMAP_PX,MINIMAP_PX,GL_RGBA,GL_UNSIGNED_BYTE,minimap_px);
	}

	//128 is power-of-two so the struct draw needs no padding fixup
	texture_STRUCT mm;
	mm.image_w=MINIMAP_PX;mm.image_h=MINIMAP_PX;
	mm.texture_w=MINIMAP_PX;mm.texture_h=MINIMAP_PX;
	mm.texture_id=minimap_tex_id;
	draw_texture_struct(&mm,MINIMAP_X,MINIMAP_Y);

}
