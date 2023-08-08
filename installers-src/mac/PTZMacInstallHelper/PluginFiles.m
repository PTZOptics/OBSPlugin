//
//  PluginFiles.m
//  PTZMacInstallHelper
//
//  Created by us01 on 11/4/22.
//

#import <Foundation/Foundation.h>
#import "PluginFiles.h"

@implementation PluginFiles

- (void) copyToObs: (NSString*) dst from: (NSString*) src withCompletion: (void(^)(bool result))completion
{
    NSTask *task = [[NSTask alloc] init];
    task.launchPath = @"/bin/cp";
    task.arguments = @[@"-r", [src stringByAppendingString: @"/"], dst];

    [task launch];
    [task waitUntilExit];
    
    remove("/Library/LaunchDaemons/com.ptz.PTZMacInstallHelper.plist");
    remove("/Library/PrivilegedHelperTools/com.ptz.PTZMacInstallHelper");
    
    completion(![task terminationStatus]);
}
    
@end
