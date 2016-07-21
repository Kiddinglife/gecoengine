/*
 * Geco Gaming Company
 * All Rights Reserved.
 * Copyright (c)  2016 GECOEngine.
 *
 * GECOEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GECOEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * gecowatchert.h
 *
 *  Created on: 18Jul.,2016
 *      Author: jackiez
 */

#ifndef SRC_COMMON_DEBUGGING_GECOWATCHERT_H_
#define SRC_COMMON_DEBUGGING_GECOWATCHERT_H_

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <functional>

#include "../plateform.h"
#include "../geco-engine-config.h"
#include "../ds/geco-bit-stream.h"

namespace geco
{
    namespace debugging
    {
#if ENABLE_WATCHERS

        class geco_watcher_base_t;
        class watcher_path_request;
        class watcher_visitor_t;

        /**
         * @internal
         * Enumeration listing all valid data types that can be
         * sent and received via the watcher protocol.
         *  @ingroup WatcherModule
         */
        enum WatcherValueType
        {
            // user must implement the following functions in its cpp file
            // for the type he defines
            // operator >> (geco_bit_stream& is, user_type type)
            // operator << (geco_bit_stream& is, user_type type)
            WVT_USER_DEFINED,
            WVT_INTEGER,
            WVT_FLOATING,
            WVT_STRING,
            WVT_BLOB,
            WVT_TUPLE,
            WVT_TYPE,
            WVT_VECTOR2,
            WVT_NORMAL_VECTOR2,
            WVT_VECTOR3,
            WVT_NORMAL_VECTOR3,
            WVT_VECTOR4,
            WVT_NORMAL_VECTOR4,
            WVT_ORTH_METRIX,
            WVT_UNKNOWN,
            WVT_COUNT
        };

        /**
         *  @internal
         *  This enumeration is used to specify the type of the watcher.
         *  @ingroup WatcherModule
         */
        enum WatcherMode
        {
            WT_INVALID,         ///< Indicates an error.
            WT_READ_ONLY,         ///< Indicates the watched value cannot be changed.
            WT_READ_WRITE,         ///< Indicates the watched value can be changed.
            WT_DIRECTORY,         ///< Indicates that the watcher has children.
            WT_CALLABLE         ///< Indicates the watcher is a callable function
        };

        /**
         *  @internal
         *  The character that separates values in a watcher path.
         *  @ingroup WatcherModule
         */
        const char WATCHER_PATH_SEPARATOR = '/';

        /**
         * Interface to allow a WatcherPathRequest to notify
         * a calling context of successfull completion.
         */
        struct watcher_request_handler_t
        {
                /**
                 * Notify the implementing class of a path request completion.
                 * @param pathRequest The WatcherPathRequest that has been completed.
                 * @param count       The number of replies contained in the path request.
                 */
                virtual void on_request_complete(watcher_path_request & pathRequest, int32 count) = 0;
                /**
                 * Create a new WatcherPathRequest associated with the current packet.
                 * @param path The watcher path the request will query.
                 * @returns A pointer to a WatcherPathRequest on success, NULL on error.
                 */
                virtual watcher_path_request * create_request(std::string & path) = 0;
        };

        /**
         * WatcherPathRequest is a handler class to allow an asyncronous request
         * of a single watcher path.
         */
        class watcher_path_request
        {
            protected:
                std::string request_path_;
                watcher_request_handler_t* wprn_ptr_;

            public:
                watcher_path_request(const std::string & path)
                        : wprn_ptr_( NULL), request_path_(path)
                {
                }
                watcher_path_request()
                        : wprn_ptr_( NULL)
                {
                }
                virtual ~watcher_path_request()
                {
                }
                /**
                 * Initiate watcher value retreival for the current path request.
                 */
                virtual void get_watcher_value()
                {
                }
                /**
                 * Initiate a watcher path set operation after data to use
                 * has been placed into the WatcherPathRequest via @see setResult().
                 * @returns true on success, false on failure.
                 */
                virtual bool set_watcher_value()
                {
                    return false;
                }
                /**
                 * Accessor method for the watcher path associated with the
                 * current instance.
                 */
                const std::string & getPath() const
                {
                    return this->request_path_;
                }
                /**
                 * Setup the pointer back to the creator of this class to notify
                 * on completion.
                 */
                void setParent(watcher_request_handler_t *parent)
                {
                    wprn_ptr_ = parent;
                }
                /**
                 * Check if we have been told about a creator, and notify them of our
                 * completion.
                 */
                virtual void notify(int32 replies = 1)
                {
                    if (wprn_ptr_ != NULL)
                    {
                        wprn_ptr_->on_request_complete(*this, replies);
                    }
                }

