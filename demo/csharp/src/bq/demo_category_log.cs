﻿using bq;
using bq.def;
using bq.impl;
using System.Runtime.InteropServices;

/*!
 * Generated Wrapper For demo_category_log
 *
 * This is a category_log that supports attaching a category to each log entry.
 * Categories can be used to filter logs within the appender settings.
 *
 *    Usage: 
 *    bq.demo_category_log my_category_log = bq.demo_category_log::create_log(log_name, log_config);  //create a demo_category_log object with config.
 *    my_category_log.info("content");  //this is for empty category
 *  my_category_log.info(my_category_log.cat.moduleA.classB, "content"); //this is a log entry for category ModuleA.ClassB, which was generated by your Category Config File
 */

public class demo_category_log : category_log
{
    private demo_category_log() : base()
    {
    }
    
    private demo_category_log(log child_inst) : base(child_inst)
    {
    }

    /// <summary>
    /// Create a demo_category_log object
    /// </summary>
    /// <param name="name">
    ///     If the demo_category_log name is an empty string, bqLog will automatically assign you a unique name. 
    ///     If the demo_category_log name already exists, it will return the previously existing demo_category_log object and overwrite the previous configuration with the new config.
    /// </param>
    /// <param name="config">
    ///     demo_category_log config string
    /// </param>
    /// <returns>
    ///     A demo_category_log object, if create failed, the is_valid() method of it will return false
    /// </returns>
    public static new demo_category_log create_log(string name, string config)
    {
        if (string.IsNullOrEmpty(config))
        {
            return new demo_category_log();
        }
        ulong log_id = 0;
        unsafe
        {
            //char*[] is not supported in some IL2CPP old versions, so we have to use such an ugly way to do this.
            //char*[] category_names_array = new char*[categories_count];
            System.IntPtr names_array_alloc_ptr = Marshal.AllocHGlobal((int)(sizeof(byte*) * categories_count));
            byte** category_names_array_byte_ptr = (byte**)names_array_alloc_ptr.ToPointer();
            for (int i = 0; i < categories_count; ++i)
            {
                category_names_array_byte_ptr[i] = utf8_encoder.alloc_utf8_fixed_str(category_names[i]);
            }

            byte* utf8_name_bytes = utf8_encoder.alloc_utf8_fixed_str(name);
            byte* utf8_config_bytes = utf8_encoder.alloc_utf8_fixed_str(config);

            log_id = log_invoker.__api_create_log(utf8_name_bytes, utf8_config_bytes, categories_count, category_names_array_byte_ptr);

            for (int i = 0; i < categories_count; ++i)
            {
                utf8_encoder.release_utf8_fixed_str(category_names_array_byte_ptr[i]);
            }
            utf8_encoder.release_utf8_fixed_str(utf8_name_bytes);
            utf8_encoder.release_utf8_fixed_str(utf8_config_bytes);
            Marshal.FreeHGlobal(names_array_alloc_ptr);
        }
        log result = get_log_by_id(log_id);
        return new demo_category_log(result);
    }


    /// <summary>
    /// Get a demo_category_log object by it's name
    /// </summary>
    /// <param name="log_name">Name of the demo_category_log object you want to find</param>
    /// <returns>A log object, if the demo_category_log object with specific name was not found, the is_valid() method of it will return false</returns>
    public static new demo_category_log get_log_by_name(string log_name)
    {
        demo_category_log result = new demo_category_log(log.get_log_by_name(log_name));
        if(!result.is_valid())
        {
            return result;
        }
        //check categories
        if (result.get_categories_count() != demo_category_log.categories_count)
        {
            return new demo_category_log();
        }
        for (uint i = 0; i < result.get_categories_count(); ++i)
        {
            if (!demo_category_log.category_names[(int)i].Equals(result.get_categories_name_array()[(int)i]))
            {
                return new demo_category_log();
            }
        }
        return result;
    }


