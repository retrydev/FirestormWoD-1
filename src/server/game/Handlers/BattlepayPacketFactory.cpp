////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2015 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "BattlepayPacketFactory.h"

namespace Battlepay
{
    namespace PacketFactory
    {
        void SendProductList(WorldSession* p_Session)
        {
            WorldPacket l_Data(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE);
            l_Data << uint32(ProductListResult::Available);                         ///< Result
            l_Data << uint32(sBattlepayMgr->GetShopCurrency());                     ///< CurrencyID

            l_Data << uint32(sBattlepayMgr->GetProducts().size());
            l_Data << uint32(sBattlepayMgr->GetProductGroups().size());
            l_Data << uint32(sBattlepayMgr->GetShopEntries().size());

            for (auto& l_Iterator : sBattlepayMgr->GetProducts())
            {
                Battlepay::Product const& l_Product = l_Iterator.second;

                l_Data << uint32(l_Product.ProductID);
                l_Data << uint64(l_Product.NormalPriceFixedPoint);
                l_Data << uint64(l_Product.CurrentPriceFixedPoint);
                l_Data << uint32(l_Product.Items.size());
                l_Data << uint8(l_Product.Type);
                l_Data << uint32(l_Product.Flags);

                for (auto& l_ItemProduct : l_Product.Items)
                {
                    l_Data << uint32(l_ItemProduct.ID);
                    l_Data << uint32(l_ItemProduct.ItemID);
                    l_Data << uint32(l_ItemProduct.Quantity);

                    l_Data.FlushBits();

                    l_Data.WriteBit(l_ItemProduct.DisplayInfoID != 0);
                    l_Data.WriteBit(l_ItemProduct.HasPet);
                    l_Data.WriteBit(l_ItemProduct.PetResult != 0);

                    if (l_ItemProduct.PetResult != 0)
                        l_Data.WriteBits(l_ItemProduct.PetResult, 4);

                    if (l_ItemProduct.DisplayInfoID != 0)
                        WriteDisplayInfo(l_ItemProduct.DisplayInfoID, l_Data);
                }

                l_Data.FlushBits();
                l_Data.WriteBits(l_Product.ChoiceType, 2);
                l_Data.WriteBit(l_Product.DisplayInfoID != 0);

                if (l_Product.DisplayInfoID != 0)
                    WriteDisplayInfo(l_Product.DisplayInfoID, l_Data);
            }

            for (auto& l_ProductGroup : sBattlepayMgr->GetProductGroups())
            {
                l_Data << uint32(l_ProductGroup.GroupID);
                l_Data << uint32(l_ProductGroup.IconFileDataID);
                l_Data << uint8(l_ProductGroup.DisplayType);
                l_Data << int32(l_ProductGroup.Ordering);

                l_Data.FlushBits();

                l_Data.WriteBits(l_ProductGroup.Name.size(), 8);
                l_Data.WriteString(l_ProductGroup.Name);
            }

            for (auto& l_ShopEntry : sBattlepayMgr->GetShopEntries())
            {
                l_Data << uint32(l_ShopEntry.EntryID);
                l_Data << uint32(l_ShopEntry.GroupID);
                l_Data << uint32(l_ShopEntry.ProductID);
                l_Data << int32(l_ShopEntry.Ordering);
                l_Data << uint32(l_ShopEntry.Flags);
                l_Data << uint8(l_ShopEntry.BannerType);

                l_Data.FlushBits();
                l_Data.WriteBit(l_ShopEntry.DisplayInfoID != 0);

                if (l_ShopEntry.DisplayInfoID != 0)
                    Battlepay::PacketFactory::WriteDisplayInfo(l_ShopEntry.DisplayInfoID, l_Data);
            }

            p_Session->SendPacket(&l_Data);
        }

