//
//  main.m
//  PTZMacInstaller
//
//  Created by us01 on 11/4/22.
//

#import <Cocoa/Cocoa.h>

int install(const char *argv[]);

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
    }
    
    int res = install(argv);
    
    NSAlert *alert = [[NSAlert alloc] init];
    NSImageName imageName = res ? NSImageNameInfo : NSImageNameCaution;
    NSImage * icon = [NSImage imageNamed:imageName];
    [alert setIcon: icon];
    [alert setMessageText: res ? @"Installed" : @"Install failed"];
    [alert runModal];
    
    return 0;
}
