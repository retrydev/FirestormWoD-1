////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2015 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

# ifndef _BATTLEPAY_PACKET_FACTORY_HPP
# define _BATTLEPAY_PACKET_FACTORY_HPP

#include "Common.h"
#include "BattlepayMgr.h"

namespace Battlepay
{
    namespace PacketFactory
    {
        /// Client error enum
        /// See Blizzard_StoreUISecure.lua
        /// Last update : 6.0.3 19116
        enum Error
        {
            InvalidPaymentMethod       = 25,
            PaymentFailed              = 2,
            WrongCurrency              = 12,
            BattlepayDisabled          = 13,
            InsufficientBalance        = 28,
            OtherOK                    = 0,     ///< They are lot of id for "other" case, but they are used only in internal/GM client
            OtherCancelByUser          = 4,
            //AlreadyOwned             = 0,     ///< This error is client-side only, can't be sended by the server
            ParentalControlsNoPurchase = 34,
            Denied                     = 1
        };

        /// Result of SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE
        /// see @BattlePay::PacketFactory::SendProductList
        namespace ProductListResult
        {
            enum
            {
                Available    = 0,       ///< The shop is available
                LockUnk1     = 1,       ///< The shop is locked, unknow reason (i've see nothing in client for case "1", all result > 0 lock the shop anyway)
                LockUnk2     = 2,       ///< The shop is locked, unknow reason (i've see nothing in client for case "1", all result > 0 lock the shop anyway)
                RegionLocked = 3        ///< The shop is locked because the region of the player is locked
            };
        }

        /// Update status of SMSG_BATTLE_PAY_PURCHASE_UPDATE
        /// see @Battlepay::PacketFactory::SendPurchaseUpdate
        namespace UpdateStatus
        {
            enum
            {
                Loading = 2,
                Ready   = 3,
                Finish  = 6
            };
        }

        /*
        * Write display info into packet bytebuffer
        * @param p_DisplayInfoID: Id of the display info to write
        * @param p_Packet: The packet where we write display info data
        */
        void WriteDisplayInfo(uint32 p_DisplayInfoID, WorldPacket& p_Packet);

        /*
        * Build & send a battlepay start purchase response packet to the client
        * @param p_Session  : Packet will be sent to that WorldSession
        * @param p_Purchase : Purchase we want send the start response of
        * @param p_Error    : Error / Result of the start purchase, see @Battlepay::PacketFactory::Error enum
        */
        void SendStartPurchaseResponse(WorldSession* p_Session, Battlepay::Purchase const& p_Purchase, Battlepay::PacketFactory::Error const& p_Error);

        /*
        * Build & send a battlepay purchase update
        * @param p_Session  : Packet will be sent to that WorldSession
        * @param p_Purchase : Purchase we want made a update of
        * @param p_Result   : Result of the update, need to be reverse
        */
        void SendPurchaseUpdate(WorldSession* p_Session, Battlepay::Purchase const& p_Purchase, uint32 p_Result);

        /*
        * Build & send a battlepay purchase confirm
        * @param p_Session  : Packet will be sent to that WorldSession
        * @param p_Purchase : Purchase we want made a confirm notification of
        */
        void SendConfirmPurchase(WorldSession* p_Session, Battlepay::Purchase const& p_Purchase);

        /*
        * Build & send the battlepay product list
        * @param p_Session  : Packet will be sent to that WorldSession
        */
        void SendProductList(WorldSession* p_Session);

        /*
        * NIY, send dummy packet (client need it to show store)
        * Build & send the battlepay purchase list
        * @param p_Session  : Packet will be sent to that WorldSession
        */
        void SendPurchaseList(WorldSession* p_Session);

        /*
        * NIY, send dummy packet (client need it to show store)
        * Build & send the battlepay distribution list
        * @param p_Session  : Packet will be sent to that WorldSession
        */
        void SendDistributionList(WorldSession* p_Session);
    }
}

# endif