                /**
                 * Notification method called by visitChildren with the number of children
                 * that we will be told about.
                 */
                virtual void add_watcher_num(uint count)
                {
                }
                /**
                 * Notification of a child watcher entry as called from visitChildren.
                 */
                virtual bool add_watcher_path(const void *base, const char *path, std::string & label,
                        geco_watcher_base_t &watcher)
                {
                    return false;
                }
                /**
                 * Return a pointer to the complete data set that should be used
                 * to create the watcher packet response.
                 */
                virtual const char *get_data()
                {
                    return NULL;
                }
                /**
                 * Return the size of the data that is returned by getData().
                 */
                virtual int32 get_data_size()
                {
                    return 0;
                }
        };

        class watcher_path_request_v2: public watcher_path_request
        {
            private:
                // Data obtained from the watcher nub to use in a set operation.
                char *set_data_;
                // Output stream for the watcher path requests data
                geco_bit_stream_t result_;
                // Input stream for the watcher path requests data
                geco_bit_stream_t set_stream_;
                // True when the path request is visiting children and shouldn't notify parent of completion.
                bool is_visiting_dirs_;
                // Our creator to notify on completion.
                watcher_request_handler_t *parent_;
                // Original path requested via watcher nub, used when visiting directories and requestPath_ may be altered.
                std::string origin_request_path_;

            public:
                watcher_path_request_v2(const std::string& path)
                        : watcher_path_request(path), set_data_(NULL), is_visiting_dirs_(false), parent_(
                        NULL)
                {
                }
                virtual ~watcher_path_request_v2()
                {
                }
                virtual void set_result_stream(const std::string & desc, const WatcherMode & mode,
                        geco_watcher_base_t * watcher, const void *base);
                virtual void get_watcher_value();
                virtual bool set_watcher_value();

                /*
                 * Methods used by visitChildren
                 */
                virtual bool add_watcher_path(const void *base, const char *path, std::string & label,
                        geco_watcher_base_t &watcher);
                virtual void add_watcher_num(uint count)
                {
                    result_.WriteMini(count);
                }

