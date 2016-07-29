/*
 * gecowatchert.cpp
 *
 *  Created on: 18Jul.,2016
 *      Author: jackiez
 */

#include "gecowatchert.h"

#if ENABLE_WATCHERS

DECLARE_DEBUG_COMPONENT2("COMM", 0);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - 
// 　　　　　　　　　　　　　　Section: watcher_path_request_v1 impls
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - 
bool watcher_path_request_v1::set_watcher_value()
{
    if (!geco_watcher_base_t::get_root_watcher().set_from_string(NULL, request_path_.c_str(), setopt_string_.c_str()))
    {
        ERROR_MSG("watcher_path_request_v1::set_watcher_value(): set_from_string() failed!\n");
        this->notify(0);
        return false;
    }
    this->get_watcher_value();
    return true;
}
void watcher_path_request_v1::get_watcher_value()
{
    static std::string desc;
    static std::string result;
    WatcherMode mode;
    desc.clear();
    result.clear();

    if (!geco_watcher_base_t::get_root_watcher().get_as_string(NULL, request_path_.c_str(), result, desc, mode))
    {
        ERROR_MSG("get_watcher_value():get_as_string return FALSE!\n");
        this->notify(0);
        return;
    }

    if (mode == WT_DIRECTORY)
    {
        replies_count_ = 0;
        if (!geco_watcher_base_t::get_root_watcher().visit_children(NULL, request_path_.c_str(), *this))
        {
            replies_count_ = 0;
        }
        // At this point we've collected results from directory's children
        // watchers. containedReplies_ also contains the amount of children
        // under this directory.
        this->notify(replies_count_);
    }
    else if (mode != WT_INVALID)
    {
        // Add the result onto the final result stream
        write_watcher_value_to_stream(result_, request_path_, WT_READ_ONLY);
        write_watcher_value_to_stream(result_, result, WT_READ_ONLY);
        if (use_desc_)
        {
            write_watcher_value_to_stream(result_, true, WT_READ_ONLY);
            write_watcher_value_to_stream(result_, desc, WT_READ_ONLY);
        }
        else
            write_watcher_value_to_stream(result_, false, WT_READ_ONLY, sizeof(false));
        // Tell our parent we have collected all our data
        this->notify();
    }
}

