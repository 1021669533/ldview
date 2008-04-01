#import "LDViewController.h"
#import "ModelWindow.h"
#import "LDrawModelView.h"
#import "OCLocalStrings.h"
#import "OCUserDefaults.h"
#import "LDrawPage.h"
#import "Updater.h"
#include <LDLib/LDrawModelViewer.h>
#include <TRE/TREMainModel.h>
#include <TCFoundation/TCWebClient.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLoader/LDLModel.h>
#include "Preferences.h"

@implementation LDViewController

- (id)init
{
	if ((self = [super init]) != nil)
	{
		NSString *userAgent = [NSString stringWithFormat:@"LDView/%@ (Mac OSX; ldview@gmail.com; http://ldview.sf.net/)", [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"]];
		[OCLocalStrings loadStringTable:[[NSBundle mainBundle]
			pathForResource:@"LDViewMessages" ofType:@"ini"]];
		ldrawFileTypes = [[NSArray alloc] initWithObjects: @"ldr", @"dat", @"mpd",
			nil];
		TREMainModel::loadStudTexture([[[NSBundle mainBundle] pathForResource:
			@"StudLogo" ofType:@"png"] cStringUsingEncoding:NSASCIIStringEncoding]);
		modelWindows = [[NSMutableArray alloc] init];
		TCWebClient::setUserAgent([userAgent cStringUsingEncoding:NSASCIIStringEncoding]);
	}
	return self;
}

- (void)dealloc
{
	[modelWindows release];
	[preferences release];
	[statusBarMenuFormat release];
	[super dealloc];
}

- (NSArray *)modelWindows
{
	return modelWindows;
}

- (ModelWindow *)currentModelWindow
{
	return (ModelWindow *)[[[NSApplication sharedApplication] mainWindow]
		delegate];
}

- (LDrawModelView *)currentModelView
{
	return [[self currentModelWindow] modelView];
}

- (void)updateShowHideMenuItem:(NSMenuItem *)menuItem show:(BOOL)show format:(NSString *)format
{
	NSString *showHide;

	// Note: logic is backwards on purpose.  If show is set, then the menu needs
	// to say Hide, since the current state has the item shown.
	if (show)
	{
		showHide = [OCLocalStrings get:@"Hide"];
	}
	else
	{
		showHide = [OCLocalStrings get:@"Show"];
	}
	[menuItem setTitle:[NSString stringWithFormat:format, showHide]];
}

- (void)updateStatusBarMenuItem:(BOOL)showStatusBar
{
	[self updateShowHideMenuItem:statusBarMenuItem show:showStatusBar format:statusBarMenuFormat];
}

- (int)numberOfItemsInMenu:(NSMenu *)menu
{
	return [menu numberOfItems];
}

- (void)updateLatLongRotationMenuItem:(ModelWindow *)modelWindow
{
	bool examineLatLong = [modelWindow examineLatLong];
	bool flyThroughMode = [modelWindow flyThroughMode];

	if (examineLatLong)
	{
		[latLongRotationMenuItem setState:NSOnState];
	}
	else
	{
		[latLongRotationMenuItem setState:NSOffState];
	}
	if (flyThroughMode)
	{
		[latLongRotationMenuItem setEnabled:NO];
	}
	else
	{
		[latLongRotationMenuItem setEnabled:YES];
	}
}

- (void)updateViewModeMenuItems:(bool)flyThroughMode
{
	if (flyThroughMode)
	{
		[examineMenuItem setState:NSOffState];
		[flyThroughMenuItem setState:NSOnState];
		[latLongRotationMenuItem setEnabled:NO];
	}
	else
	{
		[examineMenuItem setState:NSOnState];
		[flyThroughMenuItem setState:NSOffState];
		[latLongRotationMenuItem setEnabled:YES];
	}
}

- (void)updateAlwaysOnTopMenuItem:(int)level
{
	if (level == NSNormalWindowLevel)
	{
		[alwaysOnTopMenuItem setState:NSOffState];
	}
	else
	{
		[alwaysOnTopMenuItem setState:NSOnState];
	}
}

- (BOOL)menu:(NSMenu *)menu updateItem:(NSMenuItem *)item atIndex:(int)index shouldCancel:(BOOL)shouldCancel
{
	ModelWindow *modelWindow = (ModelWindow *)[[NSApp mainWindow] delegate];

	if ([modelWindow isKindOfClass:[ModelWindow class]])
	{
		if (item == statusBarMenuItem)
		{
			[self updateStatusBarMenuItem:[modelWindow showStatusBar]];
		}
		else if (item == alwaysOnTopMenuItem)
		{
			[self updateAlwaysOnTopMenuItem:[[modelWindow window] level]];
		}
		else if (item == latLongRotationMenuItem)
		{
			[self updateLatLongRotationMenuItem:modelWindow];
		}
		else if (item == examineMenuItem)
		{
			[self updateViewModeMenuItems:[modelWindow flyThroughMode]];
		}
	}
	return YES;
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	if (menuItem == latLongRotationMenuItem)
	{
		ModelWindow *modelWindow = [[NSApp mainWindow] delegate];
		if ([modelWindow isKindOfClass:[ModelWindow class]] && ![modelWindow flyThroughMode])
		{
			return YES;
		}
		else
		{
			return NO;
		}
	}
	else if (menuItem == cancelMenuItem)
	{
		if ([[self currentModelWindow] loading])
		{
			return YES;
		}
		else
		{
			return NO;
		}
	}
	return YES;
}

- (void)awakeFromNib
{
	//showStatusBar = [OCUserDefaults longForKey:@"StatusBar" defaultValue:1 sessionSpecific:NO];
	statusBarMenuFormat = [[statusBarMenuItem title] retain];
	[self updateStatusBarMenuItem:YES];
}

- (BOOL)createWindow:(NSString *)filename
{
	ModelWindow *modelWindow = [[ModelWindow alloc] initWithController:self];

	if ([modelWindows count] > 0)
	{
		NSWindow *lastWindow = [[modelWindows lastObject] window];
		NSRect frame = [lastWindow frame];

		frame.origin.y += frame.size.height;
		frame.origin = [lastWindow cascadeTopLeftFromPoint:frame.origin];
		[[modelWindow window] setFrameTopLeftPoint:frame.origin];
	}
	[modelWindows addObject:modelWindow];
	[modelWindow release];
	[[self preferences] initModelWindow:modelWindow];
	[modelWindow show];
	if (filename)
	{
		return [modelWindow openModel:filename];
	}
	else
	{
		return NO;
	}
}

- (IBAction)newWindow:(id)sender
{
	[self createWindow:nil];
}

- (void)recordRecentFile:(NSString *)filename
{
	[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[NSURL fileURLWithPath:filename]];
}

- (BOOL)browseForLDrawDir
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];

	[openPanel setAllowsMultipleSelection:NO];
	[openPanel setCanChooseFiles:NO];
	[openPanel setCanChooseDirectories:YES];
	[openPanel setMessage:[OCLocalStrings get:@"SelectLDrawFolder"]];
	if ([openPanel runModalForDirectory:nil file:nil] == NSOKButton)
	{
		if ([self verifyLDrawDir:[openPanel filename] prompt:NO])
		{
			[[[self preferences] ldrawPage] updateLDrawDir:[openPanel filename]];
			return YES;
		}
		else
		{
			if (NSRunCriticalAlertPanel([OCLocalStrings get:@"Error"], [OCLocalStrings get:@"LDrawNotInFolder"], [OCLocalStrings get:@"Yes"], [OCLocalStrings get:@"No"], nil) == NSOKButton)
			{
				return [self browseForLDrawDir];
			}
		}
	}
	//NSRunCriticalAlertPanel([OCLocalStrings get:@"Error"], [OCLocalStrings get:@"LDrawFolderRequired"], [OCLocalStrings get:@"OK"], nil, nil);
	return NO;
}

