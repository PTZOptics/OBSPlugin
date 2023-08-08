//
//  Server.h
//  PTZMacInstallHelper
//
//  Created by us01 on 11/4/22.
//

#ifndef Server_h
#define Server_h

@interface Server : NSObject <NSXPCListenerDelegate>

@property (nonatomic, strong, readwrite) NSXPCListener *listener;

@end

#endif /* Server_h */
