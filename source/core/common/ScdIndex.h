/*
 * File:   ScdIndex.h
 * Author: Paolo D'Apice
 *
 * Created on September 11, 2012, 12:02 PM
 */

#ifndef SCDINDEX_H
#define SCDINDEX_H

#include "ScdIndexTag.h"
#include "ScdIndexLeveldb.h"
#include "ScdIndexDocument.h"
#include "ScdParser.h"

namespace scd {

/**
 * @brief SCD multi-index container
 * This class wraps a container with two indeces allowing to retrieve
 * document offsets within an SCD file:
 * - unique index on the specified Docid tag (default to 'DOCID')
 * - non-unique index on the scpecified Property tag (default to 'uuid')
 */
template <typename Property = uuid, typename Docid = DOCID>
class ScdIndex {
    typedef ScdIndexLeveldb<Docid, Property> ContainerType; //< The actual container type.

    ContainerType container;

public:
    typedef Document<Docid, Property> DocumentType; //< The actual document type.

    typedef typename Docid::type DocidType;          //< The actual Docid type.
    typedef typename Property::type PropertyType;    //< The actual Property type

    typedef typename ContainerType::DocidIterator DocidIterator;       //< Iterator on Docid values.
    typedef typename ContainerType::PropertyIterator PropertyIterator; //< Iterator on Property values.

    /// Destructor.
    ~ScdIndex() {}

    /**
     * Build an index on an SCD file.
     * @param scdpath The full path to the SCD file.
     * @param path1 The full path to a directory into which save the docid database.
     * @param path2 The full path to a directory into which save the property database.
     * @param flush_count Flush to storage every \c flush_count documents.
     * @return A pointer to an instance of ScdIndex.
     */
    static ScdIndex<Property, Docid>* build(const std::string& scdpath,
            const std::string& path1,
            const std::string& path2,
            const unsigned flush_count = 1e5);

    /**
     * Load an existing index with the given leveldb databases.
     * @param path1 Path to the Docid leveldb database.
     * @param path2 Path to the Property leveldb database.
     */
    static ScdIndex<Property, Docid>* load(const std::string& path1, const std::string& path2) {
        return new ScdIndex(path1, path2, false);
    }

    /// @return The number of stored documents.
    size_t size() const {
        return container.size();
    }

    /// Get the offset of the document having the given Docid.
    bool getOffset(const DocidType& key, offset_type& offset) const {
        return container.getByDocid(key, offset);
    }

    /// Get the offset of the documents having the given Property.
    bool getOffsetList(const PropertyType& key, std::vector<offset_type>& offsets) const {
        return container.getByProperty(key, offsets);
    }

    /// @return begin iterator on Docid.
    DocidIterator begin() const {
        return container.begin();
    }

    /// @return end iterator on Docid.
    DocidIterator end() const {
        return container.end();
    }

    /// @return begin iterator on Property.
    PropertyIterator pbegin() const {
        return container.pbegin();
    }

    /// @return end iterator on Property.
    PropertyIterator pend() const {
        return container.pend();
    }

private:
    /// Constructor.
    ScdIndex(const std::string& path1, const std::string& path2, const bool create)
            : container(path1, path2, create) {}
};

template<typename Property, typename Docid>
ScdIndex<Property, Docid>*
ScdIndex<Property, Docid>::build(const std::string& path,
        const std::string& path1,
        const std::string& path2,
        const unsigned flush_count) {
    static ScdParser parser;
    typedef ScdParser::iterator iterator;

    CHECK(parser.load(path)) << "Cannot load file: " << path;
    LOG(INFO) << "Building index on: " << path << " ...";

    ScdIndex<Property, Docid>* index = new ScdIndex(path1, path2, true);
    iterator end = parser.end();
    unsigned count = 1;
    for (iterator it = parser.begin(); it != end; ++it, ++count) {
        SCDDocPtr doc = *it;
        CHECK(doc) << "Document is null";

        DLOG(INFO) << "got document '" << doc->at(0).second << "' @ " << it.getOffset();
        index->container.insert(DocumentType(it.getOffset(), doc), true);

        if (count % flush_count == 0) {
            index->container.flush();
            LOG(INFO) << "Saved " << count << " documents ...";
        }
    }
    index->container.flush();
    LOG(INFO) << "Indexed " << (count - 1) << " documents.";
    return index;
}

} /* namespace scd */

#endif /* SCDINDEX_H */