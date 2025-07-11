/*   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   Copyright (C) 2014-2016 Sean Wang <sean.wang@mediatek.com>
 *   Copyright (C) 2016-2017 John Crispin <blogic@openwrt.org>
 */

#ifndef NF_HNAT_MTK_H
#define NF_HNAT_MTK_H

#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include "../mtk_eth_soc.h"

#define HNAT_SKB_CB2(__skb) ((struct hnat_skb_cb2 *)&((__skb)->cb[44]))
#define MTK_FE_START_RESET	0x2000
#define MTK_FE_RESET_DONE	0x2001
#define MTK_WIFI_RESET_DONE	0x2002
#define MTK_WIFI_CHIP_ONLINE 	0x2003
#define MTK_WIFI_CHIP_OFFLINE 	0x2004
#define MTK_FE_RESET_NAT_DONE	0x4001
struct hnat_skb_cb2 {
	__u32 magic;
};

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
struct hnat_desc {
	u32 entry : 15;
	u32 filled : 3;
	u32 crsn : 5;
	u32 resv1 : 3;
	u32 sport : 4;
	u32 resv2 : 1;
	u32 alg : 1;
	u32 iface : 8;
	u32 wdmaid : 2;
	u32 rxid : 2;
	u32 wcid : 16;
	u32 bssid : 8;
	u32 usr_info : 16;
	u32 tid : 4;
	u32 is_fixedrate : 1;
	u32 is_prior : 1;
	u32 is_sp : 1;
	u32 hf : 1;
	u32 amsdu : 1;
	u32 resv3 : 19;
	u32 magic_tag_protect : 16;
} __packed;
#elif defined(CONFIG_MEDIATEK_NETSYS_RX_V2)
struct hnat_desc {
	u32 entry : 15;
	u32 filled : 3;
	u32 crsn : 5;
	u32 resv1 : 3;
	u32 sport : 4;
	u32 resv2 : 1;
	u32 alg : 1;
	u32 iface : 8;
	u32 wdmaid : 2;
	u32 rxid : 2;
	u32 wcid : 10;
	u32 bssid : 6;
	u32 resv5 : 20;
	u32 magic_tag_protect : 16;
} __packed;
#else
struct hnat_desc {
	u32 entry : 14;
	u32 crsn : 5;
	u32 sport : 4;
	u32 alg : 1;
	u32 iface : 8;
	u32 filled : 3;
	u32 resv : 1;
	u32 magic_tag_protect : 16;
	u32 wdmaid : 8;
	u32 rxid : 2;
	u32 wcid : 10;
	u32 bssid : 6;
} __packed;
#endif

#define HQOS_MAGIC_TAG 0x5678
#define HAS_HQOS_MAGIC_TAG(skb) (qos_toggle && skb->protocol == HQOS_MAGIC_TAG)

#define HNAT_MAGIC_TAG 0x6789
#define WIFI_INFO_LEN 6
#define FOE_INFO_LEN (10 + WIFI_INFO_LEN)
#define IS_SPACE_AVAILABLE_HEAD(skb)                                           \
	((((skb_headroom(skb) >= FOE_INFO_LEN) ? 1 : 0)))

#define skb_hnat_info(skb) ((struct hnat_desc *)(skb->head))
#define skb_hnat_magic(skb) (((struct hnat_desc *)(skb->head))->magic)
#define skb_hnat_reason(skb) (((struct hnat_desc *)(skb->head))->crsn)
#define skb_hnat_entry(skb) (((struct hnat_desc *)(skb->head))->entry)
#define skb_hnat_sport(skb) (((struct hnat_desc *)(skb->head))->sport)
#define skb_hnat_alg(skb) (((struct hnat_desc *)(skb->head))->alg)
#define skb_hnat_iface(skb) (((struct hnat_desc *)(skb->head))->iface)
#define skb_hnat_filled(skb) (((struct hnat_desc *)(skb->head))->filled)
#define skb_hnat_magic_tag(skb) (((struct hnat_desc *)((skb)->head))->magic_tag_protect)
#define skb_hnat_wdma_id(skb) (((struct hnat_desc *)((skb)->head))->wdmaid)
#define skb_hnat_rx_id(skb) (((struct hnat_desc *)((skb)->head))->rxid)
#define skb_hnat_wc_id(skb) (((struct hnat_desc *)((skb)->head))->wcid)
#define skb_hnat_bss_id(skb) (((struct hnat_desc *)((skb)->head))->bssid)
#define skb_hnat_usr_info(skb) (((struct hnat_desc *)((skb)->head))->usr_info)
#define skb_hnat_tid(skb) (((struct hnat_desc *)((skb)->head))->tid)
#define skb_hnat_is_fixedrate(skb)				\
	(((struct hnat_desc *)((skb)->head))->is_fixedrate)