                /**
                 * Notify the Path Request of data to be used during a forwarding watcher
                 * call.
                 *
                 * This is used in a component when it receives the Mercury call to
                 * callWatcher. It will construct the path request and place the data
                 * sent via mercury into the path request which is then used to apply
                 * to the watcher path on the current component.
                 *
                 * @param data The pre-streamed data to use when performing a watcher SET.
                 *
                 * @returns true on success, false on error.
                 */
                void set_data(geco_bit_stream_t& data)
                {
                    set_stream_.reset();
                    set_stream_.Write(data);
                }
                virtual const char *get_data()
                {
                    return result_.char_data();
                }
                virtual int32 get_data_size()
                {
                    return BITS_TO_BYTES(result_.get_payloads());
                }
                geco_bit_stream_t & get_result_stream()
                {
                    return result_;
                }
                geco_bit_stream_t& get_value_stream()
                {
                    return set_stream_;
                }

        };

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Section Starts: string and watcher value : protocol v1
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // set operations
        template<class VALUE_TYPE>
        bool read_watcher_value(const char * valueStr, VALUE_TYPE &value)
        {
            std::stringstream stream;
            stream.write(valueStr, std::streamsize(strlen(valueStr)));
            stream.seekg(0, std::ios::beg);
            stream >> value;
            return !stream.bad();
        }
        inline bool read_watcher_value(const char * valueStr, bool & value)
        {
            if (geco_stricmp(valueStr, "true") == 0)
            {
                value = true;
            }
            else if (geco_stricmp(valueStr, "false") == 0)
            {
                value = false;
            }
            else
            {
                std::stringstream stream;
                stream.write(valueStr, std::streamsize(strlen(valueStr)));
                stream.seekg(0, std::ios::beg);
                stream >> value;
                return !stream.bad();
            }
            return true;
        }
        inline bool read_watcher_value(const char * valueStr, const char *& value)
        {
            value = valueStr;
            return true;
        }
        inline bool read_watcher_value(const char * valueStr, std::string & value)
        {
            value = valueStr;
            return true;
        }
        // get operations of protocol v1
        template<class VALUE_TYPE>
        std::string write_watcher_value(const VALUE_TYPE & value)
        {
            std::stringstream stream;
            stream << value;
            return stream.str();
        }
        inline std::string write_watcher_value(const std::string & value)
        {
            return value;
        }
        inline std::string write_watcher_value(bool value)
        {
            return value ? "true" : "false";
        }/* protocol v1 ends */

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Section: Stream between Watcher Value : protocol 2
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // GET  operations of protocol v2
        template<class ValueType>
        bool read_watcher_value(geco_bit_stream_t & result, WatcherValueType& type, ValueType& value, WatcherMode& mode)
        {
            result.ReadMini(type);
            result.ReadMini(mode);
            switch (type)
            {
                case WVT_USER_DEFINED:
                    result >> value;
                    break;
                case WVT_INTEGER:
                    result.ReadMini(value);
                    break;
                case WVT_FLOATING:
                    result.ReadMini(value);
                    break;
                case WVT_TYPE:
                    result.ReadMini(value);
                    break;
                case WVT_STRING:
                    result.ReadMini(value);
                    break;
                case WVT_VECTOR2:
                    result.ReadVector(value.x, value.y);
                    break;
                case WVT_NORMAL_VECTOR2:
                    result.ReadNormVector(value.x, value.y);
                    break;
                case WVT_VECTOR3:
                    result.ReadVector(value.x, value.y, value.z);
                    break;
                case WVT_NORMAL_VECTOR3:
                    result.ReadNormVector(value.x, value.y, value.z);
                    break;
                case WVT_BLOB:
                    uint bytes2Write;
                    result.ReadMini(bytes2Write);
                    result.ReadAlignedBytes((uchar*) value, bytes2Write);
                    break;
                default:
                    printf("write_watcher_value()::no such watcher value type (%d)!\n", type);
                    abort();
                    break;
            }
            return true;
        }
        // SET operations of protocol v2
        template<class ValueType>
        void write_watcher_value(geco_bit_stream_t & result, const WatcherValueType type, const ValueType & value,
                const WatcherMode & mode, const uint bytes2Write = 0)
        {
            switch (type)
            {
                case WVT_USER_DEFINED:
                    result.WriteMini((uchar) WVT_USER_DEFINED);
                    result.WriteMini((uchar) mode);
                    result << value;
                    break;
                case WVT_INTEGER:
                    result.WriteMini((uchar) WVT_INTEGER);
                    result.WriteMini((uchar) mode);
                    result.WriteMini(value);
                    break;
                case WVT_FLOATING:
                    result.WriteMini((uchar) WVT_FLOATING);
                    result.WriteMini((uchar) mode);
                    result.WriteMini(value);
                    break;
                case WVT_TYPE:
                    result.WriteMini((uchar) WVT_TYPE);
                    result.WriteMini((uchar) mode);
                    result.WriteMini(value);
                    break;
                case WVT_STRING:
                    result.WriteMini((uchar) WVT_STRING);
                    result.WriteMini((uchar) mode);
                    result.WriteMini(value);
                    break;
                case WVT_VECTOR2:
                    result.WriteMini((uchar) WVT_VECTOR2);
                    result.WriteMini((uchar) mode);
                    result.write_vector(value.x, value.y);
                    break;
                case WVT_NORMAL_VECTOR2:
                    result.WriteMini((uchar) WVT_NORMAL_VECTOR2);
                    result.WriteMini((uchar) mode);
                    result.write_normal_vector(value.x, value.y);
                    break;
                case WVT_VECTOR3:
                    result.WriteMini((uchar) WVT_VECTOR3);
                    result.WriteMini((uchar) mode);
                    result.write_vector(value.x, value.y, value.z);
                    break;
                case WVT_NORMAL_VECTOR3:
                    result.WriteMini((uchar) WVT_NORMAL_VECTOR3);
                    result.WriteMini((uchar) mode);
                    result.write_normal_vector(value.x, value.y, value.z);
                    break;
                case WVT_BLOB:
                    result.WriteMini((uchar) WVT_BLOB);
                    result.WriteMini((uchar) mode);
                    result.WriteMini(bytes2Write);
                    result.write_aligned_bytes((const uchar*) value, bytes2Write);
                    break;
                default:
                    printf("read_watcher_value()::no such watcher value type (%d)!\n", type);
                    abort();
                    break;
            }
        }/*protocol v2 ends*/

