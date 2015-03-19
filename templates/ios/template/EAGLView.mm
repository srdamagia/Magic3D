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

#import <OpenGLES/EAGLDrawable.h>

// A class extension to declare private methods
#import "EAGLView.h"


// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
//@property (nonatomic, assign) NSTimer *animationTimer;
@property (nonatomic, assign) CADisplayLink *displayLink;
@property (nonatomic, retain) CMMotionManager* motionManager;


- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLView

@synthesize context;
@synthesize animationInterval;
//@synthesize animationTimer;
@synthesize displayLink;
@synthesize motionManager;
 
 + (Class)layerClass {
     return [CAEAGLLayer class];
 }

- (id)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.frame = frame;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys : [NSNumber numberWithBool : NO], 
                                        kEAGLDrawablePropertyRetainedBacking, 
                                        kEAGLColorFormatRGBA8,
                                        //kEAGLColorFormatRGB565, //Faster but gradient problems
                                        kEAGLDrawablePropertyColorFormat, nil];
        
        
        context = [[EAGLContext alloc] initWithAPI : kEAGLRenderingAPIOpenGLES2];
        
        if (!context || ![EAGLContext setCurrentContext : context]) {
            [self release];
            return 0;
        }
        
        animationInterval = 1.0 / 60.0;
        
        [eaglLayer setContentsScale:[UIScreen mainScreen].scale];
        [self setContentScaleFactor:[UIScreen mainScreen].scale];
        [self setOpaque:YES];
        [self setMultipleTouchEnabled:YES];
        
        self.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
        self.autoresizesSubviews = YES;
        
        NSTimeInterval motionInterval = 1.0 / 30.0;
        self.motionManager = [[CMMotionManager alloc] init];
        self.motionManager.accelerometerUpdateInterval = motionInterval;
        self.motionManager.gyroUpdateInterval = motionInterval;
        self.motionManager.deviceMotionUpdateInterval = motionInterval;
        
        if([self.motionManager isDeviceMotionAvailable])
        {
            [self.motionManager startDeviceMotionUpdatesToQueue:[NSOperationQueue currentQueue]
                withHandler:^(CMDeviceMotion *deviceMotion, NSError *error) {
                    [self outputAttitude:deviceMotion];
                }];
        }
        else
        {
            if([self.motionManager isAccelerometerAvailable])
            {
                [self.motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                                         withHandler:^(CMAccelerometerData  *accelerometerData, NSError *error) {
                                                             [self outputAccelertionData:accelerometerData.acceleration];
                                                         }];
            }
            
            if([self.motionManager isGyroAvailable])
            {
                [self.motionManager startGyroUpdatesToQueue:[NSOperationQueue currentQueue]
                                                withHandler:^(CMGyroData *gyroData, NSError *error) {
                                                    [self outputRotationData:gyroData.rotationRate];
                                                }];
            }
        }
                
        for (int i = 0; i < MAX_SIMULTANEOUS_TOUCHES; i++)
        {
            finger[i] = NULL;
        }
    }
    return self;
}

