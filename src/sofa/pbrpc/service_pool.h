// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_SERVICE_POOL_H_
#define _SOFA_PBRPC_SERVICE_POOL_H_

#include <map>
#include <set>
#include <list>

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/builtin_service.pb.h>
#include <sofa/pbrpc/counter.h>
#include <sofa/pbrpc/murmurhash.h>
#include <sofa/pbrpc/ptime.h>

namespace sofa {
namespace pbrpc {

#define SERVICE_CACHE_SLOT_COUNT 1019
#define STAT_SLOT_COUNT 602
#define STAT_SLOT_SECONDS 1

// All the time is in micro-seconds.
struct StatSlot
{
    int64 succeed_count;
    int64 succeed_process_time;
    int64 succeed_max_process_time;
    int64 failed_count;
    int64 failed_process_time;
    int64 failed_max_process_time;
    int64 slot_id;
};

class RpcServerImpl;
class ServiceBoard;

class MethodBoard
{
public:
    MethodBoard() : _service_board(NULL), _desc(NULL), _slot_index(0), _slot_id(0)
    {
        memset(_stat_slots, 0, sizeof(_stat_slots));
    }

    ~MethodBoard() {}

    void SetServiceBoard(ServiceBoard* service_board)
    {
        _service_board = service_board;
    }

    ServiceBoard* GetServiceBoard()
    {
        return _service_board;
    }

    void SetDescriptor(const google::protobuf::MethodDescriptor* desc)
    {
        _desc = desc;
    }

    const google::protobuf::MethodDescriptor* Descriptor()
    {
        return _desc;
    }

    void ReportProcessBegin()
    {
    }

    void ReportProcessEnd(bool succeed, int64 time_in_us)
    {
        volatile StatSlot* slot = &_stat_slots[_slot_index];
        if (succeed) {
            ++slot->succeed_count;
            slot->succeed_process_time += time_in_us;
            if (slot->succeed_max_process_time < time_in_us) {
                slot->succeed_max_process_time = time_in_us;
            }
        }
        else {
            ++slot->failed_count;
            slot->failed_process_time += time_in_us;
            if (slot->failed_max_process_time < time_in_us) {
                slot->failed_max_process_time = time_in_us;
            }
        }
    }

    void NextSlot()
    {
        int index = ((_slot_index + 1 == STAT_SLOT_COUNT) ?  0 : _slot_index + 1);
        memset(&_stat_slots[index], 0, sizeof(StatSlot));
        _slot_index = index;
        ++_slot_id;
    }

    void LatestStats(int slot_count, StatSlot* stat_out)
    {
        SCHECK(slot_count > 0 && slot_count <= STAT_SLOT_COUNT);
        SCHECK(stat_out != NULL);
        memset(stat_out, 0, sizeof(StatSlot));
        int index = (_slot_index == 0 ? STAT_SLOT_COUNT - 1 : _slot_index - 1);
        int64 slot_id = _slot_id - 1;
        while (slot_count > 0 && slot_id >= 0) {
            volatile StatSlot* slot = &_stat_slots[index];
            stat_out->succeed_count += slot->succeed_count;
            stat_out->succeed_process_time += slot->succeed_process_time;
            if (stat_out->succeed_max_process_time < slot->succeed_max_process_time) {
                stat_out->succeed_max_process_time = slot->succeed_max_process_time;
            }
            stat_out->failed_count += slot->failed_count;
            stat_out->failed_process_time += slot->failed_process_time;
            if (stat_out->failed_max_process_time < slot->failed_max_process_time) {
                stat_out->failed_max_process_time = slot->failed_max_process_time;
            }
            stat_out->slot_id = slot_id;
            --slot_count;
            index = (index == 0 ? STAT_SLOT_COUNT - 1 : index - 1);
            --slot_id;
        }
    }

