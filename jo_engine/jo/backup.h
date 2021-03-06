/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** @file backup.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine saving features function prototypes
 *  @todo List directory
 *  @bug No known bugs.
 */

#ifndef __JO_BACKUP_H__
# define __JO_BACKUP_H__

# ifdef JO_COMPILE_WITH_BACKUP_SUPPORT

/** @brief Backup device type
  * @warning If you change these values, the program will crash
 */
typedef enum
{
    JoInternalMemoryBackup = 0,
    JoCartridgeMemoryBackup = 1,
    JoExternalDeviceBackup = 2
}                   jo_backup_device;

/** @brief Backup status */
typedef enum
{
    JoBackupStatusOk = 0,
    JoBackupStatusWriteProtect = 1,
    JoBackupStatusOutOfMemory = 2,
    JoBackupStatusNotFound = 3,
    JoBackupStatusNoMatch = 4,
    JoBackupStatusBroken = 5,
    JoBackupStatusUnknown = 6
}                   jo_backup_status;

/** @brief Mount the backup device (must be the first call)
 *  @param backup_device Backup device
 *  @return true if succeed
 */
bool                jo_backup_mount(const jo_backup_device backup_device);

/** @brief Unmount the backup device (optional)
 *  @param backup_device Backup device
 *  @return true if succeed
 */
bool                jo_backup_unmount(const jo_backup_device backup_device);

/** @brief Check if file exists
 *  @param backup_device Backup device
 *  @param fname File name (max 11 characters not NULL)
 *  @return true if the file exists
 */
bool                jo_backup_file_exists(const jo_backup_device backup_device, const char * const fname);

/** @brief Format the device (all data is lost)
 *  @param backup_device Backup device
 *  @return true if succeed
 */
bool                jo_backup_format_device(const jo_backup_device backup_device);

/** @brief Save file contents to the backup device (if the file exists it will be overwritten)
 *  @param backup_device Backup device
 *  @param fname File name (max 11 characters not NULL)
 *  @param comment Comment (max 10 characters not NULL)
 *  @param contents Data (not NULL or empty)
 *  @param content_size Data size
 *  @return true if succeed
 */
bool                jo_backup_save_file_contents(const jo_backup_device backup_device, const char * const fname, const char * const comment, void *contents, unsigned short content_size);

/** @brief Delete file on the backup device
 *  @param backup_device Backup device
 *  @param fname File name (max 11 characters not NULL)
 *  @return true if succeed
 */
bool                jo_backup_delete_file(const jo_backup_device backup_device, const char * const fname);

/** @brief Get file last modified date on the backup device
 *  @param backup_device Backup device
 *  @param fname File name (max 11 characters not NULL)
 *  @param datetime Output date and time (not NULL)
 *  @return true if succeed
 */
bool                jo_backup_get_file_last_modified_date(const jo_backup_device backup_device, const char * const fname, jo_datetime *datetime);

/** @brief Load file contents from the backup device
 *  @param backup_device Backup device
 *  @param fname File name (max 11 characters not NULL)
 *  @param length File length (optional, pass JO_NULL to ignore)
 *  @return File contents
 */
void                *jo_backup_load_file_contents(const jo_backup_device backup_device, const char * const fname, unsigned int *length);

/** @brief Get free block count on the backup device
 *  @param backup_device Backup device
 *  @return Free block count
 */
int                 jo_backup_get_free_block_count(const jo_backup_device backup_device);

/** @brief Get last backup status (error details)
 *  @param backup_device Backup device
 *  @return Backup status
 */
jo_backup_status    jo_backup_get_last_status(const jo_backup_device backup_device);

#endif /* !JO_COMPILE_WITH_BACKUP_SUPPORT */

#endif /* !__JO_BACKUP_H__ */

/*
** END OF FILE
*/