#define skb_hnat_is_prior(skb) (((struct hnat_desc *)((skb)->head))->is_prior)
#define skb_hnat_is_sp(skb) (((struct hnat_desc *)((skb)->head))->is_sp)
#define skb_hnat_hf(skb) (((struct hnat_desc *)((skb)->head))->hf)
#define skb_hnat_amsdu(skb) (((struct hnat_desc *)((skb)->head))->amsdu)
#define skb_hnat_ppe2(skb)						\
	((skb_hnat_sport(skb) == NR_GMAC3_PORT) && (CFG_PPE_NUM >= 3))
#define skb_hnat_ppe1(skb)						\
	((skb_hnat_sport(skb) == NR_GMAC2_PORT) && (CFG_PPE_NUM >= 2))
#define skb_hnat_ppe(skb)						\
	(skb_hnat_ppe2(skb) ? 2 : (skb_hnat_ppe1(skb) ? 1 : 0))
#define headroom_iface(h) (h.iface)
#define headroom_ppe1(h)						\
		((headroom_iface(h) == FOE_MAGIC_GE_LAN2 ||		\
		 headroom_iface(h) == FOE_MAGIC_WED2) && CFG_PPE_NUM == 3)
#define headroom_ppe2(h)						\
	((headroom_iface(h) == FOE_MAGIC_GE_LAN2 ||			\
	 headroom_iface(h) == FOE_MAGIC_WED2) && CFG_PPE_NUM == 3)
#define headroom_ppe(h) \
	(headroom_ppe2(h) ? 2 : (headroom_ppe1(h) ? 1 : 0))

#define do_ext2ge_fast_try(dev, skb)						\
	(IS_EXT(dev) && !is_from_extge(skb))
#define set_from_extge(skb) (HNAT_SKB_CB2(skb)->magic = 0x78786688)
#define clr_from_extge(skb) (HNAT_SKB_CB2(skb)->magic = 0x0)
#define set_to_ppe(skb) (HNAT_SKB_CB2(skb)->magic = 0x78681415)
#define is_from_extge(skb) (HNAT_SKB_CB2(skb)->magic == 0x78786688)
#define is_magic_tag_valid(skb) (skb_hnat_magic_tag(skb) == HNAT_MAGIC_TAG)
#define set_from_mape(skb) (HNAT_SKB_CB2(skb)->magic = 0x78787788)
#define is_from_mape(skb) (HNAT_SKB_CB2(skb)->magic == 0x78787788)
#define is_unreserved_port(hdr)						       \
	((ntohs(hdr->source) > 1023) && (ntohs(hdr->dest) > 1023))

#define TTL_0 0x02
#define HAS_OPTION_HEADER 0x03
#define NO_FLOW_IS_ASSIGNED 0x07
#define IPV4_WITH_FRAGMENT 0x08
#define IPV4_HNAPT_DSLITE_WITH_FRAGMENT 0x09
#define IPV4_HNAPT_DSLITE_WITHOUT_TCP_UDP 0x0A
#define IPV6_5T_6RD_WITHOUT_TCP_UDP 0x0B
#define TCP_FIN_SYN_RST                                                        \
	0x0C /* Ingress packet is TCP fin/syn/rst (for IPv4 NAPT/DS-Lite or IPv6 5T-route/6RD) */
#define UN_HIT 0x0D /* FOE Un-hit */
#define HIT_UNBIND 0x0E /* FOE Hit unbind */
#define HIT_UNBIND_RATE_REACH 0x0F
#define HIT_BIND_TCP_FIN 0x10
#define HIT_BIND_TTL_1 0x11
#define HIT_BIND_WITH_VLAN_VIOLATION 0x12
#define HIT_BIND_KEEPALIVE_UC_OLD_HDR 0x13
#define HIT_BIND_KEEPALIVE_MC_NEW_HDR 0x14
#define HIT_BIND_KEEPALIVE_DUP_OLD_HDR 0x15
#define HIT_BIND_FORCE_TO_CPU 0x16
#define HIT_BIND_WITH_OPTION_HEADER 0x17
#define HIT_BIND_MULTICAST_TO_CPU 0x18
#define HIT_BIND_MULTICAST_TO_GMAC_CPU 0x19
#define HIT_PRE_BIND 0x1A
#define HIT_BIND_PACKET_SAMPLING 0x1B
#define HIT_BIND_EXCEED_MTU 0x1C

#define TPORT_ID(x) ((x) & GENMASK(3, 0))
#define TOPS_ENTRY(x) ((x) & GENMASK(5, 0))
#define CDRT_ID(x) ((x) & GENMASK(7, 0))

u32 hnat_tx(struct sk_buff *skb);
u32 hnat_set_skb_info(struct sk_buff *skb, u32 *rxd);
u32 hnat_reg(struct net_device *, void __iomem *);
u32 hnat_unreg(void);

#endif
