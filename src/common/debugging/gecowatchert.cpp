/*
 * gecowatchert.cpp
 *
 *  Created on: 18Jul.,2016
 *      Author: jackiez
 */

//  gecowatchert.h includes plateform.h that includes <windows.h>
#include "gecowatchert.h"

#if ENABLE_WATCHERS

DECLARE_DEBUG_COMPONENT2("COMM", LOG_MSG_CRITICAL);


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
void geco_watcher_base_t::partition_path(const std::string path, std::string & name,
        std::string & dir)
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
bool geco_watcher_director_t::add_watcher(const char * path, geco_watcher_base_t& pChild,
        void * withBase)
{
    bool was_added = false;
    if (this->is_empty_path(path))
    {
        ERROR_MSG(
        "geco_watcher_director_t::add_watcher() tried to add unnamed child (no trailing slashes please)\n");
    }
    else if (strchr(path, '/') == NULL)  // appending value
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
            container_.insert(iter, newdir);
            was_added = true;
            //VERBOSE_MSG("append watcher %s, container size %d!\n\n", newdir.label.c_str(), (int)container_.size());
        }
        else  //existed value
        {
            WARNING_MSG("geco_watcher_director_t::add_watcher()  add existed watcher label (%s) is NOT allowed ! \n", path);
        }
    }
    else  // it is child dir watcher
    {
        watcher_directory_t* pFound = this->find_child(path);
        if (pFound == NULL)
        {
            char* pseparator = strchr((char*)path, WATCHER_PATH_SEPARATOR);
            uint cmp_len = (pseparator == NULL) ? strlen(path) : (pseparator - path);
            geco_watcher_base_t* newwatcher = new geco_watcher_director_t();
            watcher_directory_t newdir;
            newdir.watcher = newwatcher;
            newdir.base = NULL;
            newdir.label = std::string(path, cmp_len);
            auto iter = container_.begin();
            while (iter != container_.end() && (iter->label < newdir.label)) ++iter;
            pFound = &(*(container_.insert(iter, newdir)));
            //VERBOSE_MSG(" create new dir (%s),container size %d\n", newdir.label.c_str(), (int)container_.size());
        }
        // recusice call add_watcher, this will create new watcher dir if needed
        // finally result is a new path is built and the watcher itself is added
        if (pFound != NULL)
        {
            was_added = pFound->watcher->add_watcher(get_path_tail(path), pChild, withBase);
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
            if (cmp_len == (*iter).label.length()
                    && strncmp(path, (*iter).label.c_str(), cmp_len) == 0)
            {
                if (pseparator == NULL)
                {
                    container_.erase(iter);
                    // VERBOSE_MSG(" delete watcher (%s) sucesseds\n",(*iter).label.c_str());
                    return true;
                }
                else
                {
                    return iter->watcher->remove_watcher(get_path_tail(path));
                }
            }
            ++iter;
        }
    }
    return false;
}
int geco_watcher_director_t::set_from_string(void * base, const char * path, const char * valueStr)
{
    watcher_directory_t* pChild = this->find_child(path);
    if (pChild != NULL)
    {
        void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
        return pChild->watcher->set_from_string(addedBase, get_path_tail(path), valueStr);
    }
    else
    {
        return false;
    }
}
int geco_watcher_director_t::set_from_stream(void * base, const char * path,
        watcher_value_query_t& pathRequest)
{
    watcher_directory_t* pChild = this->find_child(path);
    if (pChild != NULL)
    {
        void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
        return pChild->watcher->set_from_stream(addedBase, get_path_tail(path), pathRequest);
    }
    else
    {
        return 0;
    }
}
int geco_watcher_director_t::get_as_string(const void * base, const char * path,
        std::string & result, std::string & desc, WatcherMode & mode)
{
    if (geco_watcher_director_t::is_empty_path(path))
    {
        result = "<DIR>";
        mode = WatcherMode::WT_DIRECTORY;
        desc = this->comment_;
        return 1;
    }
    else
    {
        watcher_directory_t* pChild = this->find_child(path);
        if (pChild != NULL)
        {
            void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
            return pChild->watcher->get_as_string(addedBase, get_path_tail(path), result, desc,
                    mode);
        }
        else
        {
            return 0;
        }
    }
}
int geco_watcher_director_t::get_as_stream(const void * base, const char * path,
        watcher_value_query_t & pathRequest)
{
    if (geco_watcher_director_t::is_empty_path(path))
    {
        pathRequest.get_result_stream().Write((ushort) WT_DIRECTORY);
        uint size = container_.size();
        pathRequest.get_result_stream().Write(size);
        pathRequest.get_result_stream().Write(pathRequest.get_request_path());
        pathRequest.get_result_stream().Write(comment_);
        VERBOSE_MSG(
        "write dir watcher [%d,%d,%s,%s]\n", (int)WT_DIRECTORY, (int)size, pathRequest.get_request_path().c_str(), comment_);

        std::string old(pathRequest.origin_request_path_);
        pathRequest.origin_request_path_ = pathRequest.request_path_;
        this->visit_children(base, NULL, pathRequest);
        pathRequest.origin_request_path_ = old;
        pathRequest.request_path_ = pathRequest.origin_request_path_;
        pathRequest.notify();
        return true;
    }
    else if (geco_watcher_director_t::is_doc_path(path))
    {
        write_watcher_value_to_stream(pathRequest.get_result_stream(), comment_, WT_READ_ONLY);
        std::string old(pathRequest.origin_request_path_);
        pathRequest.origin_request_path_ = pathRequest.request_path_;
        this->visit_children(base, NULL, pathRequest);
        pathRequest.origin_request_path_ = old;
        pathRequest.request_path_ = pathRequest.origin_request_path_;
        pathRequest.notify();
        return true;
    }
    else
    {
        watcher_directory_t* pChild = this->find_child(path);
        if (pChild != NULL)
        {
            void * addedBase = (void*) (((uintptr) base) + ((uintptr) pChild->base));
            return pChild->watcher->get_as_stream(addedBase, get_path_tail(path), pathRequest);
        }
        else
        {
            return false;
        }
    }
    return false;
}
bool geco_watcher_director_t::visit_children(const void * base, const char *path,
        watcher_value_query_t& pathRequest)
{
    bool handled = false;

    if (is_empty_path(path))
    {
        bool ret;
        Container::iterator iter = container_.begin();
        while (iter != container_.end())
        {
            const void * addedBase = (const void*) (((const uintptr) base)
                    + ((const uintptr) (*iter).base));
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
            const void * addedBase = (const void*) (((const uintptr) base)
                    + ((const uintptr) pChild->base));
            handled = pChild->watcher->visit_children(addedBase, get_path_tail(path), pathRequest);
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
        ret = geco_watcher_base_t::get_root_watcher().add_watcher(path, *ptr,
        NULL);
    }
    else
    {
        ret = false;
    }
    return ret;
}
#endif