bool watcher_path_request_v1::on_visit_dirwt_child(WatcherMode mode, const std::string & label,
        const std::string & desc, const std::string & valueStr)
{
    static std::string path;
    path.clear();

    path += valueStr;
    // add up dir name and make it path
    request_path_.size() > 0 ? path += request_path_ + "/" + label : path = label;
    if (use_desc_)
    {
        path += desc;
    }
    path += "\n";
    // Add the result onto the final result stream
    result_.Write(path);
    this->replies_count_++;
    printf("%s", path.c_str());
    return true;
}
bool watcher_path_request_v1::add_watcher_path(const void *base, const char *path, std::string & label,
        geco_watcher_base_t &watcher)
{
    static std::string valstr;
    static std::string desc;
    valstr.clear();
    valstr.clear();
    WatcherMode mode;
    watcher.get_as_string(base, path, valstr, desc, mode);
    this->on_visit_dirwt_child(mode, label, desc, valstr);
    return watcher.visit_children(base, NULL, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
// 　　　　　　　　　　　　Section: watcher_path_request_v2 impls
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
void watcher_path_request_v2::set_result_stream(const std::string & desc, const WatcherMode mode,
        geco_watcher_base_t * watcher, const void *base)
{
    if (mode == WT_DIRECTORY)
    {
        std::string old(origin_request_path_);
        origin_request_path_ = request_path_;
        watcher->visit_children(base, NULL, *this);
        origin_request_path_ = old;
        request_path_ = origin_request_path_;
        this->notify();
    }
    else if (mode != WT_INVALID)
    {
        // At this point we have type, mode, size, and data on the stream
        // (as that is filled before calling this function)
        // Ready to fill in the packet now
        // Tell our parent we have collected all our data
        this->notify();
    }
}
void watcher_path_request_v2::get_watcher_value()
{
    if (!geco_watcher_base_t::get_root_watcher().get_as_stream(NULL, request_path_.c_str(), *this))
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
    bool status = geco_watcher_base_t::get_root_watcher().set_from_stream(NULL, request_path_.c_str(), *this);
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
    origin_request_path_.size() > 0 ? request_path_ = origin_request_path_ + "/" + label : request_path_ = label;

// Push the directory entry onto the result stream
// We are using a reference to the correct watcher now, so no need
// to pass in the path to search for.
    bool status = watcher.get_as_stream(base, NULL, *this);

    if (!status)
    {
        ERROR_MSG("watcher_path_request_v2::add_watcher_path::!status error\n");
        // If the get operation failed, we still need to notify the parent
        // so it can continue with its response.
        result_.WriteMini((uchar) WVT_UNKNOWN);
        result_.WriteMini((uchar) WT_READ_ONLY);
        result_.WriteMini(status);
        return false;
    }
// Always need to return true from the asyn version 2 protocol
// otherwise the stream won't be completed.
    return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
// 　　　　　　　　　　　                       geco_watcher_t impls
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
// 　　　　　　　　　　　　 Section: geco_watcher_director_t
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
watcher_directory_t* geco_watcher_director_t::find_child(const char * path) const
{
    if (path == NULL) return NULL;
    char* pseparator = strchr((char*) path, WATCHER_PATH_SEPARATOR);
    uint cmp_len = (pseparator == NULL) ? strlen(path) : (pseparator - path);

//    char* label = new char[cmp_len + 1];
//    label[cmp_len] = '\0';
//    memcpy(label, path, cmp_len);
//    TRACE_MSG("search for  child label %s\n", label);

    watcher_directory_t* ptr = NULL;
    if (cmp_len != 0)
    {
        auto iter = container_.begin();
        while (iter != container_.end() && ptr == NULL)
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
    else if(strchr(path, '/') == NULL)  // appending value
    {
        if (this->find_child(path) == NULL)
        {
            //make sure we do not add it twice
            watcher_directory_t newdir;
            newdir.watcher = &pChild;
            newdir.base = withBase;
            newdir.label = path;
            auto iter = container_.begin();
            while (iter != container_.end() && (iter->label < newdir.label)) ++iter;
            container_.insert( iter, newdir );
            was_added = true;
            TRACE_MSG("append watcher %s, container size %d!\n\n",newdir.label.c_str(), (int)container_.size());
        }
        else  //existed value
        {
            WARNING_MSG( "geco_watcher_director_t::add_watcher()  add existed watcher label (%s) is NOT allowed ! \n", path );
        }
    }
    else  // it is child dir watcher
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
            TRACE_MSG(" create new dir (%s),container size %d\n", newdir.label.c_str(), (int)container_.size());
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
                    // TRACE_MSG(" delete watcher (%s) sucesseds\n",(*iter).label.c_str());
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
bool geco_watcher_director_t::set_from_string(void * base, const char * path, const char * valueStr)
{
    watcher_directory_t* pChild = this->find_child(path);
    if (pChild != NULL)
    {
        void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
        return pChild->watcher->set_from_string(addedBase, geco_watcher_director_t::get_path_tail(path), valueStr);
    }
    else
    {
        return false;
    }
}
bool geco_watcher_director_t::set_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
{
    watcher_directory_t* pChild = this->find_child(path);
    if (pChild != NULL)
    {
        void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
        return pChild->watcher->set_from_stream(addedBase, geco_watcher_director_t::get_path_tail(path), pathRequest);
    }
    else
    {
        return false;
    }
}
bool geco_watcher_director_t::get_as_string(const void * base, const char * path, std::string & result,
        std::string & desc, WatcherMode & mode)
{
    if (geco_watcher_director_t::is_empty_path(path))
    {
        result = "<DIR>";
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
            return pChild->watcher->get_as_string(addedBase, geco_watcher_director_t::get_path_tail(path), result, desc,
                    mode);
        }
        else
        {
            return false;
        }
    }
}
bool geco_watcher_director_t::get_as_stream(const void * base, const char * path, watcher_path_request_v2 & pathRequest)
{
    if (geco_watcher_director_t::is_empty_path(path))
    {
        pathRequest.get_result_stream().WriteMini((uchar) WT_DIRECTORY);
        pathRequest.get_result_stream().WriteMini((int) container_.size());
        pathRequest.get_result_stream().Write(pathRequest.get_request_path());
        pathRequest.get_result_stream().Write(comment_);
        TRACE_MSG("write [%d,%d,%s,%s]\n",(int)WT_DIRECTORY, (int)container_.size(), pathRequest.get_request_path().c_str(),comment_);
        pathRequest.set_result_stream(comment_, WT_DIRECTORY, this, base);
        return true;
    }
    else if (geco_watcher_director_t::is_doc_path(path))
    {
        write_watcher_value_to_stream(pathRequest.get_result_stream(), comment_, WT_READ_ONLY);
        pathRequest.set_result_stream(comment_, WT_DIRECTORY, this, base);
        return true;
    }
    else
    {
        watcher_directory_t* pChild = this->find_child(path);
        if (pChild != NULL)
        {
            void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
            return pChild->watcher->get_as_stream(addedBase, geco_watcher_director_t::get_path_tail(path), pathRequest);
        }
        else
        {
            return false;
        }
    }
    return false;
}
bool geco_watcher_director_t::visit_children(const void * base, const char *path, watcher_path_request& pathRequest)
{
    bool handled = false;

    if (is_empty_path(path))
    {
        bool ret;
        Container::iterator iter = container_.begin();
        while (iter != container_.end())
        {
            const void * addedBase = (const void*) (((const uintptr) base) + ((const uintptr) (*iter).base));
            ret = pathRequest.add_watcher_path(addedBase, NULL, iter->label, *iter->watcher);
            if (!ret) break;
            iter++;
        }
        handled = true;
    }
    else
    {
        watcher_directory_t* pChild = this->find_child(path);
        if (pChild != NULL)
        {
            const void * addedBase = (const void*) (((const uintptr) base) + ((const uintptr) pChild->base));
            handled = pChild->watcher->visit_children(addedBase, geco_watcher_director_t::get_path_tail(path),
                    pathRequest);
        }
    }
    return handled;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
// 　　　　　　　　　　　　　　　　　　　　　　　　　Section: Helper functions
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - -
/// This helper function is used to watch the component priorities.
bool init_value_watcher(int& value, const char * path)
{
    int ret;
    if (path != NULL)
    {
        geco_watcher_base_t* ptr = new value_watcher_t<int>(value, WT_READ_WRITE, path);
        ret = geco_watcher_base_t::get_root_watcher().add_watcher(path, *ptr, NULL);
    }
    else
    {
        ret = false;
    }
    return ret;
}
#endif

