//
//  Server.m
//  PTZMacInstallHelper
//
//  Created by us01 on 11/4/22.
//

#import <Foundation/Foundation.h>
#import "Server.h"
#import "HelperProtocol.h"
#import "PluginFiles.h"

@implementation Server

- (id) init
  {
    _listener = [[NSXPCListener alloc] initWithMachServiceName:label];
    _listener.delegate = self;

    [_listener resume];

    return self;
  }

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
    
    newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(HelperProtocol)];
    
    PluginFiles* pluginFiles = [[PluginFiles alloc] init];
    newConnection.exportedObject = pluginFiles;

    [newConnection resume];

    return YES;
}

@end

