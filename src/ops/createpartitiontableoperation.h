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

#if !defined(KPMCORE_CREATEPARTITIONTABLEOPERATION_H)

#define KPMCORE_CREATEPARTITIONTABLEOPERATION_H

#include "util/libpartitionmanagerexport.h"

#include "ops/operation.h"

#include "core/partitiontable.h"

#include <QString>

class Device;
class CreatePartitionTableJob;
class PartitionTable;
class OperationStack;

/** Create a PartitionTable.
    @author Volker Lanz <vl@fidra.de>
*/
class LIBKPMCORE_EXPORT CreatePartitionTableOperation : public Operation
{
    Q_DISABLE_COPY(CreatePartitionTableOperation)

    friend class OperationStack;

public:
    CreatePartitionTableOperation(Device& d, PartitionTable::TableType t);
    CreatePartitionTableOperation(Device& d, PartitionTable* ptable);
    ~CreatePartitionTableOperation();

public:
    QString iconName() const override {
        return QStringLiteral("edit-clear");
    }
    QString description() const override;
    void preview() override;
    void undo() override;
    bool execute(Report& parent) override;

    bool targets(const Device& d) const override;
    bool targets(const Partition&) const  override{
        return false;
    }

    static bool canCreate(const Device* device);

protected:
    Device& targetDevice() {
        return m_TargetDevice;
    }
    const Device& targetDevice() const {
        return m_TargetDevice;
    }

    PartitionTable* partitionTable() {
        return m_PartitionTable;
    }
    const PartitionTable* partitionTable() const {
        return m_PartitionTable;
    }

    PartitionTable* oldPartitionTable() {
        return m_OldPartitionTable;
    }
    void setOldPartitionTable(PartitionTable* old) {
        m_OldPartitionTable = old;
    }

    CreatePartitionTableJob* createPartitionTableJob() {
        return m_CreatePartitionTableJob;
    }

private:
    Device& m_TargetDevice;
    PartitionTable* m_OldPartitionTable;
    PartitionTable* m_PartitionTable;
    CreatePartitionTableJob* m_CreatePartitionTableJob;
};

#endif
