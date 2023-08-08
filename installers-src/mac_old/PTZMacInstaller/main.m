#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

int install(const char *argv[]);

int main(int argc, const char * argv[])
{
    int res = install(argv);

    @autoreleasepool {}
    NSAlert *alert = [[NSAlert alloc] init];
    NSImageName imageName = res ? NSImageNameInfo : NSImageNameCaution;
    NSImage * icon = [NSImage imageNamed:imageName];
    [alert setIcon: icon];
    [alert setMessageText: res ? @"Installed" : @"Install failed"];
    [alert runModal];

    return 0;
}
