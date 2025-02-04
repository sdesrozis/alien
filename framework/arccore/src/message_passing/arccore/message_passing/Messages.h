﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2020 IFPEN-CEA
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* Messages.h                                                  (C) 2000-2020 */
/*                                                                           */
/* Interface du gestionnaire des échanges de messages.                       */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_MESSAGEPASSING_MESSAGES_H
#define ARCCORE_MESSAGEPASSING_MESSAGES_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/message_passing/IMessagePassingMng.h"
#include "arccore/message_passing/IDispatchers.h"
#include "arccore/message_passing/ITypeDispatcher.h"
#include "arccore/message_passing/Request.h"
#include "arccore/base/RefDeclarations.h"
#include "arccore/base/Span.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore::MessagePassing
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(type)                                                               \
  inline void mpAllGather(IMessagePassingMng* pm, Span<const type> send_buf, Span<type> recv_buf)                     \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->allGather(send_buf, recv_buf);                                                  \
  }                                                                                                                   \
  inline void mpGather(IMessagePassingMng* pm, Span<const type> send_buf, Span<type> recv_buf, Int32 rank)            \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->gather(send_buf, recv_buf, rank);                                               \
  }                                                                                                                   \
  inline void mpAllGatherVariable(IMessagePassingMng* pm, Span<const type> send_buf, Array<type>& recv_buf)           \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->allGatherVariable(send_buf, recv_buf);                                          \
  }                                                                                                                   \
  inline void mpGatherVariable(IMessagePassingMng* pm, Span<const type> send_buf, Array<type>& recv_buf, Int32 rank)  \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->gatherVariable(send_buf, recv_buf, rank);                                       \
  }                                                                                                                   \
  inline void mpScatterVariable(IMessagePassingMng* pm, Span<const type> send_buf, Span<type> recv_buf, Int32 root);  \
  inline type mpAllReduce(IMessagePassingMng* pm, eReduceType rt, type v)                                             \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    return pm->dispatchers()->dispatcher(x)->allReduce(rt, v);                                                        \
  }                                                                                                                   \
  inline void mpAllReduce(IMessagePassingMng* pm, eReduceType rt, Span<type> v)                                       \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->allReduce(rt, v);                                                               \
  }                                                                                                                   \
  inline void mpBroadcast(IMessagePassingMng* pm, Span<type> send_buf, Int32 rank)                                    \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->broadcast(send_buf, rank);                                                      \
  }                                                                                                                   \
  inline void mpSend(IMessagePassingMng* pm, Span<const type> values, Int32 rank)                                     \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->send(values, rank, true);                                                       \
  }                                                                                                                   \
  inline void mpReceive(IMessagePassingMng* pm, Span<type> values, Int32 rank)                                        \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    pm->dispatchers()->dispatcher(x)->receive(values, rank, true);                                                    \
  }                                                                                                                   \
  inline Request mpSend(IMessagePassingMng* pm, Span<const type> values, Int32 rank, bool is_blocked)                 \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    return pm->dispatchers()->dispatcher(x)->send(values, rank, is_blocked);                                          \
  }                                                                                                                   \
  inline Request mpSend(IMessagePassingMng* pm, Span<const type> values, const PointToPointMessageInfo& message)      \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    return pm->dispatchers()->dispatcher(x)->send(values, message);                                                   \
  }                                                                                                                   \
  inline Request mpReceive(IMessagePassingMng* pm, Span<type> values, Int32 rank, bool is_blocked)                    \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    return pm->dispatchers()->dispatcher(x)->receive(values, rank, is_blocked);                                       \
  }                                                                                                                   \
  inline Request mpReceive(IMessagePassingMng* pm, Span<type> values, const PointToPointMessageInfo& message)         \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    return pm->dispatchers()->dispatcher(x)->receive(values, message);                                                \
  }                                                                                                                   \
  inline void mpAllToAll(IMessagePassingMng* pm, Span<const type> send_buf, Span<type> recv_buf, Int32 count)         \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    return pm->dispatchers()->dispatcher(x)->allToAll(send_buf, recv_buf, count);                                     \
  }                                                                                                                   \
  inline void mpAllToAllVariable(IMessagePassingMng* pm, Span<const type> send_buf, ConstArrayView<Int32> send_count, \
                                 ConstArrayView<Int32> send_index, Span<type> recv_buf,                               \
                                 ConstArrayView<Int32> recv_count, ConstArrayView<Int32> recv_index)                  \
  {                                                                                                                   \
    type* x = nullptr;                                                                                                \
    auto d = pm->dispatchers()->dispatcher(x);                                                                        \
    d->allToAllVariable(send_buf, send_count, send_index, recv_buf, recv_count, recv_index);                          \
  }

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Créé une liste de requêtes.
 *
 * \sa IRequestList
 */
