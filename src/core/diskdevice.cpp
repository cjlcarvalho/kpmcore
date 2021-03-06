/*************************************************************************
 *  Copyright (C) 2008 by Volker Lanz <vl@fidra.de>                      *
 *  Copyright (C) 2016 by Andrius Štikonas <andrius@stikonas.eu>         *
 *                                                                       *
 *  This program is free software; you can redistribute it and/or        *
 *  modify it under the terms of the GNU General Public License as       *
 *  published by the Free Software Foundation; either version 3 of       *
 *  the License, or (at your option) any later version.                  *
 *                                                                       *
 *  This program is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *  GNU General Public License for more details.                         *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 *************************************************************************/

#include "core/diskdevice.h"

#include "core/partitiontable.h"
#include "core/smartstatus.h"

#include <KLocalizedString>

#include <QFile>
#include <QByteArray>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __gnu_linux__
  #include <linux/fs.h>
#endif

#if !defined(BLKPBSZGET)
#define BLKPBSZGET _IO(0x12,123)/* get block physical sector size */
#endif

static qint64 getPhysicalSectorSize(const QString& device_node)
{
    /*
     * possible ways of getting the physical sector size for a drive:
     * - ioctl(BLKPBSZGET) -- supported with Linux 2.6.32 and later
     * - /sys/block/sda/queue/physical_block_size
     * - libblkid from util-linux-ng 2.17 or later
     * TODO: implement the blkid method
     */

#if defined(BLKPBSZGET)
    int phSectorSize = -1;
    int fd = open(device_node.toLocal8Bit().constData(), O_RDONLY);
    if (fd != -1) {
        if (ioctl(fd, BLKPBSZGET, &phSectorSize) >= 0) {
            close(fd);
            return phSectorSize;
        }

        close(fd);
    }
#endif

    QFile f(QStringLiteral("/sys/block/%1/queue/physical_block_size").arg(QString(device_node).remove(QStringLiteral("/dev/"))));

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray a = f.readLine();
        return a.trimmed().toInt();
    }

    return -1;
}

/** Constructs a Disk Device with an empty PartitionTable.
    @param name the Device's name, usually some string defined by the manufacturer
    @param deviceNode the Device's node, for example "/dev/sda"
    @param heads the number of heads in CHS notation
    @param numSectors the number of sectors in CHS notation
    @param cylinders the number of cylinders in CHS notation
    @param sectorSize the size of a sector in bytes
*/
DiskDevice::DiskDevice(const QString& name,
                       const QString& deviceNode,
                       qint32 heads,
                       qint32 numSectors,
                       qint32 cylinders,
                       qint64 sectorSize,
                       const QString& iconName)
    : Device(name, deviceNode, sectorSize, (static_cast<qint64>(heads) * cylinders * numSectors), iconName, Device::Disk_Device)
    , m_Heads(heads)
    , m_SectorsPerTrack(numSectors)
    , m_Cylinders(cylinders)
    , m_LogicalSectorSize(sectorSize)
    , m_PhysicalSectorSize(getPhysicalSectorSize(deviceNode))
{
}
