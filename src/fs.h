#ifndef SC_FS_H
#define SC_FS_H

/**
 * @file fs.h
 * @author SWPP TAs (swpp@sf.snu.ac.kr)
 * @brief Module for reading & writing files
 * @version 2023.1.3
 * @date 2023-06-07
 * @copyright Copyright (c) 2022-2023 SWPP TAs
 */

#include "result.h"
#include "static_error.h"

#include <string>

namespace fs {
/**
 * @brief Exception thrown by filesystem module
 */
class FilesystemError : public Error<FilesystemError> {
private:
  std::string message;

public:
  /**
   * @brief Construct a new FilesystemError object
   *
   * @param message Error description
   * @param path File or directory that caused the exception
   */
  FilesystemError(const std::string_view message,
                  const std::string_view path) noexcept;

  /**
   * @brief Read the exception
   * @return Exception message in C-String format
   */
  const char *what() const noexcept { return message.c_str(); }
};

/**
 * @brief Read the file into string
 * 
 * @param filepath Relative path from the current working directory to the file
 * @return Result<std::string, FilesystemError> Content of the file if read
 * succeeds, otherwise an error that contains the reason of failure
 */
Result<std::string, FilesystemError>
readFile(const std::string_view filepath) noexcept;

/**
 * @brief Write the string into file
 * 
 * @param filepath Relative path from the current working directory to the file
 * @param content Content to write on the file
 * @return Result<size_t, FilesystemError> Number of bytes written if write
 * succeeds, otherwise an error that contains the reason of failure
 */
Result<size_t, FilesystemError>
writeFile(const std::string_view filepath,
          const std::string_view content) noexcept;
} // namespace fs
#endif // SC_FS_H