- (BOOL)verifyLDrawDir
{
	if (!TCUserDefaults::boolForKey(VERIFY_LDRAW_DIR_KEY, true, false))
	{
		return YES;
	}
	return [self verifyLDrawDir:[[[self preferences] ldrawPage] ldrawDir] prompt:YES];
}

- (BOOL)checkForUpdates:(NSString *)parentDir full:(bool)full
{
	Updater *updater = [[Updater alloc] init];

	if ((full && [updater downloadLDraw:parentDir]) || (!full && [updater checkForUpdates:parentDir]))
	{
		[updater release];
		return YES;
	}
	else
	{
		[updater release];
		return NO;
	}
}

- (BOOL)downloadLDraw
{
	NSString *parentDir = [NSHomeDirectory() stringByAppendingPathComponent:@"Library"];
	
	if ([self checkForUpdates:parentDir full:true])
	{
		NSString *ldrawDir = [parentDir stringByAppendingPathComponent:@"ldraw"];
		[[[self preferences] ldrawPage] updateLDrawDir:ldrawDir];
		NSRunAlertPanel([OCLocalStrings get:@"LDrawInstalled"], [NSString stringWithFormat:[OCLocalStrings get:@"LDrawInstalledFormat"], ldrawDir], [OCLocalStrings get:@"OK"], nil, nil);
		return YES;
	}
	else
	{
		return NO;
	}
}

