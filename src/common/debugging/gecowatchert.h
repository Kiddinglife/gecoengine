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
  *   TODO on 29 Jul, 2016, combine reqv1 and reqv2.
  *
  * see unit test  test-watcher.cc in ubittests project for details how it is used
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

#include "debug.h"
using namespace geco::debugging;
using namespace std::placeholders;

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
            WVT_INT8,
            WVT_UINT8,
            WVT_INT16,
            WVT_UINT16,
            WVT_INT32,
            WVT_UINT32,
            WVT_INT64,
            WVT_UINT64,
            WVT_FLOAT,
            WVT_DOUBLE,
            WVT_STRING,
            WVT_TUPLE,
            WVT_TYPE,
            /*
             Add the identifier like WVT_USER_DEFINED
             any defined complicated types  can be divided into basic types and tranfered
             for the sake of easyness, for any user defined types with operator << and >>,
             user MUST impl a pair of write_value_to_stream and read_value_from_stream in this file

             @example
             struct ExampleClass
             {
             int a;
             };
             geco_bit_stream_t& operator << (geco_bit_stream_t &os, const ExampleClass &d)
             {
             os.WriteMini(d.a);
             return os;
             }
             geco_bit_stream_t& operator >> (geco_bit_stream_t &is,  ExampleClass &d)
             {
             is.ReadMini(d.a);
             return is;
             }
             // todo dd more write and read for any other types that have gloval operator << and >>
             inline void write_watcher_value_to_stream(geco_bit_stream_t & result,ExampleClass& value)
             {
             uchar tmp;
             result.WriteMini((uchar)type);
             result.WriteMini((uchar)mode);
             result << value;
             }
             inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, ExampleClass& value,WatcherMode& mode)
             {
             result.WriteMini((uchar)WVT_USER_DEFINED);
             result.WriteMini((uchar)mode);
             result >> value;
             }
             */
            WVT_UNKNOWN,
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

        /*
         * Interface to allow a WatcherPathRequest to notify
         * a calling context of successfull completion.
         * Notify the implementing class of a path request completion.
         * @param pathRequest The WatcherPathRequest that has been completed.
         * @param count       The number of replies contained in the path request.
         */
        typedef std::function<void(watcher_path_request & pathRequest, int32 count)> on_wt_path_req_complete_t;

        /**
         * Create a new WatcherPathRequest from  the current packet
         * @param path The watcher path the request will query.
         * @returns A pointer to a WatcherPathRequest on success, NULL on error.
         */
         //  watcher_path_request * create_request(std::string & path);
         /**
          * WatcherPathRequest is a handler class to allow an asyncronous request of a single watcher path.
          */
        struct watcher_path_request
        {
            std::string request_path_;
            // Output stream for the watcher path requests data
            geco_bit_stream_t result_;
            on_wt_path_req_complete_t on_complete_cb_;

            watcher_path_request(const char* path)
                : request_path_(path)
            {
            }
            watcher_path_request(const char* path, const on_wt_path_req_complete_t& on_complete_cb)
                : request_path_(path), on_complete_cb_(on_complete_cb)
            {
            }
            virtual ~watcher_path_request()
            {
            }
            /// @brief Initiate watcher value retreival for the current path request.
            virtual void get_watcher_value()
            {
            }
            /**
             * Initiate a watcher path set operation after data to use
             * has been placed into us via @see set_result_stream().
             * @returns true on success, false on failure.
             */
            virtual bool set_watcher_value()
            {
                return false;
            }
            const std::string & get_request_path() const
            {
                return this->request_path_;
            }
            ///  Setup the pointer back to the creator of this class to notify on completion.
            void set_request_complete_cb(const on_wt_path_req_complete_t& cb)
            {
                on_complete_cb_ = cb;
            }
            /// notify upper layer  of our completion.
            void notify(int32 replies = 1)
            {
                on_complete_cb_(*this, replies);
            }
            /// Notification of a child watcher entry as called from visitChildren.*/
            virtual bool add_watcher_path(const void *base, const char *path, std::string & label,
                geco_watcher_base_t &watcher)
            {
                return false;
            }
            geco_bit_stream_t& get_result_stream()
            {
                return result_;
            }
        };

        struct watcher_path_request_v1 : public watcher_path_request
        {
            std::string setopt_string_;  //!< String used in set operations for this path request
            uint replies_count_;  //!< Number of watcher path replies contained in this path request.
            bool use_desc_;  //!< True if the watcher request reply should contain a description.

            watcher_path_request_v1(const char* path, bool use_use_desc = false)
                : watcher_path_request(path), replies_count_(1), use_desc_(use_use_desc)
            {
            }
            virtual bool set_watcher_value();
            virtual void get_watcher_value();

            /*
             *  This method is called once for each child of a directory watcher when
             *  the visit_children member is called. This function can return false
             *  to stop any further visits.
             *  @see Watcher::visit_children
             */
            bool on_visit_dirwt_child(WatcherMode, const std::string & label, const std::string & desc,
                const std::string & valueStr);
            virtual bool add_watcher_path(const void *base, const char *path, std::string & label,
                geco_watcher_base_t &watcher);
            //  set a value to be used to in setopt of  watcher value
            void set_setopt_string(const char *valueStr)
            {
                setopt_string_ = valueStr;
            }
        };
        struct watcher_path_request_v2 : public watcher_path_request
        {
            // Data obtained from the watcher nub to use in a set operation.
            char *set_data_;
            // Input stream for the watcher path requests data
            geco_bit_stream_t set_stream_;
            // Original path requested via watcher nub, used when visiting directories and requestPath_ may be altered.
            std::string origin_request_path_;

            watcher_path_request_v2(const char* path)
                : watcher_path_request(path), set_data_(NULL)
            {
            }
            virtual ~watcher_path_request_v2()
            {
            }
            virtual void get_watcher_value();
            virtual bool set_watcher_value();

            // Methods used by visit_children()
            virtual bool add_watcher_path(const void *base, const char *path, std::string & label,
                geco_watcher_base_t &watcher);

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
            /**
             * Return a pointer to the complete data set that should be used
             * to create the watcher packet response.
             */
            const char *get_data()
            {
                return result_.char_data();
            }
            /// Return the size of the data that is returned by get_data().
            int32 get_data_size()
            {
                return BITS_TO_BYTES(result_.get_payloads());
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
        bool watcher_string_to_value(const char * valueStr, VALUE_TYPE &value)
        {
            std::stringstream stream;
            stream.write(valueStr, std::streamsize(strlen(valueStr)));
            stream.seekg(0, std::ios::beg);
            stream >> value;
            return !stream.bad();
        }
        inline bool watcher_string_to_value(const char * valueStr, bool & value)
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
                return false;
            }
            return true;
        }
        inline bool watcher_string_to_value(const char * valueStr, const char *& value)
        {
            value = valueStr;
            return true;
        }
        inline bool watcher_string_to_value(const char * valueStr, std::string & value)
        {
            value = valueStr;
            return true;
        }
        // get operations of protocol v1
        template<class VALUE_TYPE>
        std::string watcher_value_to_string(const VALUE_TYPE & value)
        {
            static std::stringstream stream;
            stream.str("");
            stream.clear();
            stream << value;
            return stream.str();
        }
        inline std::string& watcher_value_to_string(std::string & value)
        {
            return value;
        }
        inline std::string& watcher_value_to_string(bool value)
        {
            static std::string t = "true";
            static std::string f = "false";
            return value ? t : f;
        }/* protocol v1 ends */

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Section: Stream between Watcher Value : protocol 2
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // GET  operations of protocol v2
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, bool& value, uchar& valtype, uchar& mode,
            std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, int& value, uchar& valtype, uchar& mode,
            std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value, false);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, uint& value, uchar& valtype, uchar& mode,
            std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, uint8& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, int8& value, uchar& valtype, uchar& mode,
            std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value, false);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, uint16& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, int16& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value, false);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, uint64& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value);
            result.Read(path);
            result.Read(comment);
            return true;;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, int64& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.ReadMini(value, false);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, double& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.Read(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, float& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.Read(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, uchar* value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.Read(value);
            result.Read(path);
            result.Read(comment);
            return true;
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, char* value, uchar& valtype, uchar& mode,
            std::string& path, std::string& comment)
        {
            return read_watcher_value_from_stream(result, (uchar*)value, valtype, mode, path, comment);
        }
        inline bool read_watcher_value_from_stream(geco_bit_stream_t & result, std::string& value, uchar& valtype,
            uchar& mode, std::string& path, std::string& comment)
        {
            result.ReadMini(valtype);
            result.ReadMini(mode);
            result.Read(value);
            result.Read(path);
            result.Read(comment);
            if (valtype != WVT_STRING) return false;
            return true;
        }
        // SET operations of protocol v2
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const int8 value, const WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT8);
            result.WriteMini((uchar)mode);
            result.WriteMini(value, false);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const uint8 value, const WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT8);
            result.WriteMini((uchar)mode);
            result.WriteMini(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const int16 value, const WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT16);
            result.WriteMini((uchar)mode);
            result.WriteMini(value, false);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const uint16 value,
            const WatcherMode mode, const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT16);
            result.WriteMini((uchar)mode);
            result.WriteMini(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const int64 value, const WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT64);
            result.WriteMini((uchar)mode);
            result.WriteMini(value, false);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const uint64 value,
            const WatcherMode mode, const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT64);
            result.WriteMini((uchar)mode);
            result.WriteMini(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const int value, WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT32);
            result.WriteMini((uchar)mode);
            result.WriteMini(value, false);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const uint value, WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT32);
            result.WriteMini((uchar)mode);
            result.WriteMini(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const float value, WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT32);
            result.WriteMini((uchar)mode);
            result.Write(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const double value, WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_INT32);
            result.WriteMini((uchar)mode);
            result.Write(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const std::string& value,
            WatcherMode mode, const std::string& path, const std::string& comment)
        {
            result.WriteMini((uchar)WVT_STRING);
            result.WriteMini((uchar)mode);
            result.Write(value);
            result.Write(path);
            result.Write(comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, char* value, WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            static std::string buf;
            buf.clear();
            buf = value;
            write_watcher_value_to_stream(result, buf, mode, path, comment);
        }
        inline void write_watcher_value_to_stream(geco_bit_stream_t & result, const char* value, WatcherMode mode,
            const std::string& path, const std::string& comment)
        {
            return write_watcher_value_to_stream(result, (char*)value, mode, path, comment);
        }/*protocol v2 ends*/

        /**
         *  @brief This class is the base class for all debug value watchers.
         *   It is part of the@ref WatcherModule.
         *  @ingroup WatcherModule
         */
        class geco_watcher_base_t
        {
        public:
            const char* comment_;

        public:
            /// @name Construction / Destruction
            //@{
            /// Constructor.
            geco_watcher_base_t(const char* comment = "None")
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
            virtual bool get_as_string(const void * base, const char * path, std::string & result,
                std::string & desc, WatcherMode & mode)
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
            virtual bool get_as_stream(const void * base, const char * path, watcher_path_request_v2 & pathRequest)
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
             *  @return True if the new value was set, otherwise return false if watcher with the assciated path not exist.
             */
            virtual bool set_from_string(void * base, const char * path, const char * valueStr)
            {
                return false;
            }
            /**
             *  @brief This method sets the value of the watcher associated with the input path from a PathRequest.
             *  The Path Request is an abstraction of the data associated with watcher path being queried.
             *  @pre the watcher with the assciated path must exist
             *  @param[in] base
             *   This is a pointer used by a number of Watchers. It isused as an offset into a struct or container.
             *  @param[in] path     The path string used to find the desired watcher. If
             *  the path is the empty string, the value associated with this watcher is set.
             *  @param pathRequest[in]  A reference to the PathRequest object to use when setting the watcher's value.
             *  @return True if the new value was set, otherwise return false if watcher with the assciated path not exist..
             *   @note For asynchronous watcher set requests this method will always return success.
             */
            virtual bool set_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
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
             *  @caution it is caller's responsibility to free the watcher
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
                return true;
            }

            virtual void walk_all_files(std::string& path, std::string& buf, int& num, bool print)
            {
                num++;
                path.replace(path.length() - 1, 1, "\0");
                path.append("\n");
                buf.append(path);
                if (print) printf("%s", path.c_str());
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
        class geco_watcher_director_t : public geco_watcher_base_t
        {
        private:
            typedef std::vector<watcher_directory_t> Container;
            Container container_;

        public:
            /**
             *  @brief
             *  This method finds the immediate child of this directory matching the first identifier in the path string.
             *  @param path
             *   This string must start with the identifier that you are
             *   searching for. For example, "myDir/myValue" would match the
             *   child with the label "myDir".
             *  @return The watcher matching the input path. NULL if one was not found.
             */
            watcher_directory_t* find_child(const char * path) const
            {
                if (path == NULL) return NULL;
                char* pseparator = strchr((char*)path, WATCHER_PATH_SEPARATOR);
                uint cmp_len = (pseparator == NULL) ? strlen(path) : (pseparator - path);

                //    char* label = new char[cmp_len + 1];
                //    label[cmp_len] = '\0';
                //    memcpy(label, path, cmp_len);
                //    VERBOSE_MSG("search for  child label %s\n", label);

                watcher_directory_t* ptr = NULL;
                if (cmp_len != 0)
                {
                    auto iter = container_.begin();
                    while (iter != container_.end() && ptr == NULL)
                    {
                        if (cmp_len == (*iter).label.length() && strncmp(path, (*iter).label.c_str(), cmp_len) == 0)
                        {
                            ptr = (watcher_directory_t*)(&(*iter));
                        }
                        ++iter;
                    }
                }
                return ptr;
            }
            virtual bool add_watcher(const char * path, geco_watcher_base_t& pChild, void * withBase = NULL);
            virtual bool remove_watcher(const char * path);
            virtual bool set_from_string(void * base, const char * path, const char * valueStr);
            virtual bool set_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest);
            virtual bool get_as_string(const void * base, const char * path, std::string & result,
                std::string & desc, WatcherMode & mode);
            virtual bool get_as_stream(const void * base, const char * path, watcher_path_request_v2 & pathRequest);
            virtual bool visit_children(const void * base, const char *path, watcher_path_request& pathRequest);

            /**
             *  used to find the string representing the path without the initial identifier.
             *  @param path     The path that you want to find the tail of.
             *  @return A string that represents the remainder of the path.
             */
            static const char * get_path_tail(const char * path)
            {
                if (path == NULL) return NULL;
                char * pSeparator = strchr((char*)path, WATCHER_PATH_SEPARATOR);
                if (pSeparator == NULL) return NULL;
                return pSeparator + 1;
            }

            virtual void walk_all_files(std::string& path, std::string& buf, int& num, bool print)
            {
                std::string pathold = path;
                auto iter = container_.begin();
                while (iter != container_.end())
                {
                    path += (*iter).label + "/";
                    iter->watcher->walk_all_files(path, buf, num, print);
                    path = pathold;
                    ++iter;
                }
            }
        };

        /**
         *  This templatised class is used to watch a given data.
         *  @see WatcherModule
         *  @ingroup WatcherModule
         */
        template<class TYPE>
        class value_watcher_t : public geco_watcher_base_t
        {
        private:
            TYPE & rValue_;
            WatcherMode access_;

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
            explicit value_watcher_t(TYPE & rValue, WatcherMode access = WT_READ_ONLY, const char * path = NULL)
                : rValue_(rValue), access_(access)
            {
                if (access_ != WT_READ_WRITE && access_ != WT_READ_ONLY) access_ = WT_READ_ONLY;
                /*if (path != NULL) geco_watcher_base_t::get_root_watcher().add_watcher(path, *this);*/
            }
            //@}

            virtual bool set_from_string(void * base, const char * path, const char * valueStr)
            {
                if (geco_watcher_director_t::is_empty_path(path) && access_ == WT_READ_WRITE)
                {
                    TYPE & useValue = *(TYPE*)(((const uintptr)&rValue_) + ((const uintptr)base));
                    return watcher_string_to_value(valueStr, useValue);
                }
                return false;
            }
            virtual bool set_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
            {
                bool ret = false;
                if (geco_watcher_director_t::is_empty_path(path) && access_ == WT_READ_WRITE)
                {
                    uchar valtype;
                    uchar mode;
                    static std::string mypath;
                    static std::string comment;
                    mypath.clear();
                    comment.clear();
                    TYPE useValue;
                    if (read_watcher_value_from_stream(pathRequest.get_value_stream(), useValue, valtype, mode,
                        mypath, comment))
                    {
                        if (valtype != WT_DIRECTORY)
                        {
                            if (mode == WT_READ_WRITE)
                            {
                                ret = true;
                                // push the result into the reply stream
                                // assuming mode RW because we have already set
                                if (comment != "None" && comment != "") comment_ = comment.c_str();
                                if (!mypath.empty() && mypath != "" && mypath != pathRequest.request_path_)
                                {
                                    std::string::size_type pos = pathRequest.request_path_.find_last_of('/');
                                    ((geco_watcher_director_t&)geco_watcher_base_t::get_root_watcher()).find_child(
                                        pathRequest.request_path_.c_str())->label =
                                        pathRequest.request_path_.substr(pos);
                                }
                                pathRequest.get_result_stream().reset();
                                write_watcher_value_to_stream(pathRequest.get_result_stream(), useValue, access_,
                                    mypath, comment);
                                pathRequest.notify();
                            }
                            else
                            {
                                ret = false;
                            }
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
                return ret;
            }
            virtual bool get_as_string(const void * base, const char * path, std::string & result,
                std::string & desc, WatcherMode & mode)
            {
                if (geco_watcher_director_t::is_empty_path(path))                //this is visit children called
                {
                    const TYPE & useValue = *(const TYPE*)(((const uintptr)&rValue_) + ((const uintptr)base));
                    result = watcher_value_to_string(useValue);
                    mode = access_;
                    desc = comment_;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            virtual bool get_as_stream(const void * base, const char * path, watcher_path_request_v2 & pathRequest)
            {
                if (geco_watcher_director_t::is_empty_path(path))  //this is visit children called
                {
                    const TYPE & useValue = *(const TYPE*)(((const uintptr)&rValue_) + ((const uintptr)base));
                    write_watcher_value_to_stream(pathRequest.get_result_stream(), useValue, access_,
                        pathRequest.get_request_path(), comment_);
                    DEBUG_MSG("write value watcher  [%d,%s,%s]\n", access_, pathRequest.get_request_path().c_str(),
                        watcher_value_to_string(useValue).c_str());
                    pathRequest.notify();
                    return true;
                }
                else if (geco_watcher_director_t::is_doc_path(path))
                {
                    write_watcher_value_to_stream(pathRequest.get_result_stream(), comment_, WT_READ_ONLY,
                        pathRequest.get_request_path(), comment_);
                    pathRequest.notify();
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };

        /**
         *	This templatised class is used to watch the result of a given function.
         *	@see WatcherModule
         *	@ingroup WatcherModule
         */
        template<class RETURN_TYPE>
        class func_watcher_t : public geco_watcher_base_t
        {
        private:
            RETURN_TYPE&(*getFunction_)();
            void(*setFunction_)(RETURN_TYPE&);

        public:
            /// @name Construction/Destruction
            //@{
            /**
             *	Constructor. Leave 'path' as NULL to prevent the watcher adding
             *	itself to the root watcher. In the normal case, the MF_WATCH style
             *	macros should be used, as they do error checking and this
             *	constructor cannot (it's certainly not going to delete itself
             *	anyway).
             */
            explicit func_watcher_t(RETURN_TYPE&(*getFunction)(), void(*setFunction)(RETURN_TYPE&) = NULL,
                const char * path = NULL)
                : getFunction_(getFunction), setFunction_(setFunction)
            {
                /*	if (path != NULL)
                 geco_watcher_base_t::get_root_watcher().add_watcher(pth, *this, NULL);*/
            }
            //@}

            virtual bool set_from_string(void * base, const char * path, const char * valueStr)
            {
                if (geco_watcher_director_t::is_empty_path(path) && setFunction_ != NULL)
                {
                    RETURN_TYPE useValue;
                    if (watcher_string_to_value(valueStr, useValue))
                    {
                        (*setFunction_)(useValue);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                return false;
            }
            virtual bool set_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
            {
                bool ret = false;
                if (geco_watcher_director_t::is_empty_path(path) && (setFunction_ != NULL))
                {
                    uchar valtype;
                    uchar mode;
                    static std::string mypath;
                    static std::string comment;
                    mypath.clear();
                    comment.clear();
                    RETURN_TYPE useValue;
                    if (read_watcher_value_from_stream(pathRequest.get_value_stream(), useValue, valtype, mode,
                        mypath, comment))
                    {
                        if (valtype != WT_DIRECTORY)
                        {
                            if (mode == WT_READ_WRITE)
                            {
                                ret = true;
                                (*setFunction_)(useValue);
                                // push the result into the reply stream
                                // assuming mode RW because we have already set
                                if (comment != "None" && comment != "") comment_ = comment.c_str();
                                if (!mypath.empty() && mypath != "" && mypath != pathRequest.request_path_)
                                {
                                    std::string::size_type pos = pathRequest.request_path_.find_last_of('/');
                                    ((geco_watcher_director_t&)geco_watcher_base_t::get_root_watcher()).find_child(
                                        pathRequest.request_path_.c_str())->label =
                                        pathRequest.request_path_.substr(pos);
                                }
                                write_watcher_value_to_stream(pathRequest.get_result_stream(), useValue,
                                    WT_READ_WRITE, mypath, comment);
                                pathRequest.notify();
                            }
                            else
                            {
                                ret = false;
                            }
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
                return ret;
            }

            virtual bool get_as_string(const void * base, const char * path, std::string & result,
                std::string & desc, WatcherMode & mode)
            {
                if (geco_watcher_director_t::is_empty_path(path))
                {
                    result = watcher_value_to_string((*getFunction_)());
                    mode = (setFunction_ != NULL) ? WT_READ_WRITE : WT_READ_ONLY;
                    desc = this->comment_;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            virtual bool get_as_stream(const void * base, const char * path, watcher_path_request_v2 & pathRequest)
            {
                if (geco_watcher_director_t::is_empty_path(path))
                {
                    WatcherMode mode = (setFunction_ != NULL) ? WT_READ_WRITE : WT_READ_ONLY;
                    write_watcher_value_to_stream(pathRequest.get_result_stream(), (*getFunction_)(), mode,
                        pathRequest.get_request_path(), comment_);
                    DEBUG_MSG("write func watcher [%d,%s,%s]\n", mode, pathRequest.get_request_path().c_str(),
                        watcher_value_to_string((*getFunction_)()).c_str());
                    pathRequest.notify();
                    return true;
                }
                else if (geco_watcher_director_t::is_doc_path(path))
                {
                    write_watcher_value_to_stream(pathRequest.get_result_stream(), comment_, WT_READ_ONLY,
                        pathRequest.get_request_path(), comment_);
                    pathRequest.notify();
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };

        /**
         *	This templatised class is used to store debug values that are a member of a
         *	class.
         *
         *	@see WatcherModule
         *	@ingroup WatcherModule
         */
        template<class RETURN_TYPE, class OBJECT_TYPE>
        class method_watcher_t : public geco_watcher_base_t
        {
        private:
            typedef RETURN_TYPE& (OBJECT_TYPE::*GetMethodType)();
            typedef void (OBJECT_TYPE::*SetMethodType)(RETURN_TYPE&);

            OBJECT_TYPE * pObject_;
            GetMethodType getMethod_;
            SetMethodType setMethod_;

        public:
            /// @name Construction/Destruction
            //@{
            /**
             *	Constructor.
             *	This constructor should only be called from the Debug::addValue
             *	function.
             */
            method_watcher_t(GetMethodType getMethod, SetMethodType setMethod = NULL, const char * path = NULL)
                : pObject_(NULL), getMethod_(getMethod), setMethod_(setMethod)
            {
                /*	if (path != NULL)
                 geco_watcher_base_t::get_root_watcher().add_watcher(pth, *this, NULL);*/
            }

            method_watcher_t(OBJECT_TYPE & rObject, GetMethodType getMethod, SetMethodType setMethod =
                NULL, const char * path = NULL)
                : pObject_(&rObject), getMethod_(getMethod), setMethod_(setMethod)
            {
                //if (path != NULL)
                //	geco_watcher_base_t::get_root_watcher().add_watcher(pth, *this, NULL);
            }

            virtual bool set_from_string(void * base, const char * path, const char * valueStr)
            {
                if (geco_watcher_director_t::is_empty_path(path) && (setMethod_ != NULL))
                {
                    RETURN_TYPE useValue;
                    if (watcher_string_to_value(valueStr, useValue))
                    {
                        OBJECT_TYPE & useObject = *(OBJECT_TYPE*)(((uintptr)pObject_) + ((uintptr)base));
                        (useObject.*setMethod_)(useValue);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                return false;
            }
            virtual bool set_from_stream(void * base, const char * path, watcher_path_request_v2& pathRequest)
            {
                bool ret = false;
                if (geco_watcher_director_t::is_empty_path(path) && (setMethod_ != NULL))
                {
                    uchar valtype;
                    uchar mode;
                    static std::string mypath;
                    static std::string comment;
                    mypath.clear();
                    comment.clear();
                    RETURN_TYPE useValue;
                    if (read_watcher_value_from_stream(pathRequest.get_value_stream(), useValue, valtype, mode,
                        mypath, comment))
                    {
                        if (valtype != WT_DIRECTORY)
                        {
                            if (mode == WT_READ_WRITE)
                            {
                                ret = true;
                                OBJECT_TYPE & useObject = *(OBJECT_TYPE*)(((uintptr)pObject_) + ((uintptr)base));
                                (useObject.*setMethod_)(useValue);
                                // push the result into the reply stream
                                // assuming mode RW because we have already set
                                if (comment != "None" && comment != "") comment_ = comment.c_str();
                                if (!mypath.empty() && mypath != "" && mypath != pathRequest.request_path_)
                                {
                                    std::string::size_type pos = pathRequest.request_path_.find_last_of('/');
                                    ((geco_watcher_director_t&)geco_watcher_base_t::get_root_watcher()).find_child(
                                        pathRequest.request_path_.c_str())->label =
                                        pathRequest.request_path_.substr(pos);
                                }
                                write_watcher_value_to_stream(pathRequest.get_result_stream(), useValue,
                                    WT_READ_WRITE, mypath, comment);
                                pathRequest.notify();
                            }
                            else
                            {
                                ret = false;
                            }
                        }
                        else
                        {
                            ret = false;
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                }
                return ret;
            }

            virtual bool get_as_string(const void * base, const char * path, std::string & result,
                std::string & desc, WatcherMode & mode)
            {
                if (geco_watcher_director_t::is_empty_path(path))
                {
                    if (getMethod_ == (GetMethodType)NULL)
                    {
                        result = "getMethod_ == (GetMethodType)NULL";
                        mode = WT_READ_ONLY;
                        desc = comment_;
                        return true;
                    }
                    OBJECT_TYPE & useObject = *(OBJECT_TYPE*)(((const uintptr)pObject_) + ((const uintptr)base));
                    const RETURN_TYPE& useValue = (useObject.*getMethod_)();
                    result = watcher_value_to_string(useValue);
                    mode = (setMethod_ != NULL) ? WT_READ_WRITE : WT_READ_ONLY;
                    desc = this->comment_;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            virtual bool get_as_stream(const void * base, const char * path, watcher_path_request_v2 & pathRequest)
            {

                if (geco_watcher_director_t::is_empty_path(path))
                {
                    OBJECT_TYPE & useObject = *(OBJECT_TYPE*)(((uintptr)pObject_) + ((uintptr)base));
                    WatcherMode mode = (setMethod_ != NULL) ? WT_READ_WRITE : WT_READ_ONLY;
                    if (getMethod_ != (GetMethodType)NULL)
                    {
                        const RETURN_TYPE& useValue = (useObject.*getMethod_)();
                        write_watcher_value_to_stream(pathRequest.get_result_stream(), useValue, mode,
                            pathRequest.request_path_, comment_);
                        DEBUG_MSG("write mtd watcher [%d,%s,%s,%s]\n", mode, pathRequest.get_request_path().c_str(),
                            watcher_value_to_string(useValue).c_str());
                    }
                    else
                    {
                        write_watcher_value_to_stream(pathRequest.get_result_stream(), "None", mode,
                            pathRequest.request_path_, comment_);
                        DEBUG_MSG("write mtd watcher[%d,%s,%s]\n", mode, pathRequest.get_request_path().c_str(),
                            watcher_value_to_string("No Get Method !").c_str());
                    }
                    pathRequest.notify();
                    return true;
                }
                else if (geco_watcher_director_t::is_doc_path(path))
                {
                    write_watcher_value_to_stream(pathRequest.get_result_stream(), comment_, WT_READ_ONLY,
                        pathRequest.request_path_, comment_);
                    pathRequest.notify();
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };

        /**
         *	This function is used to add a watcher of a simple value.
         *
         *	@ingroup WatcherModule
         */
        template<class TYPE>
        geco_watcher_base_t* add_watcher(const char * path, TYPE& rValue, WatcherMode access = WT_READ_WRITE,
            const char * comment = NULL)
        {
            geco_watcher_base_t* ptr = new value_watcher_t<TYPE>(rValue, access, path);
            if (!geco_watcher_base_t::get_root_watcher().add_watcher(path, *ptr, NULL))
            {
                printf("add_value_watcher failed!\n");
                ptr = NULL;
            }
            else if (comment != NULL)
            {
                ptr->set_comment(comment);
            }
            return ptr;
        }

        /**
         *	This function is used to add a watcher of a function value.
         *
         *	@ingroup WatcherModule
         */
        template<class RETURN_TYPE>
        geco_watcher_base_t* add_watcher(const char * path, RETURN_TYPE&(*getFunction)(),
            void(*setFunction)(RETURN_TYPE&) = NULL, const char * comment = NULL)
        {
            geco_watcher_base_t* ptr = new func_watcher_t<RETURN_TYPE>(getFunction, setFunction, path);
            if (!geco_watcher_base_t::get_root_watcher().add_watcher(path, *ptr, NULL))
            {
                ptr = NULL;
            }
            else if (comment != NULL)
            {
                ptr->set_comment(comment);
            }
            return ptr;
        }

        template<class RETURN_TYPE, class OBJECT_TYPE>
        geco_watcher_base_t* add_watcher(const char * path, OBJECT_TYPE & rObject,
            RETURN_TYPE& (OBJECT_TYPE::*getMethod)(), void (OBJECT_TYPE::*setMethod)(RETURN_TYPE&) = NULL,
            const char * comment = NULL)
        {
            // WatcherPtr pNewWatcher = makeWatcher( rObject, getMethod, setMethod );
            geco_watcher_base_t* ptr = new method_watcher_t<RETURN_TYPE, OBJECT_TYPE>(rObject, getMethod, setMethod,
                path);
            if (!geco_watcher_base_t::get_root_watcher().add_watcher(path, *ptr, NULL))
            {
                ptr = NULL;
            }
            else if (comment != NULL)
            {
                ptr->set_comment(comment);
            }
            return ptr;
        }

#define CAST_METHOD_RW( TYPE, CLASS, rMETHOD,wMETHOD )							\
	static_cast< TYPE& (CLASS::*)()>(&CLASS::rMETHOD),			\
	static_cast< void (CLASS::*)(TYPE&)   >(&CLASS::wMETHOD)

#define CAST_FUNC_RW( TYPE, rFUNC,wFUNC )							\
	static_cast<TYPE& (*)()>(rFUNC),			\
	static_cast< void (*)(TYPE&)   >(wFUNC)

#define CAST_METHOD_R( TYPE, CLASS, METHOD )					\
	static_cast< TYPE& (CLASS::*)()>( NULL ),					\
	static_cast< void (CLASS::*)(TYPE&)   >( &CLASS::METHOD )

#define CAST_FUNC_R( TYPE, rFUNC )							\
	static_cast<TYPE& (*)()>(rFUNC),			\
	static_cast< void (*)(TYPE&)   >(NULL)

#define GECO_WATCH geco::debugging::add_watcher
#endif
    }
}

#endif /* SRC_COMMON_DEBUGGING_GECOWATCHERT_H_ */
