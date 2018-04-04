/**
 * Copyright 2018 Quobyte Inc. All rights reserved.
 */

#include "quobyte.h"

#define _GNU_SOURCE

#include <assert.h>
#include <attr/xattr.h>
#include <errno.h>
#include <fcntl.h>
#include <libaio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>

static const char* kQuobyteTestPrefix = "quobyte-test-";
static const int kTestDirectoryMode = 0700;
static const int64_t kMillisecondsInNanoseconds = 1000000;
static const int kObjectSize = 8 * 1024 * 1024;

char* test_directory = NULL;

struct quobyte_fh {
  int fd;
};

struct quobyte_dh {
  DIR *dirp;
};

char* generate_test_path(const char* path) {
  assert(path != NULL);
  assert(strlen(path) > 0);
  assert(path[0] == '/');
  if (test_directory == NULL) {
    fprintf(stderr, "Call quobyte_create_adapter first\n");
    errno = ENOENT;
    return NULL;
  }
  char* result = calloc(1, strlen(test_directory) + strlen(path) + 1);
  strcat(result, test_directory);
  strcat(result, path);
  return result;
}

int quobyte_create_adapter(const char* registry_addresses) {
  assert(registry_addresses != NULL);
  assert(strlen(registry_addresses) > 0);

  char* test_dir = getenv("QUOBYTE_TESTDIR");
  if (test_dir == NULL) {
    test_dir = getenv("TMPDIR");
  }
  if (test_dir == NULL) {
    fprintf(stderr, "Cannot determine test directory. Set environment variable "
        "QUOBYTE_TESTDIR or TMPDIR\n");
    errno = ENOTDIR;
    return -1;
  }

  test_directory = calloc(1, strlen(test_dir) + strlen(kQuobyteTestPrefix) +
                          strlen(registry_addresses) + 2);
  strcat(test_directory, test_dir);
  strcat(test_directory, "/");
  strcat(test_directory, kQuobyteTestPrefix);
  strcat(test_directory, registry_addresses);

  if (mkdir(test_directory, kTestDirectoryMode) == -1 && errno != EEXIST) {
    free(test_directory);
    return -1;
  }
  printf("Set up test directory %s\n", test_directory);
  return 0;
}

int quobyte_destroy_adapter() {
  free(test_directory);
  return 0;
}

int quobyte_statvfs(const char* path, struct statvfs* buf) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = statvfs(mapped_path, buf);
  free(mapped_path);
  return res;
}

int quobyte_access(const char* path, int mode) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = access(mapped_path, mode);
  free(mapped_path);
  return res;
}

int quobyte_getattr(const char* path, struct stat* buf) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = stat(mapped_path, buf);
  free(mapped_path);
  return res;
}

int quobyte_chmod(const char* path, mode_t mode) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = chmod(mapped_path, mode);
  free(mapped_path);
  return res;
}

int quobyte_chown(const char* path, uid_t uid, gid_t gid) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = chown(mapped_path, uid, gid);
  free(mapped_path);
  return res;
}

int quobyte_utime(const char* path, struct utimbuf* buf) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = utime(mapped_path, buf);
  free(mapped_path);
  return res;
}

int quobyte_utimens(const char* path, const struct timespec tv[2]) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  struct timeval val[2];
  val[0].tv_sec  = tv[0].tv_sec;
  val[0].tv_usec = tv[0].tv_nsec / kMillisecondsInNanoseconds;
  val[1].tv_sec  = tv[1].tv_sec;
  val[1].tv_usec = tv[1].tv_nsec / kMillisecondsInNanoseconds;
  int res = utimes(mapped_path, val);
  free(mapped_path);
  return res;
}

int quobyte_link(const char* path, const char* newpath) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = link(mapped_path, newpath);
  free(mapped_path);
  return res;
}

int quobyte_readlink(const char* path, char* buf, size_t bufsize) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = readlink(mapped_path, buf, bufsize);
  free(mapped_path);
  return res;
}

int quobyte_rename(const char* path, const char* newpath) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = rename(mapped_path, newpath);
  free(mapped_path);
  return res;
}

