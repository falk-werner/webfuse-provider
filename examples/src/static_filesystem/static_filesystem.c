#include "static_filesystem.h"

#include "webfuse/provider/client_config.h"
#include "webfuse/provider/dirbuffer.h"
#include "webfuse/provider/operation/error.h"

#include "path.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STATIC_FILESYSTEM_DEFAULT_CAPACITY (16)
#define STATIC_FILSYSTEM_INODE_ROOT (1)
#define STATIC_FILESYSTEM_MAX_READ_SIZE (4 * 1024)

struct static_filesystem_entry
{
    size_t inode;
    size_t parent;
    char * name;
    bool is_file;
    int mode;
    size_t size;
    char * content;
    static_filesystem_read_fn * read;
    static_filesystem_get_info_fn * get_info;
    void * user_data;
};

struct static_filesystem
{
    struct static_filesystem_entry * entries;
    size_t size;
    size_t capacity;
};

static struct static_filesystem_entry *
static_filesystem_get_entry(
    struct static_filesystem * filesystem,
    size_t inode)
{
    struct static_filesystem_entry * entry = NULL;

    if ((0 < inode) && (inode <= filesystem->size))
    {
        entry = &filesystem->entries[inode - 1];
    }
    
    return entry;
}

static struct static_filesystem_entry *
static_filesystem_get_entry_by_name(
    struct static_filesystem * filesystem,
    size_t parent,
    char const * name)
{
    struct static_filesystem_entry * entry = NULL;
    for(size_t i = 0; i < filesystem->size; i++)
    {
        struct static_filesystem_entry * current = &filesystem->entries[i];
        if ((parent == current->parent) && (0 == strcmp(name, current->name)))
        {
            entry = current;
            break;
        }
    }

    return entry;
}

static struct  static_filesystem_entry *
static_filesystem_add_entry(
    struct static_filesystem * filesystem)
{
    struct  static_filesystem_entry * entry = NULL;

    if (filesystem->size >= filesystem->capacity)
    {
        struct static_filesystem_entry * entries;

        size_t new_capacity = 2 * filesystem->capacity;
        size_t new_size = new_capacity * sizeof(struct static_filesystem_entry);
        entries = realloc(filesystem->entries, new_size);

        if (NULL != entries)
        {
            filesystem->entries = entries;
            filesystem->capacity = new_capacity;
        }
    }

    if (filesystem->size < filesystem->capacity)
    {
        entry = &filesystem->entries[filesystem->size];
        entry->inode = filesystem->size + 1;
        filesystem->size++;
    }    

    return entry;
}

static size_t
static_filesystem_entry_read(
    size_t offset,
    char * buffer,
    size_t buffer_size,
    void * user_data)
{
    size_t result = 0;
    struct static_filesystem_entry * entry = user_data;
    if (offset < entry->size)
    {
        size_t remaining = (entry->size - offset);
        result = (buffer_size < remaining) ? buffer_size : remaining;
        memcpy(buffer, entry->content, result);
    }

    return result;
}

static void
static_filesystem_entry_get_info(
    void * user_data,
    int * result_mode,
    size_t * result_size)
{
    struct static_filesystem_entry * entry = user_data;
    *result_mode = entry->mode;
    *result_size = entry->size;
}

static size_t
static_filesystem_file_read(
    size_t offset,
    char * buffer,
    size_t buffer_size,
    void * user_data)
{
    size_t result = 0;
    struct static_filesystem_entry * entry = user_data;
    char const * filename = entry->content;

    FILE * file = fopen(filename, "rb");
    if (NULL != file)
    {
        fseek(file, offset, SEEK_SET);
        result = fread(buffer, buffer_size, 1, file);
        fclose(file);
    }

    return result;
}

static void
static_filesystem_file_get_info(
    void * user_data,
    int * result_mode,
    size_t * result_size)
{
    struct static_filesystem_entry * entry = user_data;
    char const * filename = entry->content;

    struct stat buffer;
    stat(filename, &buffer);

    *result_mode = (int) (buffer.st_mode & 0777);
    *result_size = (size_t) buffer.st_size;
}


