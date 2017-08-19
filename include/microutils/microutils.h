/*
 * libmicroutils - Micro cross-platform C library for handling strings, files,
 * regex, hashs, among others.
 *
 * Copyright (c) 2017 Silvio Clecio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * \file microutils.h
 * \brief Public interface to _libmicroutils_.
 */

#ifndef MICROUTILS_H
#define MICROUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*TODO: shared and static support. */
#ifndef MU_EXTERN
# define MU_EXTERN extern
#endif

#ifdef __GNUC__

#ifndef __format
# define __format(args...) __attribute__((__format__(__printf__, args)))
#endif

#ifndef __nonnull
# define __nonnull(params)
#endif

#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * \brief Retrieve the size of an array.
 * \param array Specified array.
 */
#ifndef MU_ARRAY_SIZE
# define MU_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

/**
 * \brief Hexadecimal representation.
 */
#define MU_HEX_CHARS "0123456789abcdef"

/**
 * \brief Hexadecimal representation. (uppercase)
 */
#define MU_HEX_UCHARS "0123456789ABCDEF"

/**
 * \brief Default MIME type for unrecognized extensions.
 */
#define MU_DEF_MIME "application/octet-stream"

/**
 * \brief Buffer size for the deflate/inflate.
 */
#define MU_ZBUF_CHUNK (16384)

/**
 * \brief Returns the library version.
 * \return Library version.
 */
MU_EXTERN const char *mu_ver();

/* system */

/**
 * \brief Retrieves the number of last error.
 * \return Number of last error.
 */
MU_EXTERN int mu_errno();

/**
 * \brief Maps the **errno** value to error message string.
 * \param errnum Error number.
 * \return Error message.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_err(int errnum);

/**
 * \brief Allocates memory passing its size and sets the allocated memory to zero.
 * \param size Allocated size.
 * \return Allocated memory.
 */
MU_EXTERN void *mu_alloc(size_t size);

/**
 * \brief Frees a previously allocated memory.
 * \param ptr Memory pointer.
 */
MU_EXTERN void mu_free(void *ptr) __nonnull((1));

/**
 * \brief Executes a program in a new process.
 * \param dir Directory of the process.
 * \param name Program name or script name.
 * \param args `NULL`-terminated array of arguments to be passed to the process.
 * \param waited **true** waits for the program to finish.
 * \param showed **true** shows the window of the program (Windows only).
 * \param term **true** opens the program in a new terminal.
 * \param timeout Max waiting time when \p wait is **true**.
 * \return **true** if the function succeeds, **false** otherwise.
 */
MU_EXTERN bool mu_exec2(const char *dir, const char *name, const char **args, bool waited, bool showed, bool term,
                        int timeout) __nonnull((2));

/**
 * \brief Executes a program in a new process.
 * \param name Program name or script name.
 * \return **true** if the function succeeds, **false** otherwise.
 */
MU_EXTERN bool mu_exec(const char *name) __nonnull((1));

/* strings */

/**
 * \brief Replace all occurrences of the search string with the replacement string.
 * \param str Input string.
 * \param substr Searched string.
 * \param rep Replacement string.
 * \return String with the replaced strings.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_replace(const char *str, const char *substr, const char *rep) __nonnull((1, 2, 3));

/**
 * \brief Replace all occurrences of the multiple search strings with the replacement string.
 * \param str Input string.
 * \param rep Replacement string.
 * \param ... Multiple searched strings, \c NULL determines the end of array.
 * \return String with the replaced strings.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_mreplace(const char *str, const char *rep, ...) __nonnull((1, 2));

/**
 * \brief #mu_split()'s callback.
 * \param cls Callback closure.
 * \param step Step string.
 * \param token Extracted token.
 * \return **true** searchs remaining occurrences.
 */
typedef bool (*mu_split_cb)(void *cls, const char *step, const char *token);

/**
 * \brief Split a string into an array of substrings.
 * \param str Source string.
 * \param delim Specifies the string to use for splitting the string.
 * \param cb Occurrence callback. **true** searchs remaining occurrences.
 * \param cls Callback closure.
 * \return **true** if at least one occurrence.
 */
MU_EXTERN bool mu_split(const char *str, const char *delim, mu_split_cb cb, void *cls) __nonnull((1, 2));

/**
 * \brief Removes leading and trailing blanks from a string.
 * \param str Input string.
 * \return Resulting string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_trim(const char *str) __nonnull((1));

/**
 * \brief Removes leading blanks from a string.
 * \param str Input string.
 * \return Resulting string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_ltrim(const char *str) __nonnull((1));

/**
 * \brief Removes trailing blanks from a string.
 * \param str Input string.
 * \return Resulting string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_rtrim(const char *str) __nonnull((1));

/**
 * \brief Extracts only numbers from a given string.
 * \param str Input string.
 * \return Extracted numbers.
 * \warning Caller **must** free the returned string.
 * \par Example
 * \code
 *  char *nums;
 *  nums = mu_nums("abc123def");
 *  printf("%s", nums); // returns 123
 *  free(nums);
 * \endcode
 */
