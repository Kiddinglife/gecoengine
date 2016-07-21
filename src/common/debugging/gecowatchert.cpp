/*
 * gecowatchert.cpp
 *
 *  Created on: 18Jul.,2016
 *      Author: jackiez
 */

#include "gecowatchert.h"
#if ENABLE_WATCHERS
using namespace geco::debugging;

#include "debug.h"
DECLARE_DEBUG_COMPONENT2("engine-common-module-logger", 0);

// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　- -　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
// 　　　　　　　　　　　　　　　　　　　　　　　　　Section: watcher_path_request_v2 impls
// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　- -　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
void watcher_path_request_v2::set_result_stream(const std::string & desc, const WatcherMode & mode,
        geco_watcher_base_t * watcher, const void *base)
{
    if (mode == WT_DIRECTORY && !is_visiting_dirs_)
    {
        is_visiting_dirs_ = true;
        origin_request_path_ = request_path_;
        watcher->visit_children(base, NULL, *this);
        request_path_ = origin_request_path_;
        is_visiting_dirs_ = false;
        // Ready to fill in the packet now
        this->notify();
    }
    else if (mode != WT_INVALID)
    {
        // At this point we have type, mode, size, and data on the stream
        // (as that is filled before calling this function)
        // Ready to fill in the packet now
        // Tell our parent we have collected all our data
        if (!is_visiting_dirs_)
        {
            this->notify();
        }
    }
}
void watcher_path_request_v2::get_watcher_value()
{
    if (!geco_watcher_base_t::get_root_watcher().get_value_to_stream(NULL, request_path_.c_str(), *this))
    {
        result_.WriteMini((uchar) WVT_UNKNOWN);
        result_.WriteMini((uchar) WT_READ_ONLY);
        result_.WriteMini(false);
        // And notify the parent of our failure.
        this->notify();
    }
}
bool watcher_path_request_v2::set_watcher_value()
{
    bool status = geco_watcher_base_t::get_root_watcher().set_value_from_stream(NULL, request_path_.c_str(), *this);
    if (!status)
    {
        result_.WriteMini((uchar) WVT_UNKNOWN);
        result_.WriteMini((uchar) WT_READ_ONLY);
        result_.WriteMini(status);
        // We're pretty much done setting watcher value (failed)
        // Ready to fill in the packet now
        this->notify();
    }
    return true;
}
bool watcher_path_request_v2::add_watcher_path(const void *base, const char *path, std::string & label,
        geco_watcher_base_t &watcher)
{
    std::string desc;
    if (origin_request_path_.size() > 0) request_path_ = origin_request_path_ + "/" + label;
    else
        request_path_ = label;
    // Push the directory entry onto the result stream
    // We are using a reference to the correct watcher now, so no need
    // to pass in the path to search for.
    bool status = watcher.get_value_to_stream(base, NULL, *this);
    if (!status)
    {
        // If the get operation failed, we still need to notify the parent
        // so it can continue with its response.
        result_.WriteMini((uchar) WVT_UNKNOWN);
        result_.WriteMini((uchar) WT_READ_ONLY);
        result_.WriteMini(status);
    }
    // Directory entries require an appended label
    result_.WriteMini(label);
    // Always need to return true from the version 2 protocol
    // otherwise the stream won't be completed.
    return true;
}
// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
// 　　　　　　　　　　　　　　　　　　　　　　　　　Section: geco_watcher_t
// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
static geco_watcher_base_t* root_watcher_gptr = NULL;
geco_watcher_base_t & geco_watcher_base_t::get_root_watcher()
{
    if (root_watcher_gptr == NULL)
    {
        root_watcher_gptr = new geco_watcher_director_t();
    }
    return *root_watcher_gptr;
}
void destroy_root_watcher()
{
    if (root_watcher_gptr != NULL)
    {
        delete root_watcher_gptr;
        root_watcher_gptr = NULL;
    }
}
void geco_watcher_base_t::partition_path(const std::string path, std::string & name, std::string & dir)
{
    int pos = path.find_last_of(WATCHER_PATH_SEPARATOR);
    if (0 <= pos && pos < (int) path.size())
    {
        dir = path.substr(0, pos + 1);
        name = path.substr(pos + 1, path.length() - pos - 1);
    }
    else
    {
        name = path;
        dir = "";
    }
}

// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
// 　　　　　　　　　　　　　　　　　　　　　　　　　Section: geco_watcher_director_t
// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
watcher_directory_t* geco_watcher_director_t::find_child(const char * path) const
{
    if (path == NULL) return NULL;
    char* pseparator = strchr((char*) path, WATCHER_PATH_SEPARATOR);
    uint cmp_len = (pseparator == NULL) ? strlen(path) : (pseparator - path);
    watcher_directory_t* ptr = NULL;
    if (cmp_len != 0)
    {
        auto iter = container_.begin();
        while (iter != container_.end() && ptr != NULL)
        {
            if (cmp_len == (*iter).label.length() && strncmp(path, (*iter).label.c_str(), cmp_len) == 0)
            {
                ptr = (watcher_directory_t*) (&(*iter));
            }
            ++iter;
        }
    }
    return ptr;
}
bool geco_watcher_director_t::add_watcher(const char * path, geco_watcher_base_t& pChild, void * withBase)
{
    bool was_added = false;

    if (this->is_empty_path(path))
    {
        ERROR_MSG("geco_watcher_director_t::add_watcher() tried to add unnamed child (no trailing slashes please)\n");
    }
    else if(strchr(path, '/') == NULL)  // path is actually watcher name
    {
        if (this->find_child(path) == NULL)
        {        //make sure we do not add it twice
            watcher_directory_t newdir;
            newdir.watcher = &pChild;
            newdir.base = withBase;
            newdir.label = path;
            auto iter = container_.begin();
            while (iter != container_.end() && (iter->label < newdir.label)) ++iter;
            container_.insert( iter, newdir );
            was_added = true;
        }
        else  // it is a child
        {
            WARNING_MSG( "geco_watcher_director_t::add_watcher()  add existed watcher (%s) is NOT allowed ! \n", path );
        }
    }
    else
    {
        watcher_directory_t* pFound = this->find_child(path);
        if (pFound == NULL)
        {
            char* pseparator = strchr((char*) path, WATCHER_PATH_SEPARATOR);
            uint cmp_len = (pseparator == NULL) ? strlen(path) : (pseparator - path);
            geco_watcher_base_t* newwatcher = new geco_watcher_director_t();
            watcher_directory_t newdir;
            newdir.watcher = newwatcher;
            newdir.base = NULL;
            newdir.label = std::string(path, cmp_len);
            auto iter = container_.begin();
            while (iter != container_.end() && (iter->label < newdir.label)) ++iter;
            pFound = &(*(container_.insert( iter, newdir )));
        }
        // recusice call add_watcher, this will create new watcher dir if needed
        // finally result is a new path is built and the watcher itself is added
        if(pFound != NULL)
        {
            was_added = pFound->watcher->add_watcher(geco_watcher_director_t::get_path_tail(path), pChild, withBase);
        }
    }
    return was_added;
}
bool geco_watcher_director_t::remove_watcher(const char * path)
{
    if (path == NULL) return false;
    char* pseparator = strchr((char*) path, WATCHER_PATH_SEPARATOR);
    uint cmp_len = (pseparator == NULL) ? strlen(path) : (pseparator - path);
    if (cmp_len > 0)
    {
        auto iter = container_.begin();
        while (iter != container_.end())
        {
            if (cmp_len == (*iter).label.length() && strncmp(path, (*iter).label.c_str(), cmp_len) == 0)
            {
                if (pseparator == NULL)
                {
                    container_.erase(iter);
                    return true;
                }
                else
                {
                    return iter->watcher->remove_watcher(geco_watcher_director_t::get_path_tail(path));
                }
            }
            ++iter;
        }
    }
    return false;
}
bool geco_watcher_director_t::set_value_from_string(void * base, const char * path, const char * valueStr)
{
    watcher_directory_t* pChild = this->find_child(path);
    if (pChild != NULL)
    {
        void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
        return pChild->watcher->set_value_from_string(addedBase, geco_watcher_director_t::get_path_tail(path), valueStr);
    }
    else
    {
        return false;
    }
}
bool geco_watcher_director_t::set_value_from_stream(void * base, const char * path,
        watcher_path_request_v2& pathRequest)
{
    watcher_directory_t* pChild = this->find_child(path);
    if (pChild != NULL)
    {
        void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
        return pChild->watcher->set_value_from_stream(addedBase, geco_watcher_director_t::get_path_tail(path),
                pathRequest);
    }
    else
    {
        return false;
    }
}
bool geco_watcher_director_t::get_value_to_string(const void * base, const char * path, std::string & result,
        std::string & desc, WatcherMode & mode) const
{
    if (geco_watcher_director_t::is_empty_path(path))
    {
        result = "<dir>";
        mode = WatcherMode::WT_DIRECTORY;
        desc = this->comment_;
        return true;
    }
    else
    {
        watcher_directory_t* pChild = this->find_child(path);
        if (pChild != NULL)
        {
            void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
            return pChild->watcher->get_value_to_string(addedBase, geco_watcher_director_t::get_path_tail(path), result,
                    desc, mode);
        }
        else
        {
            return false;
        }
    }
}
bool geco_watcher_director_t::get_value_to_stream(const void * base, const char * path,
        watcher_path_request_v2 & pathRequest) const
{
    return false;
}

// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
// 　　　　　　　　　　　　　　　　　　　　　　　　　Section: Helper functions
// -　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-　-　-　-　-　-　-　-　- -　-　-　-　- -　-　-　-　-　-　-
/// This helper function is used to watch the component priorities.
int init_watcher(int& value, const char * path)
{
    // WatcherPtr pWatcher = new DataWatcher<int>(value, Watcher::WT_READ_WRITE);
    // geco_watcher_base_t::get_root_watcher().add_watcher(path, pWatcher);
    return 0;
}

#endif

