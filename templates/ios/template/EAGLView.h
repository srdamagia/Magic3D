/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2015 by MagicTech.
 
 @Platform     ANSI compatible
 ******************************************************************************/
/*
 Magic3D Engine
 Copyright (c) 2008-2015
 Thiago C. Moraes
 http://www.magictech.com.br
 
 This software is provided 'as-is', without any express or implied warranty.
 In no event will the authors be held liable for any damages arising from the use of this software.
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it freely,
 subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
 If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.
 */

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <CoreMotion/CoreMotion.h>

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#include "Game.h"

#define MAX_SIMULTANEOUS_TOUCHES 5

/*
 This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 The view content is basically an EAGL surface you render your OpenGL scene into.
 Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
@interface EAGLView : UIView {
	
@private
	/* The pixel dimensions of the backbuffer */
	GLint backingWidth;
	GLint backingHeight;
	
	EAGLContext* context;
    CMMotionManager* motionManager;
	
	/* OpenGL names for the renderbuffer and framebuffers used to render to this view */
	GLuint viewRenderbuffer;
    GLuint viewFramebuffer;
	
	/* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
	GLuint depthRenderbuffer;
	
	//NSTimer *animationTimer;
    CADisplayLink* displayLink;
	NSTimeInterval animationInterval;
    
    UITouch* finger[MAX_SIMULTANEOUS_TOUCHES];
}

@property (nonatomic, assign) NSTimeInterval animationInterval;

- (void)startAnimation;
- (void)stopAnimation;
- (void)draw;

- (void) startDeviceMotion;
- (void) stopDeviceMotion;

@end
