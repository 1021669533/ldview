//
//  ToolbarPopUpButton.m
//  LDView
//
//  Created by Travis Cobbs on 8/12/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "ToolbarPopUpButton.h"
#import "ToolbarPopUpButtonCell.h"

@implementation ToolbarPopUpButton

+ (void)initialize
{
	[[self class] setCellClass:[ToolbarPopUpButtonCell class]];
}

+ (Class)cellClass
{
	return [ToolbarPopUpButtonCell class];
}

- (id)initWithTemplate:(id)other
{
	//NSPopUpButtonCell *otherCell = [other cell];
	ToolbarPopUpButtonCell *cell;

	[super initWithFrame:[other frame]];
	cell = [self cell];
	[self setTarget:[other target]];
	[self setAction:[other action]];
	[self setMenu:[other menu]];
	[self setEnabled:[other isEnabled]];
	[self setPullsDown:[other pullsDown]];
	return self;
}

@end