    void LatestStats(int slot_count, sofa::pbrpc::builtin::MethodStat* stat_out)
    {
        SCHECK(stat_out != NULL);
        stat_out->Clear();
        StatSlot stat;
        LatestStats(slot_count, &stat);
        stat_out->set_method_name(_desc->full_name());
        stat_out->set_succeed_count(stat.succeed_count);
        stat_out->set_succeed_avg_time_us(stat.succeed_count > 0 ?
                (float)stat.succeed_process_time / stat.succeed_count : 0);
        stat_out->set_succeed_max_time_us(stat.succeed_max_process_time);
        stat_out->set_failed_count(stat.failed_count);
        stat_out->set_failed_avg_time_us(stat.failed_count > 0 ?
                (float)stat.failed_process_time / stat.failed_count : 0);
        stat_out->set_failed_max_time_us(stat.failed_max_process_time);
        stat_out->set_slot_id(stat.slot_id);
    }

private:
    ServiceBoard* _service_board;
    const google::protobuf::MethodDescriptor* _desc;
    StatSlot _stat_slots[STAT_SLOT_COUNT];
    volatile int _slot_index;
    volatile int64 _slot_id;
};

class ServiceBoard
{
public:
    ServiceBoard(ServiceBoard* next, google::protobuf::Service* svc, bool own)
        : _next(next), _svc(svc), _own(own)
    {
        _svc_desc = _svc->GetDescriptor();
        _method_count = _svc_desc->method_count();
        _method_boards = new MethodBoard[_method_count];
        for (int i = 0; i < _method_count; ++i) {
            _method_boards[i].SetServiceBoard(this);
            _method_boards[i].SetDescriptor(_svc_desc->method(i));
        }
    }

    ~ServiceBoard()
    {
        delete[] _method_boards;
        if (_own) {
            delete _svc;
        }
    }

    ServiceBoard* Next()
    {
        return _next;
    }

    const std::string& ServiceName()
    {
        return _svc_desc->full_name();
    }

    const google::protobuf::ServiceDescriptor* Descriptor()
    {
        return _svc_desc;
    }

    google::protobuf::Service* Service()
    {
        return _svc;
    }

    MethodBoard* Method(int method_id)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        return &_method_boards[method_id];
    }

    void ReportProcessBegin(int method_id)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].ReportProcessBegin();
    }

    void ReportProcessEnd(int method_id, bool succeed, int64 time_in_us)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].ReportProcessEnd(succeed, time_in_us);
    }

    void NextSlot()
    {
        for (int i = 0; i < _method_count; ++i) {
            _method_boards[i].NextSlot();
        }
    }

    void LatestStats(int method_id, int slot_count, StatSlot* stat_out)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].LatestStats(slot_count, stat_out);
    }

    void LatestStats(int method_id, int slot_count, sofa::pbrpc::builtin::MethodStat* stat_out)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].LatestStats(slot_count, stat_out);
    }

    void LatestStats(int64 expect_period, sofa::pbrpc::builtin::ServiceStat* stat_out)
    {
        SCHECK(stat_out != NULL);
        stat_out->Clear();
        int64 real_period;
        int slot_count;
        GetRealPeriod(expect_period, &real_period, &slot_count);
        stat_out->set_service_name(ServiceName());
        stat_out->set_period_seconds(real_period);
        stat_out->set_succeed_count(0);
        stat_out->set_failed_count(0);
        for (int i = 0; i < _method_count; ++i) {
            sofa::pbrpc::builtin::MethodStat* method_stat = stat_out->add_method_stats();
            LatestStats(i, slot_count, method_stat);
            stat_out->set_succeed_count(
                    stat_out->succeed_count() + method_stat->succeed_count());
            stat_out->set_failed_count(
                    stat_out->failed_count() + method_stat->failed_count());
        }
    }

private:
    void GetRealPeriod(int64 expect_period, int64* real_period, int* slot_count)
    {
        int sc = 1;
        if (expect_period > 1) {
            sc = (expect_period - 1) / STAT_SLOT_SECONDS + 1;
        }
        sc = std::min(sc, STAT_SLOT_COUNT - 2);
        *real_period = sc * STAT_SLOT_SECONDS;
        *slot_count = sc;
    }

private:
    ServiceBoard* _next;
    google::protobuf::Service* _svc;
    bool _own;
    const google::protobuf::ServiceDescriptor* _svc_desc;
    int _method_count;
    MethodBoard* _method_boards;
};

class ServicePool
{
public:
    ServicePool(RpcServerImpl* rpc_server) : _rpc_server(rpc_server), _head(NULL), _count(0)
    {
        memset(_cache, 0, sizeof(_cache));
    }

    virtual ~ServicePool()
    {
        for (ServiceMap::iterator it = _service_map.begin();
                it != _service_map.end(); ++it) {
            delete it->second;
        }
    }

    RpcServerImpl* RpcServer()
    {
        return _rpc_server;
    }

