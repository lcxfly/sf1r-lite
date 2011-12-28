/**
 * @file LogServerCfg.h
 * @author Zhongxia Li
 * @date Dec 20, 2011
 * @brief Log server configuration
 */
#ifndef LOG_SERVER_CFG_H_
#define LOG_SERVER_CFG_H_

#include <string>
#include <set>

#include <util/singleton.h>

namespace sf1r
{

class LogServerCfg
{
public:
    LogServerCfg();

    ~LogServerCfg();

    static LogServerCfg* get()
    {
        return izenelib::util::Singleton<LogServerCfg>::get();
    }

    bool parse(const std::string& cfgFile);

    inline const std::string& getLocalHost() const
    {
        return host_;
    }

    inline unsigned int getRpcServerPort() const
    {
        return rpcPort_;
    }

    inline unsigned int getDriverServerPort() const
    {
        return driverPort_;
    }

    inline unsigned int getThreadNum() const
    {
        return threadNum_;
    }

    const std::set<std::string>& getDriverCollections() const
    {
        return driverCollections_;
    }

    inline const std::string& getDrumName() const
    {
        return drum_name_;
    }

    inline unsigned int getDrumNumBuckets() const
    {
        return drum_num_buckets_;
    }

    inline unsigned int getDrumBucketBuffElemSize() const
    {
        return drum_bucket_buff_elem_size_;
    }

    inline unsigned int getDrumBucketByteSize() const
    {
        return drum_bucket_byte_size_;
    }

private:
    bool parseCfgFile_(const std::string& cfgFile);

    void parseDriverCollections(const std::string& collections);

private:
    std::string cfgFile_;

    std::string host_;
    unsigned int rpcPort_;
    unsigned int driverPort_;
    unsigned int threadNum_;

    std::set<std::string> driverCollections_;

    std::string drum_name_;
    unsigned int drum_num_buckets_;
    unsigned int drum_bucket_buff_elem_size_;
    unsigned int drum_bucket_byte_size_;
};

}

#endif /* LOGSERVERCFG_H_ */