        /**
         *  @brief This class is the base class for all debug value watchers.
         *   It is part of the@ref WatcherModule.
         *  @ingroup WatcherModule
         */
        class geco_watcher_base_t
        {
            protected:
                const char* comment_;

            public:
                /// @name Construction / Destruction
                //@{
                /// Constructor.
                geco_watcher_base_t(const char* comment = "geco_watcher_base_t ctor")
                        : comment_(comment)
                {
                }
                virtual ~geco_watcher_base_t()
                {
                }
                //@}
                void set_comment(const char* comment = "")
                {
                    comment_ = comment;
                }
                const char* get_comment()
                {
                    return comment_;
                }
                /// @name Methods to be overridden
                //@{
                /**
                 *  @brief This method is used to get a string representation of the value
                 *  associated with the path. The path is relative to this watcher.
                 *  @param base
                 *  This is a pointer used by a number of Watchers. It isused as an offset into a struct or container.
                 *  @param path
                 *  The path string used to find the desired watcher.
                 *   If the path is the empty string, the value associated with this watcher is used.
                 *  @param result   A reference to a string where the resulting string will  be placed.
                 *  @param desc     A reference to a string that contains description of this watcher.
                 *  @param mode     A reference to a watcher mode. It is set to the mode of the returned value.
                 *  @return True if successful, otherwise false.
                 */
                virtual bool get_value_to_string(const void * base, const char * path, std::string & result,
                        std::string & desc, WatcherMode & mode) const
                {
                    return false;
                }
                /**
                 *  This method is used to get a stream representation of the value
                 *  associated with the path. The path is relative to this watcher.
                 *  The stream is comprised of
                 *  [data type] - (string/bool/int ...)
                 *  [data mode] - (read only/read write)
                 *  [data]
                 *
                 *  @param base     This is a pointer used by a number of Watchers. It is
                 *                  used as an offset into a struct or container.
                 *  @param path     The path string used to find the desired watcher. If
                 *                  the path is the empty string, the value associated with
                 *                  this watcher is used.
                 *  @param pathRequest  A reference to a Watcher protocol v2 PathRequest
                 *              containing the stream that we store the watcher value in.
                 *
                 *  @return True if successful, otherwise false.
                 */
                virtual bool get_value_to_stream(const void * base, const char * path,
                        watcher_path_request_v2 & pathRequest) const
                {
                    return false;
                }

                /**
                 *  @brief This method sets the value of the watcher associated with the input
                 *  path from a string. The path is relative to this watcher.
                 *  @param base This is a pointer used by a number of Watchers.
                 *   It isused as an offset into a struct or container.
                 *  @param path     The path string used to find the desired watcher. If
                 *   the path is the empty string, the value associated with this watcher is set.
                 *  @param valueStr
                 *  A string representing the value which will be used to set to watcher's value.
                 *  @return True if the new value was set, otherwise false.
                 */
                virtual bool set_value_from_string(void * base, const char * path, const char * valueStr)
                {
                    return false;
                }
                /**
                 *  @brief This method sets the value of the watcher associated with the input path from a PathRequest.
                 *  The Path Request is an abstraction of the data associated with watcher path being queried.
                 *  @param[in] base
                 *   This is a pointer used by a number of Watchers. It isused as an offset into a struct or container.
                 *  @param[in] path     The path string used to find the desired watcher. If
                 *  the path is the empty string, the value associated with this watcher is set.
                 *  @param pathRequest[in]  A reference to the PathRequest object to use when setting the watcher's value.
                 *  @return True if the new value was set, otherwise false.
                 *   @note For asynchronous watcher set requests this method will always return success.
                 */
                virtual bool set_value_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
                {
                    return false;
                }
                /**
                 *  @brief This method adds a watcher as a child to another watcher.
                 *  @param path     The path of the watcher to add the child to.
                 *  @param pChild   The watcher to add as a child.
                 *  @param withBase This is a pointer used by a number of Watchers. It is
                 *                  used as an offset into a struct or container.
                 *  @return True if successful, otherwise false. This method may fail if the
                 *          watcher could not be found or the watcher cannot have children.
                 */
                virtual bool add_watcher(const char * path, geco_watcher_base_t& pChild, void * withBase = NULL)
                {
                    return false;
                }
                /**
                 *  This method removes a child identifier in the path string.
                 *
                 *  @param path     This string must start with the identifier that you are
                 *                  searching for. For example, "myDir/myValue" would match the
                 *                  child with the label "myDir".
                 *
                 *  @return true if remove, false if not found
                 */
                virtual bool remove_watcher(const char * path)
                {
                    return false;
                }

