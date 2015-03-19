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

#import "AppDelegate.h"
#import "ViewController.h"

@implementation ViewController

@synthesize glView;

- (void)loadView 
{
    CGRect bounds = [UIScreen mainScreen].bounds;
    bounds.size.width *= [UIScreen mainScreen].scale;
    bounds.size.height *= [UIScreen mainScreen].scale;
    
    self.view = [[UIView alloc] initWithFrame:bounds];
    self.view.autoresizesSubviews = TRUE;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    CGRect bounds = [UIScreen mainScreen].bounds;
    bounds.size.width *= [UIScreen mainScreen].scale;
    bounds.size.height *= [UIScreen mainScreen].scale;
    
    glView = [[EAGLView alloc] initWithFrame:bounds];
    glView.userInteractionEnabled=YES;
    [self.view addSubview:glView];
    
    NSString *filePath = [[NSBundle mainBundle] resourcePath];
    NSString *userPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    /*if (![[NSFileManager defaultManager] fileExistsAtPath:dataPath])
    {
        [[NSFileManager defaultManager] createDirectoryAtPath:dataPath withIntermediateDirectories:NO attributes:nil error:&error];
    }*/
    
    Game::start(std::string(filePath.UTF8String), std::string(userPath.UTF8String), glView.bounds.size.width, glView.bounds.size.height);
    
    [glView startAnimation];
    
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(orientationChanged:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil];
}

-(BOOL)shouldAutorotate
{
    return YES;
}

-(NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}

- (void)viewDidLayoutSubviews
{
    Magic3D::Renderer::getInstance()->resizeWindow(glView.bounds.size.width * glView.contentScaleFactor, glView.bounds.size.height * glView.contentScaleFactor);
}

- (void)orientationChanged:(NSNotification *)notification
{
    
}

- (void)pause
{
    Magic3D::Magic3D::pause();
    [glView stopAnimation];
}

- (void)stop
{
    Game::finish();
    [glView stopAnimation];
}

- (void)play
{
    if (!Magic3D::Scene::getInstance()->isLoading())
    {
        Magic3D::Magic3D::play();
    }
    [glView startAnimation];
}

@end