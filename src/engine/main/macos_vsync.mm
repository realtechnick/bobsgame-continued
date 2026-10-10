//PORT: native macOS vsync. SDL3 implements SDL_GL_SetSwapInterval in
//software on macOS (CVDisplayLink + condition variable) and it does not
//reliably produce 16.67ms swap blocks for interval 2 on 120Hz displays —
//frametimes showed interval-1 behavior (~12.4ms avg) with the 60Hz software
//gate drifting against the vsync grid. Set the GPU driver's swap interval
//directly on the NSOpenGLContext; the caller disables SDL's software wait
//(interval 0) so the two don't stack. Apple only.
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#include <SDL3/SDL.h>

extern "C" int PORT_macos_set_native_swap_interval(int interval)
{
	SDL_GLContext ctx=SDL_GL_GetCurrentContext();
	if(ctx==NULL)return 0;
	NSOpenGLContext* nsctx=(__bridge NSOpenGLContext*)ctx;
	if(nsctx==nil)return 0;
	GLint val=(GLint)interval;
	[nsctx setValues:&val forParameter:NSOpenGLContextParameterSwapInterval];
	return 1;
}
