#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>


#include <utilfile.h>

bool fas::utils::GetFileStat(const std::string& filename, struct stat *st) {
  int ret = ::lstat(filename.c_str(), st);
  if (ret == -1) {
    return false;
  }
  return true;
}

bool fas::utils::IsRegularFile(struct stat *st) {
  if (S_ISREG(st->st_mode)) {
    return true;
  }
  return false;
}

bool fas::utils::IsDirectory(struct stat *st) {
  if (S_ISDIR(st->st_mode)) {
    return true;
  }
  return false;
}

off_t fas::utils::FileSizeInBytes(struct stat *st) {
  return st->st_size;
}

bool fas::utils::WriteFile(const std::string& filename, unsigned char *buf, int len) {
    int fd = open(filename.c_str(), O_WRONLY|O_CREAT, 0777);
    if (fd < 0) {
        return false;
    }

    write(fd, buf, len);

    close(fd);
    
    return true;
}