MU_EXTERN char *mu_nums(const char *str) __nonnull((1));

/**
 * \brief Extracts a string X between two strings: Y and Z.
 * \param str Input content.
 * \param p1 Y string.
 * \param p2 Z string.
 * \return X string.
 * \note This function is case-sensitive. Consider #mu_ibetween as case-insensitve alternative.
 * \warning Caller **must** free the returned string.
 * \par Example
 * \code
 *  char *s;
 *  s = mu_between("<div>abc123</div>", "<div>", "</div>");
 *  printf("%s", s); // returns abc123
 *  free(s);
 * \endcode
 */
MU_EXTERN char *mu_between(const char *str, const char *p1, const char *p2) __nonnull((1, 2, 3));

/**
 * \brief Extracts a string X between two strings: Y and Z.
 * \param str Input content.
 * \param p1 Y string.
 * \param p2 Z string.
 * \return X string.
 * \note This function is case-insensitive. Consider #mu_between as case-sensitve alternative.
 * \warning Caller **must** free the returned string.
 * \par Example
 * \code
 *  char *s;
 *  s = mu_ibetween("<DIV>abc123</div>", "<div>", "</DIV>");
 *  printf("%s", s); // returns abc123
 *  free(s);
 * \endcode
 */
MU_EXTERN char *mu_ibetween(const char *str, const char *p1, const char *p2) __nonnull((1, 2, 3));

/**
 * \brief Formats a string using same _printf()_'s formatting.
 * \param fmt Formatting string.
 * \param ... Formatting arguments.
 * \return Formatted string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_fmt(const char *fmt, ...) __nonnull((1)) __format(1, 2);

/**
 * \brief Checks if a string is null or empty.
 * \param str Testing string.
 * \return **true** if the **str** is empty or null.
 */
MU_EXTERN bool mu_is_empty(const char *str);

/**
 * \brief Converts a **uint8_t** value to a hexadecimal string.
 * \param val **uint8_t** value.
 * \param hex Hexadecimal string.
 */
MU_EXTERN void mu_uitoh(uint8_t val, char *hex) __nonnull((2));

/**
 * \brief Converts a **uint8_t** value to a hexadecimal string. (uppercase)
 * \param val **uint8_t** value.
 * \param hex Hexadecimal string.
 */
MU_EXTERN void mu_uitouh(uint8_t val, char *hex) __nonnull((2));

/**
 * \brief Converts a string to a hexadecimal string.
 * \param str Conversion string.
 * \return Hexadecimal string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_stoh(const char *str) __nonnull((1));

/**
 * \brief Converts a string to a hexadecimal string. (uppercase)
 * \param str Conversion string.
 * \return Hexadecimal string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_stouh(const char *str) __nonnull((1));

/**
 * \brief Returns the unit of measure of a size.
 * \param size Size in bytes.
 * \return Unit of measure.
 */
MU_EXTERN const char *mu_size_unit(uint64_t size);

/**
 * \brief Returns current date time according RFC 2822 .
 * \return Resulting string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_now_rfc2822();

/* file */

/**
 * \brief Removes a file or a directory.
 * \param path Path name.
 * \return **true** if file was successfully deleted.
 */
MU_EXTERN bool mu_remove(const char *path) __nonnull((1));

/**
 * \brief Renames a file or a directory.
 * \param old_path Old path name.
 * \param new_path New path name.
 * \return **true** if file was successfully renamed.
 */
MU_EXTERN bool mu_rename(const char *old_path, const char *new_path) __nonnull((1, 2));

/**
 * \brief Checks if a file or a directory exists.
 * \param path Path name.
 * \return **true** if **path** exists.
 */
MU_EXTERN bool mu_exists(const char *path) __nonnull((1));

/**
 * \brief Changes the extension value of a file fileName.
 * \param filename Filename string.
 * \param ext Extension string.
 * \return Filename with the new extension value.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_change_ext(const char *filename, const char *ext) __nonnull((1, 2));

/**
 * \brief Extracts extension from a filename.
 * \param filename Filename string.
 * \return Extracted extension.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_ext(const char *filename) __nonnull((1));

/**
 * \brief Extracts filename from a full filename.
 * \param filename Filename string.
 * \return Extracted filename.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_filename(const char *filename) __nonnull((1));

/**
 * \brief Extracts file path from a full filename.
 * \param filename Filename string.
 * \return Extracted path.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_path(const char *filename) __nonnull((1));

/**
 * \brief Loads an entire file into a string.
 * \param filename Input filename.
 * \return Resulting string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_ftos(const char *filename) __nonnull((1));

/**
 * \brief Writes or appends a string to a file.
 * \param str String to be added or written.
 * \param filename File name.
 * \param append **true** appends **false** writes.
 * \return **true** if added/written, **false** otherwise.
 */
