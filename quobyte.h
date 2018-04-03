/**
 * Copyright 2018 Quobyte Inc. All rights reserved.
 */
#ifndef QUOBYTE_H_
#define QUOBYTE_H_

#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <time.h>
#include <utime.h>

#ifdef __cplusplus
extern "C" {
#endif

struct quobyte_fh;
struct quobyte_dh;

enum quobyte_async_io_op {
  QB_READ, QB_WRITE, QB_FSYNC, QB_WRITE_SYNC, QB_FALLOCATE
};

struct quobyte_iocb {
  enum quobyte_async_io_op op_code;
  int io_context;
  struct quobyte_fh* file_handle;
  char* buffer;
  uint64_t offset;
  uint64_t length;
  int mode;  // used for fallocate
};

struct quobyte_io_event {
  struct quobyte_iocb* iocb;
  int errorcode;
  int result;
};

/**
 * Create the Quobyte adapter
 *
 * Not thread-safe. Only one adapter can be created. Only call once.
 *
 * @param registry_addresses A comma separated list of registry addresses.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_create_adapter(const char* registry_addresses);

/**
 * Shutdown and destroy the Quobyte adapter.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_destroy_adapter(void);

/**
 * Retrieves information about a file system.
 *
 * See man page statvfs(2).
 *
 * @param path The pathname of an file within the file system.
 * @param statv A pointer to a statvfs structure.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_statvfs(const char* path, struct statvfs *statv);

/**
 * Check whether the calling process can access the given file.
 *
 * @param path The pathname of the file to be checked.
 * @param mode Bitwise OR of F_OK, R_OK, W_OK, X_OK. See man page access(2).
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_access(const char* path, int mode);

/**
 * Retrieve information about a file.
 *
 * @param path The pathname of the file
 * @param statbuf A pointer to a stat struncture. See man page stat(2).
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_getattr(const char* path, struct stat* statbuf);

/**
 * Change the permissions of a file.
 *
 * See man page chmod(2).
 *
 * @param path The pathname of the file.
 * @param mode Bitmask of permissions to set. See man page chmod(2).
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_chmod(const char* path, mode_t mode);

/**
 * Change the owner and group of a file.
 *
 * See man page chown(2).
 *
 * @param path The pathname of the file.
 * @param owner New owner ID. Set to -1 if no change is wanted.
 * @param group New group ID. Set to -1 if no change is wanted.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_chown(const char* path, uid_t owner, gid_t group);

/**
 * Change the access and modification times of a file with resolution of one
 * second.
 *
 * See man page utime(2).
 *
 * @param path The pathname of the file.
 * @param ubuf A pointer to a utimbuf structure. Set to NULL for to set the
 *             current time.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_utime(const char* path, struct utimbuf* ubuf);

/**
 * Change the access and modification times of a file with resolution of one
 * nanosecond.
 *
 * See man page utime(2).
 *
 * @param path The pathname of the file.
 * @param tv An array of timespec structures. Set to NULL for to set the
 *           current time. tv[0] specifies the new access time, and tv[1]
 *           specifies the new modification time.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_utimens(const char* path, const struct timespec tv[2]);

/**
 * Create a new link to an existing file.
 *
 * See man page link(2).
 *
 * @param path The pathname of the file.
 * @param newpath The pathname of the new link.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_link(const char* path, const char* newpath);

/**
 * Read the contents of a symbolic link.
 *
 * See man page readlink(2).
 *
 * @param path The pathname of the symlink.
 * @param link The contents of the link.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_readlink(const char* path, char* link, size_t size);

/**
 * Renames a file, moving it between directories if required.
 *
 * See man page rename(2).
 *
 * @param path The pathname of the file to be renamed.
 * @param newpath The new pathname.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_rename(const char* path, const char* newpath);

/**
 * Create a symbolik link.
 *
 * See man page symlink(2).
 *
 * @parm path The content of the symbolic link.
 * @param link The pathname of the link to be created.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_symlink(const char* path, const char* link);


/**
 * Create a file.
 *
 * See man page create(2).
 *
 * @param path The pathname of the file to be created.
 * @param mode The permissions of the new file.
 * @param file_handle A pointer
 * @return On success, a pointer to a file handle is returned. On error, NULL
 *         is returned and errno is set appropriately.
 *
 */
struct quobyte_fh* quobyte_create(const char* path, mode_t mode);

/**
 * Open a file.
 *
 * See man page open(2).
 *
 * @param path The pathname of the file to be created.
 * @param flags Open flags like specified in man page open(2).
 * @param mode The permissions of the new file.
 * @return On success, a pointer to a file handle is returned. On error, NULL
 *         is returned and errno is set appropriately.
 *
 */
struct quobyte_fh* quobyte_open(const char* path, int flags, int mode);

/**
 * Read a file.
 *
 * @param file_handle The file handle to read from.
 * @param data A pointer to a buffer to read to.
 * @param offset The offset in the file specifying where to read.
 * @param length The maximum number of bytes to read.
 * @return The number of bytes read. Reading less then length indicates end of
 *         file.
 */
int quobyte_read(struct quobyte_fh* file_handle, char* data, size_t offset,
                 size_t length);

/**
 * Write a file.
 *
 * @param file_handle The file handle to write to.
 * @param data A pointer to a buffer of data to be written.
 * @param offset The offset in the file specifying where to write.
 * @param length The number of bytes to write.
 * @param sync_write Whether to sync the written data to disk.
 * @return The number of bytes written.
 */
int quobyte_write(struct quobyte_fh* file_handle, const char* data,
                  size_t offset, size_t length, bool sync_write);
/**
 * Sync all caches to disk on data services.
 *
 * @param file_handle The file handle to sync.
 */
int quobyte_fsync(struct quobyte_fh* file_handle);

/**
 * Get or set a lock on a file.
 *
 * See advisory locking section in man page fcntl(2).
 *
 * @param file_handle The file handle.
 * @param cmd F_GETLK, F_SETLK or S_FETLKW.
 * @param flock A pointer to a flock structure.
 * @return On success, a pointer to a file handle is returned. On error, NULL
 *         is returned and errno is set appropriately.
 */
int quobyte_lock(struct quobyte_fh* file_handle, int cmd, struct flock* flock);

/**
 * Truncate a file
 *
 * See man page truncate(2).
 *
 * @param path The pathname of the file
 * @param size The new file size
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_truncate(const char* path, size_t size);

/**
 * Truncate an open file
 *
 * See man page truncate(2).
 *
 * @param file_handle The file_handle of the file
 * @param size The new file size
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_ftruncate(struct quobyte_fh* file_handle, off_t new_file_size);

/**
 * Fallocate a file
 *
 * See man page fallocate(2)
 */
int quobyte_fallocate(struct quobyte_fh* file_handle, int mode, off_t offset,
                      off_t length);

/**
 * Retrieve information about an open file.
 *
 * @param file_handle The file_handle of the file
 * @param statbuf A pointer to a stat struncture. See man page stat(2).
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_fstat(struct quobyte_fh* file_handle, struct stat* statbuf);


/**
 * Retrieve the number of allocated bytes of a file
 *
 * This function returns the number of allocated bytes that is cached in the
 * client.
 *
 */
size_t quobyte_get_allocated_bytes(struct quobyte_fh* file_handle);

/**
 * Close the file handle.
 *
 * @param file_handle The file handle to close.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_close(struct quobyte_fh* file_handle);

/**
 * Delete a name from the file system.
 *
 * See man page unlink(2).
 *
 * @param path The name to be deleted.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_unlink(const char* path);

/**
 * Create a directory.
 *
 * @param path The pathname of the directory.
 * @param mode The permissions of the new directory.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_mkdir(const char* path, mode_t mode);

/**
 * Open a directory stream
 *
 * @param path The pathname of the directory.
 * @return On success, a pointer to a directory handle is returned. On error,
 *         NULL is returned and errno is set appropriately.
 *
 */
struct quobyte_dh* quobyte_opendir(const char* path);

/**
 * Retrieve the current location in the directory stream.
 *
 * @param dir_handle The directory handle.
 * @return The current location in the directory stream-
 */
uint64_t quobyte_telldir(struct quobyte_dh* dir_handle);

/**
 * Set the location in the directory stream.
 *
 * @param dir_handle The directory handle.
 * @param offset The location to set.
 */
void quobyte_seekdir(struct quobyte_dh* dir_handle, uint64_t offset);

/**
 * Consume the next directory entry.
 *
 * @param dir_handle The directory handle
 * @return A pointer to a dirent structure describing the next directory entry.
 *         If the end of the directory stream is reached, NULL is returned and
 *         errno is not changed. If an error occurs, NULL is returned and errno
 *         is set appropriately.
 */
struct dirent* quobyte_readdir(struct quobyte_dh* dir_handle);

/**
 * Close a directory stream.
 *
 * @param dir_handle The directory handle
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_closedir(struct quobyte_dh* dir_handle);

/**
 * Delete an empty directory.
 *
 * @param path The pathname of the directory.
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_rmdir(const char* path);

/**
 * Retrieve an extended attribute
 *
 * See man page getxattr(2).
 *
 * @param path The pathname of the file
 * @param name The name of the extended attribute
 * @param value A pointer to a data buffer to read to
 * @param size The size of the buffer.
 * @return On success, the size of the extended attribute value is returned.
 *         On error, -1 is returned and errno is set appropriately.
 */
int quobyte_getxattr(const char* path, const char* name, char* value,
                     size_t size);

/**
 * Retrieve a list of extended attribute names
 *
 * The list is the set of (NULL-terminated) names, one after the other.
 * See man page listxattrs(2).
 *
 * @param path The pathname of the file
 * @param list A pointer to a buffer to store the list
 * @param size The size of the buffer.
 * @return On success, the size of the list is returned.
 *         On error, -1 is returned and errno is set appropriately.
 */
int quobyte_listxattr(const char* path, char* list, size_t size);

/**
 * Remove an extended attribute.
 *
 * See man page removexattr(2).
 *
 * @param path The pathname of the file.
 * @param name The name of the extended attribute to remove.
 */
int quobyte_removexattr(const char* path, const char* name);

/**
 * Set an extended attribute.
 *
 * @param path The pathname of the file.
 * @param name The name of the extended attribute to set
 * @param value The value of the extended attribute
 * @param size The size of the value to set
 * @param flags 0, XATTR_CREATE or XATTR_REPLACE. See man page setxattr(2).
 */
int quobyte_setxattr(const char* path, const char* name, const char* value,
                     size_t size, int flags);

/**
 * Set up an context for asynchronous IO.
 *
 * @param io_depth The number of concurrent IO request
 * @return On success, the ID of the IO context is returned. On error, -1 is
 *         returned and errno is set appropriately.
 */
int quobyte_aio_setup(int io_depth);

/**
 * Submit an IO request
 *
 * Please note that QB_FSYNC is executed without any synchronization of
 * concurrent IO. It is subject to the caller to establish a barrier for
 * QB_FSYNC.
 *
 * @param io_context The ID of the IO context
 * @param io The io request
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_aio_submit(int io_context, struct quobyte_iocb* io);

/**
 * Submit an IO request with a completion callback
 *
 * Please note that QB_FSYNC is executed without any synchronization of
 * concurrent IO. It is subject to the caller to establish a barrier for
 * QB_FSYNC.
 *
 * @param io_context The ID of the IO context
 * @param io The io request
 * @param cb A pointer to a callback function
 * @param user_data first argument for the callback
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_aio_submit_with_callback(
    int io_context, struct quobyte_iocb* io, void(cb)(void* data, int result),
    void* data);

/**
 * Wait for IO requests to complete
 *
 * @param io_context The ID of the IO context
 * @param min The minimum number of requests to wait for
 * @param max The maximum number of requests to wait for
 * @param events
 * @param timeout (currently ignored)
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_aio_getevents(
    int io_context, int min, int max, struct quobyte_io_event* events,
    struct timespec* timeout);

/**
 * Destroy an IO context
 *
 * @param io_context The ID of the IO context to destroy
 * @return On success, 0 is returned. On error, -1 is returned and errno is
 *         set appropriately.
 */
int quobyte_aio_destroy(int io_context);

/**
 * Retrieve the object size of a open file
 *
 * @param file_handle the file
 * @return On success, the object size in bytes is returned
 *         On error, -1 is returned and errno is set appropriately.
 */
int quobyte_get_object_size(struct quobyte_fh* file_handle);

/**
 * Retrieve the number of storage files the logical file is split into
 *
 * @param file_handle the file
 * @return On success, the number of storage files
 *         On error, -1 is returned and errno is set appropriately.
 */
int quobyte_get_number_of_storage_files(struct quobyte_fh* file_handle);

/**
 * Retrieve which objects have changed since a given version
 *
 * @param file_handle the file
 * @param minimal_object_version a vector of versions to list all objects per
 *        storage file larger than this
 * @param current_file_version return a vector of the current largest object
 *        version per storage file
 * @param number_of_storage_files the vectors are allocated for
 * @param bitmap the bitmap which contains a set bit per changed object
 * @param bitmap_size_in_bytes the size of the bitmap in bytes
 * @return On success, the number of objects
 *         On error, -1 is returned and errno is set appropriately.
 */
int quobyte_get_changed_objects(
    struct quobyte_fh* file_handle,
    uint64_t* minimal_object_version,
    uint64_t* current_file_version,
    int number_of_storage_files,
    char* bitmap,
    int bitmap_size_in_bytes);

/**
 * Sets a custom process name
 *
 * Must be called before the adapter is created.
 *
 * @param name the new process name. The buffer is copied.
 *
 */
void quobyte_set_process_name(const char* name);

#ifdef __cplusplus
}
#endif

#endif /* QUOBYTE_H_ */
