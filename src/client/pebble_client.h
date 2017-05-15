/*
 * Tencent is pleased to support the open source community by making Pebble available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 *
 */

#ifndef  _PEBBLE_CLIENT_PEBBLE_CLIENT_H_
#define  _PEBBLE_CLIENT_PEBBLE_CLIENT_H_

#include <vector>

#include "common/log.h"
#include "common/platform.h"
#include "framework/options.h"
#include "framework/pebble_rpc.h"
#include "framework/router.h"
#include "framework/when_all.h"


//////////////////////////////////////////////////////////////////////////////////////
namespace pebble {

// ǰ������
class CoroutineSchedule;
class IEventHandler;
class IProcessor;
class Naming;
class NamingFactory;
class RouterFactory;
class SessionMgr;
class Stat;
class StatManager;
class Timer;


//////////////////////////////////////////////////////////////////////////////////////

/// @brief ���ַ������Ͷ���
typedef enum {
    kNAMING_TBUSPP = 0,
    kNAMING_ZOOKEEPER = 1,
    kNAMING_BUTT
} NamingType;

/// @brief ·�������Ͷ���
typedef enum {
    kROUTER_TBUSPP = 0,
    kROUTER_BUTT
} RouterType;

/// @brief Processor���Ͷ���
typedef enum {
    kPEBBLE_RPC_BINARY = 0, // thrift binary�����pebble rpcʵ��
    kPEBBLE_RPC_JSON,       // thrift json�����pebble rpcʵ��
    kPEBBLE_RPC_PROTOBUF,   // protobuf�����pebble rpcʵ��
    kPROCESSOR_TYPE_BUTT
} ProcessorType;


//////////////////////////////////////////////////////////////////////////////////////


/// @brief PebbleApp��һ��ʵ�֣�����Ϊһ��ͨ�õ�Pebble server���򣬾ۺ���Pebble�ĸ�����ģ��
class PebbleClient {
public:
    PebbleClient();
    ~PebbleClient();

public:
    /// @brief ��ʼ���ӿڣ�����Ҫ�������ã���������Options @see GetOptions
    /// @return 0 �ɹ�
    /// @return <0 ʧ��
    int32_t Init();

    int32_t Update();

    /// @brief ����һ��ָ�����͵�RPC stubʵ����ֻ���������գ�������Ҫ�û��ͷ�
    /// @param service_address ����ĵ�ַ����"udp://127.0.0.1:8880" @see Connect(url)
    /// @param processor_type ʹ�õ�RPC����
    /// @return RPC_CLIENT����ΪNULLʱ����ʧ�ܣ��´����Ķ�����Ҫ�û��ͷ�
    template<class RPC_CLIENT>
    RPC_CLIENT* NewRpcClientByAddress(const std::string& service_address,
        ProcessorType processor_type = kPEBBLE_RPC_BINARY);

    /// @brief ����һ��ָ�����͵�RPC stubʵ����ֻ���������գ�������Ҫ�û��ͷ�
    /// @param service_name ��������֣�Ĭ�ϸ�������Ѱַ
    /// @param processor_type ʹ�õ�RPC����
    /// @param router ·������ʹ������ѰַʱĬ�ϲ�����ѯ���ԣ��û��ɻ�ȡrouterʵ�����޸�·�ɲ���
    /// @return RPC_CLIENT����ΪNULLʱ����ʧ�ܣ��´����Ķ�����Ҫ�û��ͷ�
    template<class RPC_CLIENT>
    RPC_CLIENT* NewRpcClientByName(const std::string& service_name,
        ProcessorType processor_type = kPEBBLE_RPC_BINARY, Router** router = NULL);

    /// @brief ����Pebble�����ò������û��ɰ����޸�
    /// @return Pebble�����ò�������
    /// @note �����޸���init��reloadʱ��Ч
    Options* GetOptions() {
        return &m_options;
    }

    /// @brief ��ȡPebble�汾��Ϣ
    static const char* GetVersion();

public:
    // ����Ϊ�ǳ��ýӿ�
    int64_t Connect(const std::string &url);

    int32_t Close(int64_t handle);

    IProcessor* GetProcessor(ProcessorType processor_type);

    int32_t Attach(int64_t handle, IProcessor* processor);

    int32_t Attach(Router* router, IProcessor* processor);

