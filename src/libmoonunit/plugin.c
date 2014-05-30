/*
 * Copyright (c) 2007, Brian Koropoff
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Moonunit project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRIAN KOROPOFF ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL BRIAN KOROPOFF BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include <moonunit/plugin.h>
#include <moonunit/private/util.h>
#include <moonunit/loader.h>
#include <moonunit/logger.h>

#include <config.h>
#include <string.h>

#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

static array* plugin_list;
static array* handle_list;

static MuPlugin*
load_plugin(const char* path)
{
    void* handle = mu_dlopen(path, RTLD_LAZY);
   
    MuPlugin* (*load)(void);

    if (!handle)
    {
        return NULL;
    }

    handle_list = array_append(handle_list, handle);

    load = dlsym(handle, "__mu_p_init");

    if (!load)
        return NULL;

    return load();
}

static void
load_plugins_dir(const char* path)
{
    DIR* dir;
    struct dirent* entry;
    MuPlugin* plugin;

    dir = opendir(path);
    
    if (dir)
    {
        while ((entry = readdir(dir)))
        {
            if (ends_with(entry->d_name, PLUGIN_EXTENSION))
            {
                char* fullpath = format("%s/%s", path, entry->d_name);

                plugin = load_plugin(fullpath);
                free(fullpath);
                
                if (plugin)
                    plugin_list = array_append(plugin_list, plugin);
            }
        }
	closedir(dir);   
    }
}

static void
load_plugins(void)
{
    char* pathenv;
    char* extras;
    MuPlugin* plugin;

    if ((pathenv = getenv("MU_PLUGIN_PATH")))
    {
        char* path, *next;

        pathenv = strdup(pathenv);

        for (path = pathenv; path; path = next)
        {
            next = strchr(path, ':');
            if (next)
            {
                *(next++) = 0;
            }
            
            load_plugins_dir(path);
        }

        free(pathenv);
    }
    else
    {
        load_plugins_dir(PLUGIN_PATH);
    }

    if ((extras = getenv("MU_EXTRA_PLUGINS")))
    {
        char *extra, *next;

        extras = strdup(extras);

        for (extra = extras; extra; extra = next)
        {
            next = strchr(extra, ' ');
            if (next)
            {
                *(next++) = 0;
            }
            
            if ((plugin = load_plugin(extra)))
                plugin_list = array_append(plugin_list, plugin);
        }

        free(extras);
    }
}

static MuPlugin*
get_plugin(const char* name)
{
    unsigned int index;
    MuPlugin* plugin;
    size_t count;

    if (plugin_list == NULL)
    {
        load_plugins();
        if (plugin_list == NULL)
            return NULL;
    }

    count = array_size(plugin_list);

    for (index = 0; index < count; index++)
    {
        plugin = plugin_list[index];
        
        if (!strcmp(name, plugin->name))
        {
            return plugin;
        }
    }

    return NULL;
}

struct MuLoader*
mu_plugin_get_loader_with_name(const char *name)
{
    MuPlugin* plugin = get_plugin(name);
    MuLoader* loader;

    if (!plugin)
        return NULL;

    if (plugin->type != MU_PLUGIN_LOADER)
        return NULL;

    if (!plugin->loader)
        return NULL;

    loader = plugin->loader();

    if (!loader)
        return NULL;

    loader->plugin = plugin;

    return loader;
}

struct MuLoader*
mu_plugin_get_loader_for_file(const char* file)
{
    unsigned int index;

    for (index = 0; index < array_size(plugin_list); index++)
    {
        MuPlugin* plugin = plugin_list[index];
        if (plugin->type == MU_PLUGIN_LOADER && plugin->loader)
        {
            MuLoader* loader = plugin->loader();
            
            if (loader && mu_loader_can_open(loader, file))
                return loader;
        }
    }

    return NULL;
}

struct MuLogger*
mu_plugin_create_logger(const char* name)
{
    MuPlugin* plugin = get_plugin(name);
    MuLogger* logger;

    if (!plugin)
        return NULL;

    if (plugin->type != MU_PLUGIN_LOGGER)
        return NULL;

    if (!plugin->create_logger)
        return NULL;

    logger = plugin->create_logger();

    if (!logger)
        return NULL;

    logger->plugin = plugin;

    return logger;
}

MuPlugin**
mu_plugin_list(void)
{
    if (plugin_list == NULL)
    {
        load_plugins();
        if (plugin_list == NULL)
            return NULL;
    }

    return (MuPlugin**) plugin_list;
}

MuPlugin*
mu_plugin_get_by_name(const char* name)
{
    return get_plugin(name);
}

void
mu_plugin_shutdown(void)
{
    size_t i = 0;

    if (handle_list)
    {
        for (i = 0; i < array_size(handle_list); i++)
        {
            dlclose(handle_list[i]);
        }
        array_free(handle_list);
    }
    array_free(plugin_list);
}
