#include "RemotePurchaseManager.h"

namespace sf1r
{

RemotePurchaseManager::RemotePurchaseManager(
    const std::string& keyspace,
    const std::string& columnFamily,
    libcassandra::Cassandra* client
)
    : cassandra_(CassandraAdaptor::createColumnFamilyDef(keyspace, columnFamily,
                 "UTF8Type", "IntegerType"), client)
{
}

bool RemotePurchaseManager::getPurchaseItemSet(
    const std::string& userId,
    ItemIdSet& itemIdSet
)
{
    return cassandra_.getColumnNames(userId, itemIdSet);
}

bool RemotePurchaseManager::savePurchaseItem_(
    const std::string& userId,
    const ItemIdSet& totalItems,
    const std::list<itemid_t>& newItems
)
{
    for (std::list<itemid_t>::const_iterator it = newItems.begin();
        it != newItems.end(); ++it)
    {
        if (! cassandra_.insertColumn(userId, *it))
            return false;
    }

    return true;
}

} // namespace sf1r