MU_EXTERN bool mu_stofa(const char *str, const char *filename, bool append);

/**
 * \brief Writes a string to a file.
 * \param str String to be written.
 * \param filename File name.
 * \return **true** if written, **false** otherwise.
 */
MU_EXTERN bool mu_stof(const char *str, const char *filename);

/**
 * \brief Returns the MIME type from an extension.
 * \param ext File extension.
 * \return MIME type.
 */
MU_EXTERN const char *mu_etomime(const char *ext) __nonnull((1));

/**
 * \brief Returns the MIME type from a filename.
 * \param filename Input filename.
 * \return MIME type.
 */
MU_EXTERN const char *mu_fntomime(const char *filename) __nonnull((1));

/* regex */

/**
 * \brief #mu_mmatch()'s callback.
 * \param cls Callback closure.
 * \param str Testing string.
 * \param start Start position.
 * \param end End position.
 * \return **true** searchs remaining occurrences.
 */
typedef bool (*mu_mmatch_cb)(void *cls, const char *str, size_t start, size_t end);

/**
 * \brief Checks if a string matches a given regular expression (regex) triggering a callback on each occurrence.
 * \param pattern Regular expression.
 * \param str Testing string.
 * \param ncase Case-insensitive if **true**.
 * \param cb Occurrence callback. **true** searchs remaining occurrences.
 * \param cls Callback closure.
 * \return **true** if at least one occurrence.
 * \par Example:
 * \code
 * #include <stdio.h>
 * #include <stdbool.h>
 * #include <memory.h>
 * #include <stdlib.h>
 * #include <microutils.h>
 *
 * static bool mu_extract_cb(void *cls, const char *str, size_t start, size_t end) {
 *     char *part;
 *     if ((end - start) > 0) {
 *         part = malloc((size_t) (start + end + 1));
 *         if (part) {
 *             memcpy(part, str + start, end - start);
 *             part[end - start] = 0;
 *             fprintf(stdout, "%s\n", part);
 *             free(part);
 *         }
 *     }
 *     return true;
 * }
 *
 * int main() {
 *     const char *str = "123abc456def789ghi";
 *     mu_mmatch("([a-z]*)", str, true, &mm_cb, NULL);
 *     return 0;
 * }
 *
 * // Returns:
 * abc
 * def
 * ghi
 * \endcode
 */
MU_EXTERN bool mu_mmatch(const char *pattern, const char *str, bool ncase, mu_mmatch_cb cb,
                         void *cls) __nonnull((1, 2, 4));

/**
 * \brief Checks if a string matches a given regular expression (regex).
 * \param pattern Regular expression.
 * \param str Testing string.
 * \return **true** if **str** matches the **pattern**.
 */
MU_EXTERN bool mu_match(const char *pattern, const char *str) __nonnull((1, 2));

/**
 * \brief Checks if a string matches a given regular expression (regex).
 * \param pattern Regular expression.
 * \param str Testing string.
 * \param ncase Case-insensitive if **true**.
 * \return **true** if **str** matches the **pattern**.
 */
MU_EXTERN bool mu_match2(const char *pattern, const char *str, bool ncase) __nonnull((1, 2));

/**
 * \brief Checks if a string matches a given regular expression (regex). (case-insensitive)
 * \param pattern Regular expression.
 * \param str Testing string.
 * \return **true** if **str** matches the **pattern**.
 */
MU_EXTERN bool mu_imatch(const char *pattern, const char *str) __nonnull((1, 2));

/**
 * \brief Extracts a string matched in a given regular expression (regex).
 * \param pattern Regular expression.
 * \param str Input string.
 * \param ncase Case-insensitive if **true**.
 * \param recur **true** for recursive matching.
 * \return Extracted string.
 * \warning Caller **must** free the returned string.
 */
MU_EXTERN char *mu_mextract(const char *pattern, const char *str, bool ncase, bool recur) __nonnull((1, 2));

/* deflate/inflate */

/**
 * \brief Write/read stream callback.
 * \param cls Closure.
 * \param buf Buffer.
 * \param len Buffer length.
 */
typedef size_t (*mu_zstream_data_cb)(void *cls, void *buf, size_t len);