- (IBAction)checkForLibraryUpdates:(id)sender
{
	NSString *ldrawDir = [[[self preferences] ldrawPage] ldrawDir];
	
	if ([self verifyLDrawDir:ldrawDir prompt:NO])
	{
		NSString *lastPathComponent = [ldrawDir lastPathComponent];
		if ([lastPathComponent caseInsensitiveCompare:@"ldraw"] == NSOrderedSame)
		{
			[self checkForUpdates:[ldrawDir stringByDeletingLastPathComponent] full:false];
		}
		else
		{
			NSRunAlertPanel([OCLocalStrings get:@"CannotUpdate"], [NSString stringWithFormat:[OCLocalStrings get:@"AutoUpdatesBadFolder"], lastPathComponent], [OCLocalStrings get:@"OK"], nil, nil);
		}
	}
	else
	{
		[self downloadLDraw];
	}
}

- (BOOL)verifyLDrawDir:(NSString *)ldrawDir prompt:(BOOL)prompt
{
	if (![LDrawPage verifyLDrawDir:ldrawDir])
	{
		if (prompt)
		{
			BOOL retValue = NO;

			switch (NSRunAlertPanel([OCLocalStrings get:@"LDrawFolderNotFoundHeader"], [OCLocalStrings get:@"LDrawFolderNotFound"], [OCLocalStrings get:@"BrowseToLDrawFolder"], [OCLocalStrings get:@"DownloadFromLDrawOrg"], [OCLocalStrings get:@"Cancel"]))
			{
				case NSAlertDefaultReturn:
					retValue = [self browseForLDrawDir];
					break;
				case NSAlertAlternateReturn:
					retValue = [self downloadLDraw];
					break;
				case NSAlertOtherReturn:
					break;
			}
			if (!retValue)
			{
				NSRunCriticalAlertPanel([OCLocalStrings get:@"Error"], [OCLocalStrings get:@"LDrawFolderRequired"], [OCLocalStrings get:@"OK"], nil, nil);
			}
			return retValue;
		}
		else
		{
			return NO;
		}
	}
	return YES;
}

- (BOOL)openFile:(NSString *)filename
{
	if (![self verifyLDrawDir])
	{
		return NO;
	}
	if (![[NSApplication sharedApplication] mainWindow])
	{
		if ([self createWindow:filename])
		{
			[self recordRecentFile:filename];
			return YES;
		}
	}
	else
	{
		if ([[self currentModelWindow] openModel:filename])
		{
			[self recordRecentFile:filename];
			return YES;
		}
	}
	return NO;
}

- (void)openModel
{
	if ([self verifyLDrawDir])
	{
		NSOpenPanel *openPanel = [NSOpenPanel openPanel];

		[openPanel setMessage:[OCLocalStrings get:@"SelectModelFile"]];
		if ([openPanel runModalForTypes:ldrawFileTypes] == NSOKButton)
		{
			[self openFile:[openPanel filename]];
		}
	}
}

- (IBAction)openModel:(id)sender
{
	[self openModel];
}

- (Preferences *)preferences
{
	if (!preferences)
	{
		preferences = [[Preferences alloc] initWithController:self];
	}
	return preferences;
}

- (IBAction)preferences:(id)sender
{
	[[self preferences] show];
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	if ([self openFile:filename])
	{
		launchFileOpened = YES;
		return YES;
	}
	else
	{
		return NO;
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	if (!launchFileOpened)
	{
		[self openModel];
	}
}

- (IBAction)resetView:(id)sender
{
	[[self currentModelView] resetView:sender];
}

/*
- (IBAction)toggleStatusBar:(id)sender
{
	showStatusBar = !showStatusBar;
	[self updateStatusBarMenuItem];
	[OCUserDefaults setLong:showStatusBar forKey:@"StatusBar" sessionSpecific:NO];
	for (int i = 0; i < [modelWindows count]; i++)
	{
		[[modelWindows objectAtIndex:i] setShowStatusBar:showStatusBar];
	}
}
*/

- (BOOL)acceptsFirstResponder
{
	return NO;
}

- (BOOL)becomeFirstResponder
{
	return NO;
}

- (void)keyUp:(NSEvent *)theEvent
{
	unichar character = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	
	if (character >= '0' && character <= '9')
	{
		if (([theEvent modifierFlags] & (NSShiftKeyMask | NSControlKeyMask | NSAlternateKeyMask | NSCommandKeyMask)) == NSControlKeyMask)
		{
			[preferences hotKeyPressed:character - '0'];
			[modelWindows makeObjectsPerformSelector:@selector(reloadNeeded)];
			//NSLog(@"Ctrl+%d pressed.\n", character - '0');
		}
	}
	[super keyUp:theEvent];
}

- (BOOL)performKeyEquivalent:(NSEvent *)theEvent
{
	return [super performKeyEquivalent:theEvent];
}

- (NSMenu *)viewingAngleMenu
{
	return viewingAngleMenu;
}

@end