        void WriteDisplayInfo(uint32 p_DisplayInfoID, WorldPacket& p_Packet)
        {
            DisplayInfo const* l_DisplayInfo = sBattlepayMgr->GetDisplayInfo(p_DisplayInfoID);
            if (l_DisplayInfo == nullptr)
                return;

            p_Packet.FlushBits();
            p_Packet.WriteBit(l_DisplayInfo->CreatureDisplayInfoID != 0);
            p_Packet.WriteBit(l_DisplayInfo->FileDataID != 0);
            p_Packet.WriteBits(l_DisplayInfo->Name1.size(), 10);
            p_Packet.WriteBits(l_DisplayInfo->Name2.size(), 10);
            p_Packet.WriteBits(l_DisplayInfo->Name3.size(), 13);
            p_Packet.WriteBit(l_DisplayInfo->Flags != 0);

            if (l_DisplayInfo->CreatureDisplayInfoID != 0)
                p_Packet << uint32(l_DisplayInfo->CreatureDisplayInfoID);

            if (l_DisplayInfo->FileDataID != 0)
                p_Packet << uint32(l_DisplayInfo->FileDataID);

            p_Packet.WriteString(l_DisplayInfo->Name1);
            p_Packet.WriteString(l_DisplayInfo->Name2);
            p_Packet.WriteString(l_DisplayInfo->Name3);

            if (l_DisplayInfo->Flags != 0)
                p_Packet << uint32(l_DisplayInfo->Flags);
        }

        void SendStartPurchaseResponse(WorldSession* p_Session, Battlepay::Purchase const& p_Purchase, Battlepay::PacketFactory::Error const& p_Error)
        {
            WorldPacket l_Data(SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE);
            l_Data << uint64(p_Purchase.PurchaseID);     ///< Purchase ID
            l_Data << uint32(p_Error);                   ///< Result
            l_Data << uint32(p_Purchase.ClientToken);    ///< Client Token

            p_Session->SendPacket(&l_Data);
        }

        void SendPurchaseUpdate(WorldSession* p_Session, Battlepay::Purchase const& p_Purchase, uint32 p_Result)
        {
            /// At the moment, we only use the default wallet (points)
            std::string l_WalletName = sBattlepayMgr->GetDefaultWalletName();

            WorldPacket l_Data(SMSG_BATTLE_PAY_PURCHASE_UPDATE);
            l_Data << uint32(1);               ///< Purchase counter

            /// BattlePayPurchase foreach
            {
                l_Data << uint64(p_Purchase.PurchaseID);
                l_Data << uint32(p_Purchase.Status);      ///< Need to reverse status (2 : Search wallets in database, 3 : ready)
                l_Data << uint32(p_Result);               ///< Result code (see @Error)
                l_Data << uint32(p_Purchase.ProductID);   ///< Product ID
                l_Data.WriteBits(l_WalletName.size(), 8);
                l_Data.WriteString(l_WalletName);
            }

            p_Session->SendPacket(&l_Data);
        }

        void SendConfirmPurchase(WorldSession* p_Session, Battlepay::Purchase const& p_Purchase)
        {
            WorldPacket l_Data(SMSG_BATTLE_PAY_CONFIRM_PURCHASE);
            l_Data << uint64(p_Purchase.PurchaseID);
            l_Data << uint64(p_Purchase.CurrentPrice);
            l_Data << uint32(p_Purchase.ServerToken);

            p_Session->SendPacket(&l_Data);
        }

        /// @TODO
        void SendPurchaseList(WorldSession* p_Session)
        {
            WorldPacket l_Data(SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE);
            l_Data << uint32(0);    ///< Result
            l_Data << uint32(0);    ///< Purchase count

            p_Session->SendPacket(&l_Data);
        }

        /// @TODO
        void SendDistributionList(WorldSession* p_Session)
        {
            WorldPacket l_Data(SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE);
            l_Data << uint32(0);    ///< Result
            l_Data << uint32(0);    ///< Count

            p_Session->SendPacket(&l_Data);
        }
    }
}