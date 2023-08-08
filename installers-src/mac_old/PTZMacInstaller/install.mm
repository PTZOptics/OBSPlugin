#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#import <Foundation/Foundation.h>
#include <string>
#include <iostream>
#include <dlfcn.h>

int dir_exists(const char *path)
{
    DIR *dir = opendir(path);
    int exists = dir != NULL;
    if (exists)
        closedir(dir);
    return exists;
}

std::string get_resources_path(const char *argv[])
{
    char absolute_resources_path[2560];
    int last_slash_index = strrchr(argv[0], '/') - argv[0];
    std::string exe_path(argv[0], last_slash_index);
    std::string resources_path = exe_path + "/../Resources";
    realpath(resources_path.c_str(), absolute_resources_path);
    return std::string(absolute_resources_path);
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

//typedef OSStatus (*TypeAuthExecuteWithPrivsFn)(AuthorizationRef authorization, const char *pathToTool, AuthorizationFlags options,
//                                    char * const *arguments, FILE **communicationsPipe);
//
//void merge_folders(std::string src, std::string dst)
////void get_admin_rights()
//{
//    AuthorizationRef authRef;
//    OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
//    if (status != errAuthorizationSuccess)
//        throw new std::exception();
//    AuthorizationItem authItem		= { kAuthorizationRightExecute, 0, NULL, 0 };
//    AuthorizationRights authRights	= { 1, &authItem };
//    AuthorizationFlags flags		=	kAuthorizationFlagDefaults				|
//                                         kAuthorizationFlagInteractionAllowed	|
//                                         kAuthorizationFlagPreAuthorize			|
//                                         kAuthorizationFlagExtendRights;
//
//
//    status = AuthorizationCopyRights(authRef, &authRights, kAuthorizationEmptyEnvironment, flags, NULL);
//    if (status != errAuthorizationSuccess)
//        throw new std::exception();
//
//    std::string src_path = src+"/";
//    char * args[] =
//    {
//            "-r",
//            (char*)src_path.c_str(),
//            (char*)dst.c_str(),
//            NULL
//    };
//
//    TypeAuthExecuteWithPrivsFn _AuthExecuteWithPrivsFn = (TypeAuthExecuteWithPrivsFn)dlsym(RTLD_DEFAULT, "AuthorizationExecuteWithPrivileges");
//    OSStatus res = _AuthExecuteWithPrivsFn (authRef, "/bin/cp", kAuthorizationFlagDefaults, args, NULL);
//    if (res != errAuthorizationSuccess)
//        throw new std::exception();
//}

void merge_folders(std::string src, std::string dst)
{
    NSTask *task = [[NSTask alloc] init];
    task.launchPath = @"/bin/cp";
    NSString *src_nss = [NSString stringWithCString:(src+"/").c_str() encoding:NSASCIIStringEncoding];
    NSString *dst_nss = [NSString stringWithCString:dst.c_str() encoding:NSASCIIStringEncoding];
    task.arguments = @[@"-r", src_nss, dst_nss];

    [task launch];
    [task waitUntilExit];
    if ([task terminationStatus]) throw new std::exception();
}

void copy_files(const char *argv[],  std::string folder)
{
    std::string resources_path = get_resources_path(argv);
//    get_admin_rights();
    merge_folders(resources_path + "/OBS.app", folder);
}

int install(const char *argv[])
{
    try
    {
        std::string obs_install_location = get_obs_install_location();
        copy_files(argv, obs_install_location);
    }
    catch(...)
    {
        return 0;
    }
    return 1;
}
