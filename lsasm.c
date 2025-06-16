#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define COLOR_RESET   "\033[0m"     // Reset mau sac
#define COLOR_DIR     "\033[1;34m"  // Mau xanh cho thu muc
#define COLOR_LINK    "\033[1;36m"  // Mau xanh lam cho lien ket
#define COLOR_EXEC    "\033[1;32m"  // Mau xanh la cho file co quyen thuc thi

void print_file_info(const char *path, const char *filename) {
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filename);

    struct stat st;
    if (lstat(fullpath, &st) == -1) {
        perror("lstat");
        return;
    }

    // Kieu file
    printf((S_ISDIR(st.st_mode)) ? "d" :
           (S_ISLNK(st.st_mode)) ? "l" : "-");

    // Quyen truy cap
    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");

    // So lien ket
    printf(" %2ld", (long)st.st_nlink);

    // User va group
    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    printf(" %-8s %-8s", 
           pw ? pw->pw_name : "unknown",
           gr ? gr->gr_name : "unknown");

    // Size file
    printf(" %8ld", (long)st.st_size);

    // Thời gian
    char timebuf[64];
    struct tm *tm_info = localtime(&st.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    printf(" %s", timebuf);

    // Ten file
    if (S_ISDIR(st.st_mode)) {
        printf(" %s%s%s\n", COLOR_DIR, filename, COLOR_RESET);
    } else if (S_ISLNK(st.st_mode)) {
        printf(" %s%s%s\n", COLOR_LINK, filename, COLOR_RESET);
    } else if (st.st_mode & S_IXUSR) {
        printf(" %s%s%s\n", COLOR_EXEC, filename, COLOR_RESET);
    } else {
        printf(" %s\n", filename);
    }
}

int main(int argc, char *argv[]) {
    const char *path = ".";  // mac dinh la thu muc hien tai
    if (argc > 1) {
        path = argv[1];
    }

    DIR *dir = opendir(path);   
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Bo qua cac entry dac biet
        print_file_info(path, entry->d_name);
    }

    closedir(dir);
    return 0;
}