static size_t
static_filesystem_add_dir(
    struct static_filesystem * filesystem,
    size_t parent,
    char const * name
)
{
    struct  static_filesystem_entry * entry = static_filesystem_get_entry_by_name(filesystem, parent, name);
    if (NULL == entry)
    {
        entry = static_filesystem_add_entry(filesystem);
        entry->parent = parent;
        entry->is_file = false;
        entry->mode = 0555;
        entry->name = strdup(name);
        entry->user_data = entry;
        entry->read = &static_filesystem_entry_read;
        entry->get_info = &static_filesystem_entry_get_info;
        entry->size = 0;
        entry->content = NULL;
    }
    
    return entry->inode;
}

static size_t
static_filesystem_make_parent(
    struct static_filesystem * filesystem,
    struct path * path)
{
    size_t result = STATIC_FILSYSTEM_INODE_ROOT;

    size_t count = path_element_count(path);
    if (0 < count)
    {
        for(size_t i = 0; i < (count - 1); i++)
        {
            char const * name = path_get_element(path, i);
            result = static_filesystem_add_dir(filesystem, result, name);
        }
    }

    return result;
}

static void
static_filesystem_stat(
    struct static_filesystem_entry * entry,
    struct stat * stat
)
{
    memset(stat, 0, sizeof(struct stat));

    int mode;
    size_t size;
    entry->get_info(entry->user_data, &mode, &size);   

    stat->st_ino = entry->inode;
    stat->st_size = entry->size;
    stat->st_mode = entry->mode & 0777;
    stat->st_mode |= (entry->is_file) ? S_IFREG: S_IFDIR;
}

