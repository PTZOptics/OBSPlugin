//
//  HelperProtocol.h
//  PTZMacInstallHelper
//
//  Created by us01 on 11/4/22.
//

#ifndef HelperProtocol_h
#define HelperProtocol_h

static NSString *label = @"com.ptz.PTZMacInstallHelper";

@protocol HelperProtocol

- (void) copyToObs: (NSString*) dst from: (NSString*) src withCompletion: (void(^)(bool result))completion;

@end

#endif /* HelperProtocol_h */