int quobyte_symlink(const char* path, const char* newpath) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = symlink(mapped_path, newpath);
  free(mapped_path);
  return res;
}

int quobyte_truncate(const char* path, size_t size) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = truncate(mapped_path, size);
  free(mapped_path);
  return res;
}

int quobyte_unlink(const char* path) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = unlink(mapped_path);
  free(mapped_path);
  return res;
}

int quobyte_mkdir(const char* path, mode_t mode) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = mkdir(mapped_path, mode);
  free(mapped_path);
  return res;
}

int quobyte_rmdir(const char* path) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = rmdir(mapped_path);
  free(mapped_path);
  return res;
}

int quobyte_getxattr(const char* path, const char* name, char* buf,
                     size_t bufsize) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = getxattr(path, name, buf, bufsize);
  free(mapped_path);
  return res;
}

int quobyte_listxattr(const char* path, char* buf, size_t bufsize) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = listxattr(mapped_path, buf, bufsize);
  free(mapped_path);
  return res;
}

int quobyte_removexattr(const char* path, const char* name) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = removexattr(mapped_path, name);
  free(mapped_path);
  return res;
}

int quobyte_setxattr(const char* path, const char* name, const char* value,
                     size_t size, int flags) {
  char* mapped_path = generate_test_path(path);
  if (mapped_path == NULL) {
    return -1;
  }
  int res = setxattr(mapped_path, name, value, size, flags);
  free(mapped_path);
  return res;
}

struct quobyte_fh* quobyte_create(const char* path, mode_t mode) {
  char* test_path = generate_test_path(path);
  if (test_path == NULL) {
    return NULL;
  }
  int fd = creat(test_path, mode);
  struct quobyte_fh* fh = NULL;
  if (fd > 0) {
    fh = (struct quobyte_fh*)malloc(sizeof(struct quobyte_fh));
    if (fh != NULL) {
      fh->fd = fd;
    }
  }
  free(test_path);
  return fh;
}

struct quobyte_fh* quobyte_open(const char* path, int flags, int mode) {
  char* test_path = generate_test_path(path);
  if (test_path == NULL) {
    return NULL;
  }
  int fd = open(test_path, flags, mode);
  struct quobyte_fh* fh = NULL;
  if (fd > 0) {
    fh = (struct quobyte_fh*)malloc(sizeof(struct quobyte_fh));
    if (fh != NULL) {
      fh->fd = fd;
    }
  }
  free(test_path);
  return fh;
}

int quobyte_read(struct quobyte_fh* file_handle, char* data, size_t offset,
                 size_t length) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  int page_size = sysconf(_SC_PAGESIZE);
  char* aligned_buffer;
  if (posix_memalign((void**)&aligned_buffer, page_size, length) != 0) {
    return -1;
  }
  int bytes_read = pread(file_handle->fd, aligned_buffer, length, offset);
  memcpy(data, aligned_buffer, length);
  free(aligned_buffer);
  return bytes_read;
}

int quobyte_write(struct quobyte_fh* file_handle, const char* data,
                  size_t offset, size_t length, bool sync_write) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  int page_size = sysconf(_SC_PAGESIZE);
  char* aligned_buffer;
  if (posix_memalign((void**)&aligned_buffer, page_size, length) != 0) {
    return -1;
  }
  memcpy(aligned_buffer, data, length);
  int written = pwrite(file_handle->fd, aligned_buffer, length, offset);
  if (written > 0 && sync_write) {
    fsync(file_handle->fd);
  }
  free(aligned_buffer);
  return written;
}

int quobyte_fsync(struct quobyte_fh* file_handle) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return fsync(file_handle->fd);
}

int quobyte_lock(struct quobyte_fh* file_handle, int cmd, struct flock* flock) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return fcntl(file_handle->fd, cmd, flock);
}

int quobyte_ftruncate(struct quobyte_fh* file_handle, off_t new_file_size) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return ftruncate(file_handle->fd, new_file_size);
}

