//
// Created by hariharan on 2/21/22.
//

#include <athena/api/interceptor.h>

extern const char* kPathExclusions[15] = {
    "/bin/", "/boot/", "/dev/",  "/etc/",   "/lib/",
    "/opt/", "/proc/", "/sbin/", "/sys/",   "/usr/",
    "/var/", "/run/",  "pipe",   "socket:", "anon_inode:"};

extern const char* kExtensionExclusions[1] = {"conf"};
extern std::vector<std::string> track_files = std::vector<std::string>();