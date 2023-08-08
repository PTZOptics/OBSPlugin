//
//  PluginFiles.h
//  PTZMacInstallHelper
//
//  Created by us01 on 11/4/22.
//

#ifndef PluginFiles_h
#define PluginFiles_h

@interface PluginFiles : NSObject

- (void) copyToObs: (NSString*) dst from: (NSString*) src withCompletion: (void(^)(bool result))completion;

@end


#endif /* PluginFiles_h */