ARCCORE_MESSAGEPASSING_EXPORT Ref<IRequestList>
mpCreateRequestListRef(IMessagePassingMng* pm);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCCORE_MESSAGEPASSING_EXPORT void
mpWaitAll(IMessagePassingMng* pm, ArrayView<Request> requests);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Bloque jusqu'à ce que la requête \a request soit terminée.
 */
ARCCORE_MESSAGEPASSING_EXPORT void
mpWait(IMessagePassingMng* pm, Request request);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Bloque jusqu'à ce qu'au moins une des requêtes de \a request soit terminée.
 *
 * En retour, le tableaux \a indexes contient la valeur \a true pour indiquer
 * qu'une requête est terminée.
 */
ARCCORE_MESSAGEPASSING_EXPORT void
mpWaitSome(IMessagePassingMng* pm, ArrayView<Request> requests, ArrayView<bool> indexes);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Teste si des requêtes de \a request sont terminées.
 *
 * En retour, le tableaux \a indexes contient la valeur \a true pour indiquer
 * qu'une requête est terminée.
 */
ARCCORE_MESSAGEPASSING_EXPORT void
mpTestSome(IMessagePassingMng* pm, ArrayView<Request> requests, ArrayView<bool> indexes);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Fonction générale d'attente de terminaison de requête.
 *
 * En fonction de la valeur de \a wait_type, appelle mpWait(), mpWaitSome(), ou
 * mpTestSome().
 */
ARCCORE_MESSAGEPASSING_EXPORT void
mpWait(IMessagePassingMng* pm, ArrayView<Request> requests,
       ArrayView<bool> indexes, eWaitType wait_type);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Teste si un message est disponible.
 *
 * Cette fonction permet de savoir si un message issu du couple (rang,tag)
 * est disponible. \a message doit avoir été initialisé avec un couple (rang,tag)
 * (message.isRankTag() doit être vrai).
 *
 * Retourne une instance de \a MessageId.
 *
 * En mode non bloquant, si aucun message n'est disponible, alors
 * MessageId::isValid() vaut \a false pour l'instance retournée.
 */
ARCCORE_MESSAGEPASSING_EXPORT MessageId
mpProbe(IMessagePassingMng* pm, const PointToPointMessageInfo& message);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Créé une nouvelle instance de \a IMessagePassingMng.
 *
 * \a keep est vrai si ce rang est présent dans le nouveau communicateur.
 *
 * L'instance retournée doit être détruite par l'appel à l'opérateur
 * operator delele().
 */
ARCCORE_MESSAGEPASSING_EXPORT IMessagePassingMng*
mpSplit(IMessagePassingMng* pm, bool keep);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Effectue une barrière
 *
 * Bloque tant que tous les rangs n'ont pas atteint cette appel.
 */
ARCCORE_MESSAGEPASSING_EXPORT void
mpBarrier(IMessagePassingMng* pm);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Créé une liste de messages de sérialisation.
 *
 * \sa ISerializeMessageList
 */
ARCCORE_MESSAGEPASSING_EXPORT Ref<ISerializeMessageList>
mpCreateSerializeMessageListRef(IMessagePassingMng* pm);

//! Message d'envoi utilisant un ISerializer.
ARCCORE_MESSAGEPASSING_EXPORT Request
mpSend(IMessagePassingMng* pm, const ISerializer* values, const PointToPointMessageInfo& message);

//! Message de réception utilisant un ISerializer.
ARCCORE_MESSAGEPASSING_EXPORT Request
mpReceive(IMessagePassingMng* pm, ISerializer* values, const PointToPointMessageInfo& message);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(char)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(signed char)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(unsigned char)

ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(short)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(unsigned short)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(int)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(unsigned int)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(long)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(unsigned long)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(long long)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(unsigned long long)

ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(float)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(double)
ARCCORE_GENERATE_MESSAGEPASSING_PROTOTYPE(long double)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore::MessagePassing

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