    ///Core log functions with category param, there are 6 log levels:
    ///verbose, debug, info, warning, error, fatal
    #region log methods for param count 0
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content)
    {
        return do_log(category, log_level.verbose, log_format_content);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content)
    {
        return do_log(category, log_level.debug, log_format_content);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content)
    {
        return do_log(category, log_level.info, log_format_content);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content)
    {
        return do_log(category, log_level.warning, log_format_content);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content)
    {
        return do_log(category, log_level.error, log_format_content);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content)
    {
        return do_log(category, log_level.fatal, log_format_content);
    }
    #endregion

    #region log methods for param count 1
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1)
    {
        return do_log(category, log_level.info, log_format_content, ref p1);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1)
    {
        return do_log(category, log_level.error, log_format_content, ref p1);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1);
    }
    #endregion

    #region log methods for param count 2
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2);
    }
    #endregion

    #region log methods for param count 3
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3);
    }
    #endregion

    #region log methods for param count 4
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4);
    }
    #endregion

    #region log methods for param count 5
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5);
    }
    #endregion

    #region log methods for param count 6
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6);
    }
    #endregion

    #region log methods for param count 7
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7);
    }
    #endregion

    #region log methods for param count 8
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8);
    }
    #endregion

    #region log methods for param count 9
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9);
    }
    #endregion

    #region log methods for param count 10
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10);
    }
    #endregion

    #region log methods for param count 11
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11);
    }
    #endregion

    #region log methods for param count 12
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11, param_wrapper p12)
    {
        return do_log(category, log_level.verbose, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11, ref p12);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11, param_wrapper p12)
    {
        return do_log(category, log_level.debug, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11, ref p12);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11, param_wrapper p12)
    {
        return do_log(category, log_level.info, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11, ref p12);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11, param_wrapper p12)
    {
        return do_log(category, log_level.warning, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11, ref p12);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11, param_wrapper p12)
    {
        return do_log(category, log_level.error, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11, ref p12);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, param_wrapper p1, param_wrapper p2, param_wrapper p3, param_wrapper p4, param_wrapper p5, param_wrapper p6, param_wrapper p7, param_wrapper p8, param_wrapper p9, param_wrapper p10, param_wrapper p11, param_wrapper p12)
    {
        return do_log(category, log_level.fatal, log_format_content, ref p1, ref p2, ref p3, ref p4, ref p5, ref p6, ref p7, ref p8, ref p9, ref p10, ref p11, ref p12);
    }
    #endregion

    #region log methods for more params. but call this method will cause GC(Heap) alloc
    public unsafe bool verbose(demo_category_log_category_base category, string log_format_content, params param_wrapper[] args)
    {
        return do_log(category, log_level.verbose, log_format_content, args);
    }
    public unsafe bool debug(demo_category_log_category_base category, string log_format_content, params param_wrapper[] args)
    {
        return do_log(category, log_level.debug, log_format_content, args);
    }
    public unsafe bool info(demo_category_log_category_base category, string log_format_content, params param_wrapper[] args)
    {
        return do_log(category, log_level.info, log_format_content, args);
    }
    public unsafe bool warning(demo_category_log_category_base category, string log_format_content, params param_wrapper[] args)
    {
        return do_log(category, log_level.warning, log_format_content, args);
    }
    public unsafe bool error(demo_category_log_category_base category, string log_format_content, params param_wrapper[] args)
    {
        return do_log(category, log_level.error, log_format_content, args);
    }
    public unsafe bool fatal(demo_category_log_category_base category, string log_format_content, params param_wrapper[] args)
    {
        return do_log(category, log_level.fatal, log_format_content, args);
    }
    #endregion




    /****************************************************************************************************************/
    /**                                 Code Generated By Categories                                              ***/
    /****************************************************************************************************************/
    private static readonly string[] category_names = {
                                                          ""
                                                          , "node_2"
                                                          , "node_2.node_5"
                                                          , "node_3"
                                                          , "node_3.node_6"
                                                          , "node_3.node_10"
                                                          , "node_4"
                                                          , "node_4.node_7"
                                                          , "node_4.node_7.node_8"
                                                          , "node_4.node_7.node_9"
                                                      };
    private static readonly uint categories_count = (uint)category_names.Length;
    public class demo_category_log_category_base : log_category_base
    {
    }
    public class demo_category_log_category_root
    {
        public class demo_category_log_node_2 : demo_category_log_category_base
        {
            public demo_category_log_node_2(){index = 1;}
            public class demo_category_log_node_5 : demo_category_log_category_base
            {
                public demo_category_log_node_5(){index = 2;}
            }
            public demo_category_log_node_5 node_5 = new demo_category_log_node_5(); 	//node_2.node_5
        }
        public demo_category_log_node_2 node_2 = new demo_category_log_node_2(); 	//node_2
        public class demo_category_log_node_3 : demo_category_log_category_base
        {
            public demo_category_log_node_3(){index = 3;}
            public class demo_category_log_node_6 : demo_category_log_category_base
            {
                public demo_category_log_node_6(){index = 4;}
            }
            public demo_category_log_node_6 node_6 = new demo_category_log_node_6(); 	//node_3.node_6	//comment Test
            public class demo_category_log_node_10 : demo_category_log_category_base
            {
                public demo_category_log_node_10(){index = 5;}
            }
            public demo_category_log_node_10 node_10 = new demo_category_log_node_10(); 	//node_3.node_10
        }
        public demo_category_log_node_3 node_3 = new demo_category_log_node_3(); 	//node_3
        public class demo_category_log_node_4 : demo_category_log_category_base
        {
            public demo_category_log_node_4(){index = 6;}
            public class demo_category_log_node_7 : demo_category_log_category_base
            {
                public demo_category_log_node_7(){index = 7;}
                public class demo_category_log_node_8 : demo_category_log_category_base
                {
                    public demo_category_log_node_8(){index = 8;}
                }
                public demo_category_log_node_8 node_8 = new demo_category_log_node_8(); 	//node_4.node_7.node_8
                public class demo_category_log_node_9 : demo_category_log_category_base
                {
                    public demo_category_log_node_9(){index = 9;}
                }
                public demo_category_log_node_9 node_9 = new demo_category_log_node_9(); 	//node_4.node_7.node_9
            }
            public demo_category_log_node_7 node_7 = new demo_category_log_node_7(); 	//node_4.node_7
        }
        public demo_category_log_node_4 node_4 = new demo_category_log_node_4(); 	//node_4
    }


    
    public readonly demo_category_log_category_root cat = new demo_category_log_category_root();
}