    bool RegisterService(google::protobuf::Service* service, bool take_ownership = true)
    {
        SCHECK(service != NULL);
        const std::string& svc_name = service->GetDescriptor()->full_name();
        ScopedLocker<FastLock> _(_service_map_lock);
        // check exist
        if (_service_map.find(svc_name) != _service_map.end()) {
            return false;
        }
        // register service
        ServiceBoard* svc_board = new ServiceBoard(_head, service, take_ownership);
        _service_map[svc_name] = svc_board;
        _head = svc_board;
        ++_count;
        // add into cache
        uint64 cache_index = CacheIndex(svc_name);
        if (_cache[cache_index] == NULL) {
            _cache[cache_index] = svc_board;
        }
        return true;
    }

    ServiceBoard* FindService(const std::string& svc_name)
    {
        // find in cache first
        uint64 cache_index = CacheIndex(svc_name);
        if (_cache[cache_index] != NULL
                && _cache[cache_index]->ServiceName() == svc_name) {
            return _cache[cache_index];
        }
        // find in map then
        ScopedLocker<FastLock> _(_service_map_lock);
        ServiceMap::iterator it = _service_map.find(svc_name);
        return it == _service_map.end() ? NULL : it->second;
    }

    // List registered services in order by name.
    void ListService(std::list<ServiceBoard*>* svc_list)
    {
        SCHECK(svc_list != NULL);
        svc_list->clear();
        // get services from link, and make them in order
        ServiceBoard* sp = _head;
        ServiceMap tmp_map;
        while (sp != NULL) {
            tmp_map[sp->ServiceName()] = sp;
            sp = sp->Next();
        }
        // copy to list
        for (ServiceMap::iterator it = tmp_map.begin();
                it != tmp_map.end(); ++it) {
            svc_list->push_back(it->second);
        }
    }

    // List registered services in order by name.
    void ListService(std::list<google::protobuf::Service*>* svc_list)
    {
        SCHECK(svc_list != NULL);
        svc_list->clear();
        std::list<ServiceBoard*> board_list;
        ListService(&board_list);
        for (std::list<ServiceBoard*>::iterator it = board_list.begin();
                it != board_list.end(); ++it) {
            svc_list->push_back((*it)->Service());
        }
    }

    void ListService(sofa::pbrpc::builtin::ListServiceResponse* response)
    {
        SCHECK(response != NULL);
        response->Clear();
        // get services
        std::list<google::protobuf::Service*> svc_list;
        ListService(&svc_list);
        // add to response
        std::set<std::string> added_file_set;
        for (std::list<google::protobuf::Service*>::iterator it = svc_list.begin();
                it != svc_list.end(); ++it)
        {
            const google::protobuf::ServiceDescriptor* sd = (*it)->GetDescriptor();
            AddFileDescriptor(response, &added_file_set, sd->file());
            *(response->add_services()) = sd->full_name();
        }
    }

    int ServiceCount()
    {
        return _count;
    }

    void NextSlot()
    {
        ServiceBoard* sp = _head;
        while (sp != NULL) {
            sp->NextSlot();
            sp = sp->Next();
        }
    }

private:
    uint64 CacheIndex(const std::string& name)
    {
        return murmurhash(name.c_str(), name.size()) % SERVICE_CACHE_SLOT_COUNT;
    }

    void AddFileDescriptor(sofa::pbrpc::builtin::ListServiceResponse* response,
            std::set<std::string>* added_file_set, const google::protobuf::FileDescriptor* fd)
    {
        for (int i = 0; i < fd->dependency_count(); ++i) {
            const google::protobuf::FileDescriptor* dfd = fd->dependency(i);
            AddFileDescriptor(response, added_file_set, dfd);
        }
        if (added_file_set->find(fd->name()) == added_file_set->end()) {
            google::protobuf::FileDescriptorProto* fdp = response->add_files();
            fd->CopyTo(fdp);
            added_file_set->insert(fd->name());
        }
    }

private:
    RpcServerImpl* _rpc_server;

    typedef std::map<std::string, ServiceBoard*> ServiceMap;
    ServiceMap _service_map;
    FastLock _service_map_lock;

    ServiceBoard* _head;
    int _count;
    ServiceBoard* _cache[SERVICE_CACHE_SLOT_COUNT];

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(ServicePool);
}; // class ServicePool

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_SERVICE_POOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
