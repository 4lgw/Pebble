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


#ifndef PEBBLE_EXTENSION_EXTENSION_H
#define PEBBLE_EXTENSION_EXTENSION_H

//#include "extension/tbuspp/tbuspp_message.h"
//#include "extension/tbuspp/tbuspp_naming.h"
//#include "extension/tbuspp/tbuspp_router.h"
#include "extension/zookeeper/zookeeper_naming.h"
#include "common/log.h"


// ��չ�����װ : ����Pebble����չ���֣�Ϊ������ʽ��������Ҫ��ʹ��ʱ��ʽ��װ����ʹ��ʱ���ð�װ

/// @brief ��װTbuspp
/// @param pebble_server PebbleServerʵ��
#define INSTALL_TBUSPP(ret) \
    do { \
        (ret) = pebble::TbusppMessage::Instance()->Init(); \
        if ((ret) != 0) { \
            PLOG_ERROR("TbusppMessage Init failed, ret_code: %d, ret_msg: %s", \
                       ret, pebble::TbusppMessage::Instance()->GetLastError()); \
            break; \
        } \
        pebble::Message::SetMessageDriver(pebble::TbusppMessage::Instance()); \
        pebble::NamingFactory* naming_factory = new pebble::TbusppNamingFactory(); \
        PLOG_IF_ERROR(naming_factory == NULL, "invalid TbusppNamingFactory"); \
        ret = pebble::SetNamingFactory(pebble::kNAMING_TBUSPP, naming_factory); \
        if ((ret) != 0) { \
            PLOG_ERROR("SetNamingFactory failed, ret_code: %d", ret); \
            delete naming_factory; \
            break; \
        } \
        pebble::RouterFactory* router_factory = new pebble::TbusppRouterFactory(); \
        PLOG_IF_ERROR(router_factory == NULL, "invalid TbusppRouterFactory"); \
        (ret) = pebble::SetRouterFactory(pebble::kROUTER_TBUSPP, router_factory); \
        if ((ret) != 0) { \
            PLOG_ERROR("SetRouterFactory failed, ret_code: %d", ret); \
            delete router_factory; \
            break; \
        } \
    } while (0)

// @brief ��װzookeeper���ַ���(ʹ�ù㲥����ʱ��Ҫ)
/// @param pebble_server PebbleServerʵ��
#define INSTALL_ZOOKEEPER_NAMING(ret) \
    do { \
        pebble::NamingFactory* naming_factory = new pebble::ZookeeperNamingFactory(); \
        PLOG_IF_ERROR(naming_factory == NULL, "invalid ZookeeperNamingFactory"); \
        (ret) = pebble::SetNamingFactory(pebble::kNAMING_ZOOKEEPER, naming_factory); \
        if ((ret) != 0) { \
            PLOG_ERROR("SetNamingFactory failed, ret_code: %d", ret); \
            delete naming_factory; \
            break; \
        } \
    } while (0)


namespace pebble {
} // namespace pebble

#endif // PEBBLE_EXTENSION_EXTENSION_H