/**
 * \brief EOF stream callback.
 * \param cls Closure.
 */
typedef bool (*mu_zstream_eof_cb)(void *cls);

/**
 * \brief Compresses from stream source to stream dest until EOF on source.
 * \param read_cb Stream source read callback.
 * \param read_cls Stream source read closure.
 * \param write_cb Stream dest write callback.
 * \param write_cls Stream dest write closure.
 * \param eof_cb EOF stream source callback.
 * \param eof_cls EOF stream source closure.
 * \param level Compression level: 0 (uncompressed) to 9 (max compression).
 * \return _Z_OK_ on success, _Z_MEM_ERROR_ if memory could not be allocated for processing, _Z_STREAM_ERROR_ if an
 * invalid compression level is supplied, _Z_VERSION_ERROR_ if the version of _zlib.h_ and the version of the library
 * linked do not match, or _Z_ERRNO_ if there is an error reading or writing the streams.
 */
MU_EXTERN int mu_zdeflate(mu_zstream_data_cb read_cb, void *read_cls, mu_zstream_data_cb write_cb, void *write_cls,
                          mu_zstream_eof_cb eof_cb, void *eof_cls, int level) __nonnull((1, 2, 3, 4, 5, 6));

/**
 * \brief Compresses from file source to file dest.
 * \param filename_in Filename source.
 * \param filename_out Filename dest.
 * \param level Compression level: 0 (uncompressed) to 9 (max compression).
 * \return _Z_OK_ on success, _Z_MEM_ERROR_ if memory could not be allocated for processing, _Z_STREAM_ERROR_ if an
 * invalid compression level is supplied, _Z_VERSION_ERROR_ if the version of _zlib.h_ and the version of the library
 * linked do not match, or _Z_ERRNO_ if there is an error reading or writing the files.
 */
MU_EXTERN int mu_zfdeflate(const char *filename_in, const char *filename_out, int level) __nonnull((1, 2));

/**
 * \brief Decompresses from stream source to stream dest until stream ends or EOF.
 * \param read_cb Stream source read callback.
 * \param read_cls Stream source read closure.
 * \param write_cb Stream dest write callback.
 * \param write_cls Stream dest write closure.
 * \return _Z_OK_ on success, _Z_MEM_ERROR_ if memory could not be allocated for processing, _Z_DATA_ERROR_ if the
 * deflate data is invalid or incomplete, _Z_VERSION_ERROR_ if the version of _zlib.h_ and the version of the library
 * linked do not match, or _Z_ERRNO_ if there is an error reading or writing the streams.
 */
MU_EXTERN int mu_zinflate(mu_zstream_data_cb read_cb, void *read_cls, mu_zstream_data_cb write_cb,
                          void *write_cls) __nonnull((1, 2, 3, 4));

/**
 * \brief Decompresses from file source to file dest.
 * \param filename_in Filename source.
 * \param filename_out Filename dest.
 * \return _Z_OK_ on success, _Z_MEM_ERROR_ if memory could not be allocated for processing, _Z_DATA_ERROR_ if the
 * deflate data is invalid or incomplete, _Z_VERSION_ERROR_ if the version of _zlib.h_ and the version of the library
 * linked do not match, or _Z_ERRNO_ if there is an error reading or writing the files.
 */
MU_EXTERN int mu_zfinflate(const char *filename_in, const char *filename_out) __nonnull((1, 2));

/**
 * \brief Converts a ZLib error code to a string message.
 * \param zerrnum Error number.
 * \return Error message.
 */
MU_EXTERN const char *mu_zerr(int zerrnum);

/* hash */

/**
 * \brief Base64 encode.
 * \param src Data to be encoded.
 * \param len Length of the data to be encoded.
 * \param out_len Pointer to output length variable or \c NULL if not used.
 * \return Allocated buffer of \p out_len bytes of encoded data or \c NULL on failure.
 * \warning Caller is responsible for freeing the returned buffer.
 * \note Returned buffer is _0-null_ terminated to make it easier to use as a C string. The null terminator is not
 * included in \p out_len.
 */
MU_EXTERN unsigned char *mu_b64enc(const unsigned char *src, size_t len, size_t *out_len) __nonnull((1, 3));

/**
 * \brief Base64 decode.
 * \param src Data to be decoded.
 * \param len Length of the data to be decoded.
 * \param out_len Pointer to output length variable.
 * \return Allocated buffer of \p out_len bytes of decoded data or \c NULL on failure.
 * \warning Caller is responsible for freeing the returned buffer.
 */
MU_EXTERN unsigned char *mu_b64dec(const unsigned char *src, size_t len, size_t *out_len) __nonnull((1, 3));

#ifdef __cplusplus
}
#endif

#endif