- (BOOL)createFramebuffer {

	glGenFramebuffers(1, &viewFramebuffer);
	glGenRenderbuffers(1, &viewRenderbuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
	
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
	
	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, backingWidth, backingHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    [EAGLContext setCurrentContext:self.context];
    
    Magic3D::Renderer::getInstance()->setDefaultFBO(viewFramebuffer);
    
    return TRUE;
}

- (void)destroyFramebuffer {
	
	glDeleteFramebuffers(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffers(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	if (depthRenderbuffer)
    {
		glDeleteRenderbuffers(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}

- (void) startDeviceMotion {
    if([self.motionManager isDeviceMotionAvailable])
    {
        [self.motionManager startDeviceMotionUpdates];
    }
    else
    {
        if([self.motionManager isAccelerometerAvailable])
        {
            [self.motionManager startAccelerometerUpdates];
        }
        
        if([self.motionManager isGyroAvailable])
        {
            [self.motionManager startGyroUpdates];
        }
    }
}
- (void) stopDeviceMotion {
    if([self.motionManager isDeviceMotionAvailable])
    {
        [self.motionManager stopDeviceMotionUpdates];
    }
    else
    {
        if([self.motionManager isAccelerometerAvailable])
        {
            [self.motionManager stopAccelerometerUpdates];
        }
        
        if([self.motionManager isGyroAvailable])
        {
            [self.motionManager stopGyroUpdates];
        }
    }
}

- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
}

- (void)draw {
    
	Magic3D::Magic3D::run();
    
    if (!Magic3D::Scene::getInstance()->isLoading())
    {
        Game::getInstance()->AI();
    }
    
	glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	(void)event;

	for (UITouch *touch in touches) 
	{
        CGPoint p = [touch locationInView:self];
        p.x *= [UIScreen mainScreen].scale;
        p.y *= [UIScreen mainScreen].scale;

        for (int t = 0; t < MAX_SIMULTANEOUS_TOUCHES; t++)
        {
            if (finger[t] == NULL)
            {
                finger[t] = touch;
                
                Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_DOWN, (int)p.x, (int)p.y, t + 1);
                Magic3D::Renderer::getInstance()->getWindow()->setCursorPosition((int)p.x, (int)p.y);
                break;
            }
        }
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{  
	(void)event;
    
	for (UITouch *touch in touches)
	{
        CGPoint p = [touch locationInView:self];
        p.x *= [UIScreen mainScreen].scale;
        p.y *= [UIScreen mainScreen].scale;

        for (int t = 0; t < MAX_SIMULTANEOUS_TOUCHES; t++)
        {
            if (finger[t] == touch)
            {
                Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_MOVE, (int)p.x, (int)p.y, t + 1);
                Magic3D::Renderer::getInstance()->getWindow()->setCursorPosition((int)p.x, (int)p.y);
                break;
            }
        }
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	(void)event;
	
	for (UITouch *touch in touches)
	{
        CGPoint p = [touch locationInView:self];
        p.x *= [UIScreen mainScreen].scale;
        p.y *= [UIScreen mainScreen].scale;

        for (int t = 0; t < MAX_SIMULTANEOUS_TOUCHES; t++)
        {
            if (finger[t] == touch)
            {
                finger[t] = NULL;
                
                Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_UP, (int)p.x, (int)p.y, t + 1);
                Magic3D::Renderer::getInstance()->getWindow()->setCursorPosition((int)p.x, (int)p.y);
                break;
            }
        }
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    /*
     this can happen if the user puts more than 5 touches on the screen
     at once, or perhaps in other circumstances.  Usually (it seems)
     all active touches are canceled.
     */
    [self touchesEnded: touches withEvent: event];
}

-(void)outputAccelertionData:(CMAcceleration)acceleration
{
    Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_ACCELEROMETER, Magic3D::eEVENT_ACCELEROMETER, (float)acceleration.x, (float)acceleration.y, (float)acceleration.z);
}

-(void)outputRotationData:(CMRotationRate)rotation
{
    Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_GYROSCOPE, Magic3D::eEVENT_GYROSCOPE, (float)rotation.x, (float)rotation.y, (float)rotation.z);
}

-(void)outputAttitude:(CMDeviceMotion *)deviceMotion
{
    CMAttitude* attitude = deviceMotion.attitude;
    Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_GYROSCOPE, Magic3D::eEVENT_GYROSCOPE, (float)attitude.roll, (float)attitude.pitch, (float)attitude.yaw);
    
    CMAcceleration acceleration = deviceMotion.gravity;
    Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_ACCELEROMETER, Magic3D::eEVENT_ACCELEROMETER, (float)acceleration.x, (float)acceleration.y, (float)acceleration.z);
    
    //CMRotationRate rotation = deviceMotion.rotationRate;
    //Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_GYROSCOPE, Magic3D::eEVENT_GYROSCOPE, (float)rotation.x, (float)rotation.y, (float)rotation.z);
}

- (BOOL)canResignFirstResponder {
	return NO;
}

- (void)startAnimation {
	//self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(draw) userInfo:nil repeats:YES];
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}


- (void)stopAnimation {
	//self.animationTimer = nil;
    [self.displayLink invalidate];
    self.displayLink = nil;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
	
	animationInterval = interval;
	/*if (animationTimer) {
		[self stopAnimation];
		[self startAnimation];
	}*/
}


- (void)dealloc {
    [self stopDeviceMotion];
	[self stopAnimation];
	
	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
    
    [motionManager release];
	[context release];
    
	[super dealloc];
}

@end