int quobyte_fallocate(struct quobyte_fh* file_handle, int mode, off_t offset,
                      off_t length) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return fallocate(file_handle->fd, mode, offset, length);
}

int quobyte_fstat(struct quobyte_fh* file_handle, struct stat* statbuf) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return fstat(file_handle->fd, statbuf);
}

size_t quobyte_get_allocated_bytes(struct quobyte_fh* file_handle) {
  struct stat statbuf;
  int res = quobyte_fstat(file_handle, &statbuf);
  if (res == 0) {
    return statbuf.st_blocks * statbuf.st_blksize;
  }
  return 0;
}

int quobyte_get_object_size(struct quobyte_fh* file_handle) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return kObjectSize;
}

int quobyte_close(struct quobyte_fh* file_handle) {
  if (file_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  int res = close(file_handle->fd);
  if (res == 0) {
    free(file_handle);
  }
  return res;
}

struct quobyte_dh* quobyte_opendir(const char* path) {
  char* test_path = generate_test_path(path);
  if (test_path == NULL) {
    return NULL;
  }
  struct quobyte_dh* dh = NULL;
  DIR* dirp = opendir(test_path);
  if (dirp != NULL) {
    dh = (struct quobyte_dh*)malloc(sizeof(struct quobyte_dh));
    if (dh != NULL) {
      dh->dirp = dirp;
    }
  }
  return dh;
}

uint64_t quobyte_telldir(struct quobyte_dh* dir_handle) {
  if (dir_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  return telldir(dir_handle->dirp);
}

void quobyte_seekdir(struct quobyte_dh* dir_handle, uint64_t offset) {
  if (dir_handle == NULL) {
    return;
  }
  return seekdir(dir_handle->dirp, offset);
}

struct dirent* quobyte_readdir(struct quobyte_dh* dir_handle) {
  if (dir_handle == NULL) {
    errno = EBADF;
    return NULL;
  }
  return readdir(dir_handle->dirp);
}

int quobyte_closedir(struct quobyte_dh* dir_handle) {
  if (dir_handle == NULL) {
    errno = EBADF;
    return -1;
  }
  int res = closedir(dir_handle->dirp);
  if (res == 0) {
    free(dir_handle);
  }
  return res;
}

int quobyte_aio_setup(int io_depth) {
  return 0;
}

int quobyte_aio_destroy(int io_context) {
  return 0;
}

int quobyte_aio_submit(int io_context, struct quobyte_iocb* io) {
  assert(false);
  return -ENOTSUP;
}

int quobyte_aio_submit_with_callback(
    int io_context,
    struct quobyte_iocb* io,
    void(cb)(void* data, int result),
    void* data) {
  if (io == NULL) {
    errno = EINVAL;
    return -1;
  }

  int res = -1;
  if (io->op_code == QB_READ) {
    res = quobyte_read(io->file_handle, io->buffer, io->offset, io->length);
  } else if (io->op_code == QB_WRITE) {
    res = quobyte_write(
        io->file_handle, io->buffer, io->offset, io->length, false);
  } else if (io->op_code == QB_WRITE_SYNC) {
    res = quobyte_write(
        io->file_handle, io->buffer, io->offset, io->length, true);
  } else if (io->op_code == QB_FSYNC) {
    res = quobyte_fsync(io->file_handle);
  } else if (io->op_code == QB_FALLOCATE) {
    res = quobyte_fallocate(io->file_handle, io->mode, io->offset, io->length);
  } else {
    errno = EINVAL;
    return -1;
  }
  cb(data, res);
  return 0;
}

int quobyte_get_number_of_storage_files(struct quobyte_fh* file_handle) {
  errno = ENOTSUP;
  return -1;
}

int quobyte_get_changed_objects(
    struct quobyte_fh* file_handle,
    uint64_t* minimal_object_version,
    uint64_t* current_file_version,
    int number_of_storage_files,
    char* bitmap,
    int bitmap_size_in_bytes) {
  errno = ENOTSUP;
  return -1;
}

void quobyte_set_process_name(const char* name) {
}
