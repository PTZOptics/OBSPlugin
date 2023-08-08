//
//  install.m
//  PTZMacInstaller
//
//  Created by us01 on 11/4/22.
//

#import <Foundation/Foundation.h>
#include <dirent.h>
#include <string>
#import "HelperProtocol.h"
#import <ServiceManagement/ServiceManagement.h>

int dir_exists(const char *path)
{
    DIR *dir = opendir(path);
    int exists = dir != NULL;
    if (exists)
        closedir(dir);
    return exists;
}

std::string get_obs_install_location()
{
    std::string path = "/Applications/OBS.app";

    if (dir_exists(path.c_str()))
        return path;

    path = std::string(getenv("HOME")) + "/Applications/OBS.app";

    if (dir_exists(path.c_str()))
        return path;

    throw new std::exception();
}

std::string get_resources_path(const char *argv[])
{
    char absolute_resources_path[2560];
    long last_slash_index = strrchr(argv[0], '/') - argv[0];
    std::string exe_path(argv[0], last_slash_index);
    std::string resources_path = exe_path + "/../Resources";
    realpath(resources_path.c_str(), absolute_resources_path);
    return std::string(absolute_resources_path);
}

void install_helper(AuthorizationRef *authRef)
{
    OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, authRef);
    if (status != errAuthorizationSuccess) throw new std::exception();
    
    AuthorizationItem authItem1 = { kAuthorizationRightExecute, 0, NULL, 0 };
    AuthorizationItem authItem2 = { kSMRightModifySystemDaemons, 0, NULL, 0 };
    
    AuthorizationRights authRights    = {1, &authItem1 };
    AuthorizationRights authRights2    = {1, &authItem2 };
    AuthorizationFlags flags        =    kAuthorizationFlagDefaults                |
                                        kAuthorizationFlagInteractionAllowed    |
                                        kAuthorizationFlagPreAuthorize            |
                                        kAuthorizationFlagExtendRights;

    status = AuthorizationCopyRights(*authRef, &authRights, kAuthorizationEmptyEnvironment, flags, NULL);
    if (status != errAuthorizationSuccess) throw new std::exception();
    status = AuthorizationCopyRights(*authRef, &authRights2, kAuthorizationEmptyEnvironment, flags, NULL);
    if (status != errAuthorizationSuccess) throw new std::exception();
    
    
    if (!SMJobBless(kSMDomainSystemLaunchd, (__bridge CFStringRef)label, *authRef, NULL))
            throw new std::exception();
}

void merge_folders(std::string src, std::string dst)
{
    AuthorizationRef authRef;
    install_helper(&authRef);
    
    NSXPCConnection* _connection = [[NSXPCConnection alloc] initWithMachServiceName:label options:NSXPCConnectionPrivileged];
    _connection.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(HelperProtocol)];
    
    [_connection resume];
    NSString *src_ns = [NSString stringWithCString:src.c_str()
                                       encoding:[NSString defaultCStringEncoding]];
    NSString *dst_ns = [NSString stringWithCString:dst.c_str()
                                          encoding:[NSString defaultCStringEncoding]];
    
    id handler = [_connection synchronousRemoteObjectProxyWithErrorHandler: ^(NSError *error){}];

    __block bool result1;

    [handler copyToObs: dst_ns from: src_ns withCompletion:
         ^(bool result) {
            result1 = result;
         }
    ];
    
    SMJobRemove(kSMDomainSystemLaunchd, (__bridge CFStringRef)label, authRef, TRUE, NULL);

    if (!result1)
        throw new std::exception();
}

void copy_files(const char *argv[],  std::string folder)
{
    std::string resources_path = get_resources_path(argv);
    merge_folders(resources_path + "/OBS.app", folder);
}


extern "C"
int install(const char *argv[])
{
    try
    {
        std::string obs_install_location = get_obs_install_location();
        copy_files(argv, obs_install_location);
    }
    catch (...)
    {
        return 0;
    }
    return 1;
}