                /**
                 *  This method visits each child of a directory watcher.
                 *  @param base     This is a pointer used by a number of Watchers. It is
                 *                  used as an offset into a struct or container.
                 *  @param path     The path string used to find the desired watcher. If
                 *                  the path is the empty string, the children of this
                 *                  object are visited.
                 *  @param pathRequest  The path request object to notify for each child
                 *                  owned by the directory watcher.
                 *  @return True if the children were visited, false if specified watcher
                 *      could not be found or is not a directory watcher.
                 */

                virtual bool visit_children(const void * base, const char *path, watcher_path_request& pathRequest)
                {
                    return false;
                }

                /**
                 *  This method visits each child of a directory watcher.
                 *
                 * NB: This method is a wrapper for the path request implementation of
                 * visitChildren.
                 *
                 *  @param base     This is a pointer used by a number of Watchers. It is
                 *                  used as an offset into a struct or container.
                 *  @param path     The path string used to find the desired watcher. If
                 *                  the path is the empty string, the children of this
                 *                  object are visited.
                 *  @param visitor  An object derived from WatcherVisitor whose visit method
                 *                  will be called for each child.
                 *
                 *  @return True if the children were visited, false if specified watcher
                 *      could not be found or is not a directory watcher.
                 */
                bool visit_children(const void * base, const char * path, watcher_visitor_t & visitor)
                {
                    return false;
                }

                //@}
                /// @name Static methods
                //@{
                /**
                 *  This method returns the root watcher.
                 *  @return reference to the root watcher.
                 */
                static geco_watcher_base_t& get_root_watcher();
                static void destroy_root_watcher();
                /**
                 *  This is a simple helper method used to partition a path string into the name
                 *  and directory strings.
                 *  @param[in] path             The string to be partitioned.
                 *  @parWatcherPathRequestV2am[out] name    A reference to a string that is to receive the base name.
                 *  @param[out] dir   A reference to a string that is to receive the directory string.
                 *  @example  "section1/section/hello" would be split  into "hello" and "section1/section2/".
                 */
                static void partition_path(const std::string path, std::string & name, std::string & dir);

            protected:
                /**
                 *  This method is a helper that returns whether or not the input path is
                 *  an empty path. An empty path is an empty string or a NULL string.
                 *  @param path The path to test.
                 *  @return True if the path is <i>empty</i>, otherwise false.
                 */
                static bool is_empty_path(const char * path)
                {
                    return (path == NULL) || (*path == '\0');
                }
                static bool is_doc_path(const char * path)
                {
                    if (path == NULL) return false;
                    return (strcmp(path, "__doc__") == 0);
                }
                //@}
        };

        /**
         *  This class implements a Watcher that can contain other Watchers.
         *  It is used by the watcher module to implement the tree of watchers.
         *  To find a watcher associated with a given path,
         *   you traverse this tree in a way similar to traversing a directory structure.
         *  @see WatcherModule
         *  @ingroup WatcherModule
         */
        struct watcher_directory_t
        {
                geco_watcher_base_t* watcher;
                void * base;
                std::string label;
        };
        class geco_watcher_director_t: public geco_watcher_base_t
        {
            private:
                typedef std::vector<watcher_directory_t> Container;
                Container container_;

                /**
                 *  @brief
                 *  This method finds the immediate child of this directory matching the first identifier in the path string.
                 *  @param path
                 *   This string must start with the identifier that you are
                 *   searching for. For example, "myDir/myValue" would match the
                 *   child with the label "myDir".
                 *  @return The watcher matching the input path. NULL if one was not found.
                 */
                watcher_directory_t* find_child(const char * path) const;

            public:
                virtual bool add_watcher(const char * path, geco_watcher_base_t& pChild, void * withBase = NULL);
                virtual bool remove_watcher(const char * path);
                virtual bool set_value_from_string(void * base, const char * path, const char * valueStr);
                virtual bool set_value_from_stream(void * base, const char * path,
                        watcher_path_request_v2& pathRequest);
                virtual bool get_value_to_string(const void * base, const char * path, std::string & result,
                        std::string & desc, WatcherMode & mode) const;
                virtual bool get_value_to_stream(const void * base, const char * path,
                        watcher_path_request_v2 & pathRequest) const;
                /**
                 *  used to find the string representing the path without the initial identifier.
                 *  @param path     The path that you want to find the tail of.
                 *  @return A string that represents the remainder of the path.
                 */
                static const char * get_path_tail(const char * path)
                {
                    if (path == NULL) return NULL;
                    char * pSeparator = strchr((char*) path, WATCHER_PATH_SEPARATOR);
                    if (pSeparator == NULL) return NULL;
                    return pSeparator + 1;
                }
        };