static void static_filesystem_lookup(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    struct static_filesystem * filesystem = user_data;
    struct static_filesystem_entry * entry = static_filesystem_get_entry_by_name(filesystem, parent, name);

    if (NULL != entry)
    {
        struct stat stat;
        static_filesystem_stat(entry, &stat);
        wfp_respond_lookup(request, &stat);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}


static void static_filesystem_getattr(
    struct wfp_request * request,
    ino_t inode,
    void * user_data)
{
    struct static_filesystem * filesystem = user_data;
    struct static_filesystem_entry * entry = static_filesystem_get_entry(filesystem, inode);

    if (NULL != entry)
    {
        struct stat stat;
        static_filesystem_stat(entry, &stat);
        wfp_respond_getattr(request, &stat);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static void static_filesystem_readdir(
    struct wfp_request * request,
    ino_t directory,
    void * user_data)
{
    struct static_filesystem * filesystem = user_data;
    struct static_filesystem_entry * dir = static_filesystem_get_entry(filesystem, directory);

    if ((NULL != dir) && (!dir->is_file))
    {
        struct wfp_dirbuffer * buffer = wfp_dirbuffer_create();
        wfp_dirbuffer_add(buffer, ".", dir->inode);
        wfp_dirbuffer_add(buffer, "..", dir->inode);

        for(size_t i = 0; i < filesystem->size; i++)
        {
            struct static_filesystem_entry const * entry = &filesystem->entries[i];
            if (directory == entry->parent)
            {
                wfp_dirbuffer_add(buffer, entry->name, entry->inode);
            }
        }

        wfp_respond_readdir(request, buffer);
        wfp_dirbuffer_dispose(buffer);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static void static_filesystem_open(
    struct wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    struct static_filesystem * filesystem = user_data;
    struct static_filesystem_entry * entry = static_filesystem_get_entry(filesystem, inode);

    if ((NULL != entry) && (entry->is_file))
    {
        if (O_RDONLY == (flags & O_ACCMODE))
        {
            wfp_respond_open(request, 0U);
        }
        else
        {
            wfp_respond_error(request, WF_BAD_ACCESS_DENIED);
        }
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }
}

static void static_filesystem_read(
    struct wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data)
{
    (void) handle;

    struct static_filesystem * filesystem = user_data;
    struct static_filesystem_entry * entry = static_filesystem_get_entry(filesystem, inode);

    if ((NULL != entry) && (entry->is_file))
    {
        char buffer[STATIC_FILESYSTEM_MAX_READ_SIZE];
        size_t max_size = (length < STATIC_FILESYSTEM_MAX_READ_SIZE) ? length : STATIC_FILESYSTEM_MAX_READ_SIZE;

        size_t count = entry->read(offset, buffer, max_size, entry->user_data);
        wfp_respond_read(request, buffer, count);
    }
    else
    {
        wfp_respond_error(request, WF_BAD_NOENTRY);
    }    
}


struct static_filesystem *
static_filesystem_create(
    struct wfp_client_config * config)
{
    (void) config;

    struct static_filesystem * filesystem = malloc(sizeof(struct static_filesystem));
    if (NULL != filesystem)
    {
        filesystem->entries = malloc(sizeof(struct static_filesystem_entry) * STATIC_FILESYSTEM_DEFAULT_CAPACITY);
        filesystem->size = 0;
        filesystem->capacity = STATIC_FILESYSTEM_DEFAULT_CAPACITY;

        static_filesystem_add_dir(filesystem, 0, "<root>");

        wfp_client_config_set_userdata(config, filesystem);
        wfp_client_config_set_onlookup(config, &static_filesystem_lookup);
        wfp_client_config_set_ongetattr(config, &static_filesystem_getattr);
        wfp_client_config_set_onreaddir(config, &static_filesystem_readdir);
        wfp_client_config_set_onopen(config, &static_filesystem_open);
        wfp_client_config_set_onread(config, &static_filesystem_read);
    }

    return filesystem;
}

void
static_filesystem_dispose(
    struct static_filesystem * filesystem)
{
    for(size_t i = 0; i < filesystem->size; i++)
    {
        struct static_filesystem_entry * entry = &filesystem->entries[i];
        free(entry->name);
        free(entry->content);
    }

    free(filesystem->entries);
    free(filesystem);
}

void
static_filesystem_add(
    struct static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content,
    size_t length)
{
    struct path * path_ = path_create(path);
    if (NULL != path_)
    {
        size_t parent = static_filesystem_make_parent(filesystem, path_);
        struct static_filesystem_entry * entry = static_filesystem_add_entry(filesystem);
        entry->parent = parent;
        entry->is_file = true;
        entry->name = strdup(path_get_filename(path_));
        entry->mode = mode;
        entry->size = length;
        entry->get_info = &static_filesystem_entry_get_info;
        entry->read = &static_filesystem_entry_read;
        entry->user_data = entry;

        entry->content = malloc(length);
        memcpy(entry->content, content, length);

        path_dispose(path_);
    }
}

void
static_filesystem_add_text(
    struct static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content)
{
    size_t length = strlen(content);
    static_filesystem_add(filesystem, path, mode, content, length);
}

void
static_filesystem_add_file(
    struct static_filesystem * filesystem,
    char const * path,
    char const * filename)
{
    struct path * path_ = path_create(path);
    if (NULL != path_)
    {
        size_t parent = static_filesystem_make_parent(filesystem, path_);
        struct static_filesystem_entry * entry = static_filesystem_add_entry(filesystem);
        entry->parent = parent;
        entry->is_file = true;
        entry->mode = 0;
        entry->content = strdup(filename);
        entry->size = 0;
        entry->name = strdup(path_get_filename(path_));
        entry->get_info = &static_filesystem_file_get_info;
        entry->read = &static_filesystem_file_read;
        entry->user_data = entry;

        path_dispose(path_);
    }
}

void
static_filesystem_add_generic(
    struct static_filesystem * filesystem,
    char const * path,
    static_filesystem_read_fn * read,
    static_filesystem_get_info_fn * get_info,
    void * user_data)
{
    struct path * path_ = path_create(path);
    if (NULL != path_)
    {
        size_t parent = static_filesystem_make_parent(filesystem, path_);
        struct static_filesystem_entry * entry = static_filesystem_add_entry(filesystem);
        entry->parent = parent;
        entry->is_file = true;
        entry->mode = 0;
        entry->content = NULL;
        entry->size = 0;
        entry->name = strdup(path_get_filename(path_));
        entry->get_info = get_info;
        entry->read = read;
        entry->user_data = user_data;

        path_dispose(path_);
    }
}