    PebbleRpc* GetPebbleRpc(ProcessorType processor_type);

    Naming* GetNaming(NamingType naming_type = kNAMING_TBUSPP);

    Router* GetRouter(const std::string& name, RouterType router_type = kROUTER_TBUSPP);

    Timer* GetTimer() {
        return m_timer;
    }

    SessionMgr* GetSessionMgr();

    Stat* GetStat();

    /// @brief ��������Э�̵���������
    /// @return ��NULL �ɹ�
    /// @return NULL ʧ��
    CoroutineSchedule* GetCoroutineSchedule() {
        return m_coroutine_schedule;
    }

    /// @brief ����һ��Э�̣��������Ƿ�����ִ��
    /// @param routine Э��ִ����ں���
    /// @param start_immediately Ĭ������ִ�У�����Ϊfalseʱ����һ��updateʱִ��
    /// @return 0 �ɹ�
    /// @return <0 ʧ��
    int32_t MakeCoroutine(const cxx::function<void()>& routine, bool start_immediately = true);

private:
    int32_t ProcessMessage();

    void InitLog();

    int32_t InitCoSchedule();

    int32_t InitStat();

    int32_t InitTimer();

    int32_t OnStatTimeout();

    void OnRouterAddressChanged(const std::vector<int64_t>& handles, IProcessor* processor);

    void StatCoroutine(Stat* stat);

    void StatProcessorResource(Stat* stat);

private:
    Options            m_options;
    CoroutineSchedule* m_coroutine_schedule;
    Naming*            m_naming_array[kNAMING_BUTT];
    IProcessor*        m_processor_array[kPROCESSOR_TYPE_BUTT];
    IEventHandler*     m_rpc_event_handler;
    StatManager*       m_stat_manager;
    Timer*             m_timer;
    uint32_t           m_stat_timer_ms; // ��Դʹ�ò�����ʱ������ͳ����
    SessionMgr*        m_session_mgr;
    cxx::unordered_map<int64_t, IProcessor*> m_processor_map;
    cxx::unordered_map<std::string, Router*> m_router_map;
    static std::string m_version;
};

///////////////////////////////////////////////////////////////////////////////////////

template<class RPC_CLIENT>
RPC_CLIENT* PebbleClient::NewRpcClientByAddress(const std::string& service_address,
    ProcessorType processor_type) {
    PebbleRpc* pebble_rpc = GetPebbleRpc(processor_type);
    if (pebble_rpc == NULL) {
        PLOG_ERROR("GetPebbleRpc failed, processor_type: %d", processor_type);
        return NULL;
    }
    int64_t handle = Connect(service_address);
    if (handle < 0) {
        PLOG_ERROR("connect failed, handle: %ld, service_address: %s",
                   handle, service_address.c_str());
        return NULL;
    }
    if (Attach(handle, pebble_rpc) != 0) {
        PLOG_ERROR("Attach failed, handle: %ld, service_address: %s",
                   handle, service_address.c_str());
        Close(handle);
        return NULL;
    }
    RPC_CLIENT* client = new RPC_CLIENT(pebble_rpc);
    client->SetHandle(handle);
    return client;
}

template<class RPC_CLIENT>
RPC_CLIENT* PebbleClient::NewRpcClientByName(const std::string& service_name,
    ProcessorType processor_type, Router** router) {
    PebbleRpc* pebble_rpc = GetPebbleRpc(processor_type);
    if (pebble_rpc == NULL) {
        PLOG_ERROR("GetPebbleRpc failed, processor_type: %d", processor_type);
        return NULL;
    }
    Router* new_router = GetRouter(service_name);
    if (new_router == NULL) {
        PLOG_ERROR("GetRouter failed");
        return NULL;
    }
    if (Attach(new_router, pebble_rpc) != 0) {
        PLOG_ERROR("Attach failed.");
        return NULL;
    }
    RPC_CLIENT* client = new RPC_CLIENT(pebble_rpc);
    client->SetRouteFunction(cxx::bind(&Router::GetRoute, new_router, cxx::placeholders::_1));
    if (router != NULL) {
        *router = new_router;
    }
    return client;
}

}  // namespace pebble
#endif   //  _PEBBLE_CLIENT_PEBBLE_CLIENT_H_