        /**
         *  This templatised class is used to watch a given data.
         *  @see WatcherModule
         *  @ingroup WatcherModule
         */
        template<class TYPE>
        class value_watcher_t: public geco_watcher_base_t
        {
            private:
                TYPE & rValue_;
                WatcherMode access_;
                WatcherValueType valtype_;
                uint size_;

            public:
                /// @name Construction/Destruction
                //@{
                /**
                 *  Constructor. Leave 'path' as NULL to prevent the watcher adding
                 *  itself to the root watcher. In the normal case, the MF_WATCH style
                 *  macros should be used, as they do error checking and this
                 *  constructor cannot (it's certainly not going to delete itself
                 *  anyway).
                 */
                explicit value_watcher_t(WatcherValueType valtype, TYPE & rValue, uint size = 0, WatcherMode access =
                        WT_READ_ONLY, const char * path = NULL)
                        : rValue_(rValue), access_(access), valtype_(valtype), size_(size)
                {
                    if (access_ != WT_READ_WRITE && access_ != WT_READ_ONLY) access_ = WT_READ_ONLY;
                    if (path != NULL) geco_watcher_base_t::get_root_watcher().add_watcher(path, this);
                }
                //@}

                virtual bool set_value_from_string(void * base, const char * path, const char * valueStr)
                {
                    if (geco_watcher_director_t::is_empty_path(path) && access_ == WT_READ_WRITE)
                    {
                        const TYPE & useValue = *(const TYPE*) (((const uintptr) &rValue_) + ((const uintptr) base));
                        return read_watcher_value(valueStr, useValue);
                    }
                    else
                    {
                        return false;
                    }
                }
                virtual bool set_value_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
                {
                    if (geco_watcher_director_t::is_empty_path(path) && access_ == WT_READ_WRITE)
                    {
                        const TYPE & useValue = *(const TYPE*) (((const uintptr) &rValue_) + ((const uintptr) base));
                        if (read_watcher_value(pathRequest.get_value_stream(), valtype_, useValue, access_))
                        {
                            write_watcher_value(pathRequest.get_result_stream(), valtype_, useValue, access_, size_);
                            pathRequest.set_result_stream(comment_, access_, this, base);
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                virtual bool get_value_to_string(const void * base, const char * path, std::string & result,
                        std::string & desc, WatcherMode & mode) const
                {
                    if (geco_watcher_director_t::is_empty_path(path))
                    {
                        const TYPE & useValue = *(const TYPE*) (((const uintptr) &rValue_) + ((const uintptr) base));
                        result = write_watcher_value(useValue);
                        mode = access_;
                        desc = this->comment_;
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                virtual bool get_value_to_stream(const void * base, const char * path,
                        watcher_path_request_v2 & pathRequest) const
                {
                    if (geco_watcher_director_t::is_empty_path(path))
                    {
                        const TYPE & useValue = *(const TYPE*) (((const uintptr) &rValue_) + ((const uintptr) base));
                        write_watcher_value(pathRequest.get_result_stream(), valtype_, useValue, access_, size_);
                        pathRequest.set_result_stream(comment_, access_, this, base);
                        return true;
                    }
                    else if (geco_watcher_director_t::is_doc_path(path))
                    {
                        write_watcher_value(pathRequest.get_result_stream(), WVT_STRING, comment_, WT_READ_ONLY, size_);
                        pathRequest.set_result_stream(comment_, WT_READ_ONLY, this, base);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
        };

        template<class TYPE>
        geco_watcher_base_t* create_value_watcher(TYPE & rValue, WatcherMode access = WT_READ_ONLY)
        {
            return new value_watcher_t<TYPE>(rValue, access);
        }
        template<class CLASS, class TYPE>
        geco_watcher_base_t* create_value_watcher(TYPE CLASS::*memberPtr, WatcherMode access = WT_READ_ONLY)
        {
            CLASS * pNull = NULL;
            return new value_watcher_t<TYPE>(pNull->*memberPtr, access);
        }

#endif
    }
}

#endif /* SRC_COMMON_DEBUGGING_GECOWATCHERT_H_ */
